//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include "cfs_dos2.h"
#include "autil.h"

#define ROOT_DIR 361

CDos2::CDos2() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CDos2 constructed: %08X\n", this );
	#endif
}

CDos2::~CDos2()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CDos2 destructed: %08X\n", this );
	#endif
}

BOOL CDos2::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;

	m_pDisk = pDisk;
	m_pRoot = NULL;

	DOS2_DIRENT dire;

	WORD wEntry = 0;

	CDos2DirEntry* pPrev = NULL;

	do
	{
		BYTE abtSec[ 0x100 ];

		if ( !m_pDisk->ReadSector( abtSec, ROOT_DIR + ( wEntry / 8 ) ) )
		{
			sprintf( m_szLastError, "DOS2: Can't read directory entry %04X because\n%s", wEntry, m_pDisk->GetLastError() );
			return FALSE;
		}

		//this is not endian-safe
		//memcpy( &dire, abtSec + ( wEntry % 8) * sizeof( DOS2_DIRENT ), sizeof( DOS2_DIRENT ) );
		BYTE* pTmp = abtSec + ( wEntry % 8) * sizeof( DOS2_DIRENT );
		dire.btFlags = MGET_B( pTmp );
		dire.wSecCount = MGET_LEW( pTmp );
		dire.wSecStart = MGET_LEW( pTmp );
		memcpy( dire.acAtariName, pTmp, 11 );

		if ( !dire.btFlags )
			break;

		CDos2DirEntry* pE = CreateEntry( &dire, wEntry );

		if ( pE && ! ( pE->m_dwFlags & DIRE_DELETED ) )
		{
			if ( m_pRoot )
			{
				pPrev->m_pNext = pE;
				pE->m_pPrev = pPrev;
				pPrev = pE;
			}
			else
			{
				m_pRoot = pE;
				pPrev = pE;
			}

		}

		wEntry++;

	} while( dire.btFlags && ( wEntry < 64 ) );

	return TRUE;
}

void CDos2::Dismount()
{
	DeleteList( m_pRoot );
}

CDos2DirEntry* CDos2::CreateEntry( DOS2_DIRENT* pDire, WORD wEntry )
{
	CDos2DirEntry* pE = new CDos2DirEntry();

	if ( !pE )
	{
		sprintf( m_szLastError, "DOS2: Can't allocate memory for directory!" );
		return NULL;
	}

	if ( pDire->btFlags == 0x80 )
	{
		pE->m_dwFlags |= DIRE_DELETED;
	}

	ADos2MsDos( pE->m_szFname, pDire->acAtariName );

	sprintf( pE->m_szAscData, "%02X %04X %04X", pDire->btFlags, pDire->wSecStart, pDire->wSecCount );

	pE->m_wFileNumber = wEntry;
	pE->m_btFlags = pDire->btFlags;
	pE->m_wSecStart = pDire->wSecStart;
	pE->m_wSecCount = pDire->wSecCount;

	if ( pE->m_dwFlags & DIRE_DELETED )
	{
	}
	else if ( ExportFile( NULL, pE ) )
		m_iFilesValid++;
	else
		m_iFilesInvalid++;

	return pE;
}

BOOL CDos2::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	int hOutfile = -1;

	if ( szOutFile )
	{
		hOutfile = open( szOutFile, O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE );

		if ( -1 == hOutfile )
		{
			sprintf( m_szLastError, "DOS2: Unable to create file '%s'!", szOutFile );
			return FALSE;
		}
	}

	BYTE abtBuff[ 0x0100 ];

	WORD wSector = ((CDos2DirEntry*)pDirE ) -> m_wSecStart;
	WORD wCount = ((CDos2DirEntry*)pDirE ) -> m_wSecCount;
	WORD wSectorSize = m_pDisk->GetSectorSize();
	WORD wFileNumber = ((CDos2DirEntry*)pDirE ) -> m_wFileNumber;

	abtBuff[ wSectorSize - 1 ] = 0;

	while( wCount )
	{
		if ( wSector < 1 )
		{
			sprintf( m_szLastError, "DOS2: Corrupted file '%s' (invalid sector %04X)", szOutFile, wSector );
			return FALSE;
		}

		if ( ( abtBuff[ wSectorSize - 1 ] & 0x80 ) && ( wSectorSize == 0x80 ) )
		{
			sprintf( m_szLastError, "DOS2: Corrupted file '%s' (unexpected EOF)", szOutFile );
			return FALSE;
		}

		if ( !m_pDisk->ReadSector( abtBuff, wSector ) )
		{
			sprintf( m_szLastError, "DOS2: Corrupted file '%s'\n%s\n", szOutFile, m_pDisk->GetLastError() );
			return FALSE;
		}

		/*
		if ( -1 != hOutfile )
		{
			printf( "%04X/%04X %02X | %02X %02X %02X\n", 
				wSector,
				wCount,
				wFileNumber,
				abtBuff[ wSectorSize - 3 ], 
				abtBuff[ wSectorSize - 2 ],
				abtBuff[ wSectorSize - 1 ] );
		}
		*/

		wSector = abtBuff[ wSectorSize - 2 ] + ( 0x03 & abtBuff[ wSectorSize - 3 ] ) * 0x100;

		if ( ( abtBuff[ wSectorSize-3 ] >> 2 ) != wFileNumber )
		{
			WORD wFN = abtBuff[ wSectorSize - 3 ] >> 2;

			sprintf( m_szLastError, "DOS2: Corrupted file '%s' (167: file number mismatch [%04X != %04X])", szOutFile, wFileNumber, wFN );
			return FALSE;
		}


		if ( -1 != hOutfile )
			write( hOutfile, abtBuff, abtBuff[ wSectorSize - 1 ] );

		wCount--;
	}

	if ( ((CDos2DirEntry*)pDirE ) -> m_wSecCount )
	{
		if ( ! ( abtBuff[ wSectorSize - 1 ] & 128 ) && ( wSectorSize == 128 ) && wSector)
		{
			sprintf( m_szLastError, "DOS2: Corrupted file '%s' (expected EOF, code %02X, next sector %04X)", szOutFile, abtBuff[ wSectorSize - 1 ], wSector );
			return FALSE;
		}
	}

	if ( -1 != hOutfile )
		close( hOutfile );

	return TRUE;
}

