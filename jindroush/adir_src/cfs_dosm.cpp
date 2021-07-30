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

#include "cfs_dosm.h"
#include "autil.h"

#define ROOT_DIR 361

CDosM::CDosM() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CDosM constructed: %08X\n", this );
	#endif
}

CDosM::~CDosM()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CDosM destructed: %08X\n", this );
	#endif
}

BOOL CDosM::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;

	m_pDisk = pDisk;
	m_pRoot = NULL;

	return ReadDir( ROOT_DIR, (CDosMDirEntry**)&m_pRoot );
}

BOOL CDosM::ReadDir( int iSector, CDosMDirEntry** ppRoot )
{
	DOSM_DIRENT dire;

	WORD wEntry = 0;

	CDosMDirEntry* pPrev = NULL;

	do
	{
		BYTE abtSec[ 0x100 ];

		//printf( "reading sec %d\n", iSector + ( wEntry / 8 ) );

		if ( !m_pDisk->ReadSector( abtSec, iSector + ( wEntry / 8 ) ) )
		{
			sprintf( m_szLastError, "DOSM: Can't read directory entry %04X because\n%s", wEntry, m_pDisk->GetLastError() );
			return FALSE;
		}

		//this is not endian-safe
		//memcpy( &dire, abtSec + ( wEntry % 8) * sizeof( DOSM_DIRENT ), sizeof( DOSM_DIRENT ) );
		BYTE* pTmp = abtSec + ( wEntry % 8) * sizeof( DOSM_DIRENT );
		dire.btFlags = MGET_B( pTmp );
		dire.wSecCount = MGET_LEW( pTmp );
		dire.wSecStart = MGET_LEW( pTmp );
		memcpy( dire.acAtariName, pTmp, 11 );

		//printf( "%p %p %04X %02X\n", pTmp, abtSec, wEntry, dire.btFlags );

		if ( !dire.btFlags )
			break;

		CDosMDirEntry* pE = CreateEntry( &dire );

		if ( pE && ! ( pE->m_dwFlags & DIRE_DELETED ) )
		{
			if ( *ppRoot )
			{
				pPrev->m_pNext = pE;
				pE->m_pPrev = pPrev;
				pPrev = pE;
			}
			else
			{
				*ppRoot = pE;
				pPrev = pE;
			}

		}

		wEntry++;

	} while( dire.btFlags && ( wEntry < 64 ) );

	return TRUE;
}

void CDosM::Dismount()
{
	DeleteList( m_pRoot );
}

CDosMDirEntry* CDosM::CreateEntry( DOSM_DIRENT* pDire )
{
	CDosMDirEntry* pE = new CDosMDirEntry();

	if ( !pE )
	{
		sprintf( m_szLastError, "DOSM: Can't allocate memory for directory!" );
		return NULL;
	}

	if ( pDire->btFlags == 0x80 )
	{
		pE->m_dwFlags |= DIRE_DELETED;
	}

	ADos2MsDos( pE->m_szFname, pDire->acAtariName );

	sprintf( pE->m_szAscData, "%02X %04X %04X", pDire->btFlags, pDire->wSecStart, pDire->wSecCount );
	//printf( "%02X %04X %04X %s\n", pDire->btFlags, pDire->wSecStart, pDire->wSecCount, pE->m_szFname );

	pE->m_btFlags = pDire->btFlags;
	pE->m_wSecStart = pDire->wSecStart;
	pE->m_wSecCount = pDire->wSecCount;

	if ( pDire->btFlags == 0x10 )
	{
		pE->m_dwFlags |= DIRE_SUBDIR;

		BOOL bRes = ReadDir( pE->m_wSecStart, (CDosMDirEntry**)&pE->m_pSubdir );

		if ( !bRes )
			return NULL;
	}
	else
	{
		if ( pE->m_dwFlags & DIRE_DELETED )
		{
		}
		else if ( ExportFile( NULL, pE ) )
		{
			m_iFilesValid++;
		}
		else
		{
			m_iFilesInvalid++;
		}
	}

	return pE;
}

BOOL CDosM::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	int hOutfile = -1;

	if ( szOutFile )
	{
		hOutfile = open( szOutFile, O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE );

		if ( -1 == hOutfile )
		{
			sprintf( m_szLastError, "DOSM: Unable to create file '%s'!", szOutFile );
			return FALSE;
		}
	}

	BYTE abtBuff[ 0x0100 ];

	WORD wSector = ((CDosMDirEntry*)pDirE ) -> m_wSecStart;
	WORD wCount = ((CDosMDirEntry*)pDirE ) -> m_wSecCount;
	WORD wSectorSize = m_pDisk->GetSectorSize();

	abtBuff[ wSectorSize - 1 ] = 0;

	while( wCount )
	{
		if ( wSector < 1 )
		{
			sprintf( m_szLastError, "DOSM: Corrupted file '%s' (invalid sector %04X)", szOutFile, wSector );
			return FALSE;
		}

		if ( ( abtBuff[ wSectorSize - 1 ] & 0x80 ) && ( wSectorSize == 0x80 ) )
		{
			sprintf( m_szLastError, "DOSM: Corrupted file '%s' (unexpected EOF)", szOutFile );
			return FALSE;
		}

		if ( !m_pDisk->ReadSector( abtBuff, wSector ) )
		{
			sprintf( m_szLastError, "DOSM: Corrupted file '%s'\n%s\n", szOutFile, m_pDisk->GetLastError() );
			return FALSE;
		}

//		if ( -1 != hOutfile )
//		{
//			printf( "%04X/%04X | %02X %02X %02X\n", 
//				wSector,
//				wCount,
//				abtBuff[ wSectorSize - 3 ], 
//				abtBuff[ wSectorSize - 2 ],
//				abtBuff[ wSectorSize - 1 ] );
//		}

		//originally, there was 0x07 as a mask, but megaimages were not working with it
		if ( m_pDisk->GetSectorCount() > 943 )
			wSector = abtBuff[ wSectorSize - 2 ] + ( (WORD)abtBuff[ wSectorSize - 3 ] & 0xff ) * 0x100;
		else
			wSector = abtBuff[ wSectorSize - 2 ] + ( 0x03 & (WORD)abtBuff[ wSectorSize - 3 ] ) * 0x100;

		if ( -1 != hOutfile )
			write( hOutfile, abtBuff, abtBuff[ wSectorSize - 1 ] );

		wCount--;
	}

	if ( ((CDosMDirEntry*)pDirE ) -> m_wSecCount )
	{
		if ( ! ( abtBuff[ wSectorSize - 1 ] & 128 ) && ( wSectorSize == 128 ) && wSector)
		{
			sprintf( m_szLastError, "DOSM: Corrupted file '%s' (expected EOF, code %02X, next sector %04X)", szOutFile, abtBuff[ wSectorSize - 1 ], wSector );
			return FALSE;
		}
	}

	if ( -1 != hOutfile )
		close( hOutfile );

	return TRUE;
}

