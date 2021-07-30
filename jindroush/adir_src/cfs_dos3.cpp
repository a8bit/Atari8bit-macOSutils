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

#include "cfs_dos3.h"
#include "autil.h"

#define ROOT_DIR 0x10
#define FAT	0x18

CDos3::CDos3() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CDos3 constructed: %08X\n", this );
	#endif
}

CDos3::~CDos3()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CDos3 destructed: %08X\n", this );
	#endif
}

BOOL CDos3::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;
	m_pDisk = pDisk;
	m_pRoot = NULL;

	if ( m_pDisk->GetSectorSize() != 0x80 )
	{
		sprintf( m_szLastError, "DOS3: Can't process DD disk!" );
		return FALSE;
	}

	CDos3DirEntry* pPrev = NULL;

	DOS3_DIRENT dire;

	WORD wEntry = 1;

	if ( !pDisk->ReadSector( m_abtFat, FAT ) )
	{
		sprintf( m_szLastError, "DOS3: Can't read FAT sector because\n%s", m_pDisk->GetLastError() );
		return FALSE;
	}

	do
	{
		BYTE abtSec[ 0x100 ];

		if ( !pDisk->ReadSector( abtSec, ROOT_DIR + ( wEntry / 8 ) ) )
		{
			sprintf( m_szLastError, "DOS3: Can't read directory entry %04X because\n%s", wEntry, m_pDisk->GetLastError() );
			return FALSE;
		}

		memcpy( &dire, abtSec + ( wEntry % 8) * sizeof( DOS3_DIRENT ), sizeof( DOS3_DIRENT ) );

		if ( !dire.btFlags )
			break;

		CDos3DirEntry* pE = CreateEntry( &dire );

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

	} while( dire.btFlags );

	return TRUE;
}

void CDos3::Dismount()
{
	DeleteList( m_pRoot );
}

CDos3DirEntry* CDos3::CreateEntry( DOS3_DIRENT* pDire )
{
	CDos3DirEntry* pE = new CDos3DirEntry();

	if ( !pE )
		return NULL;

	if ( pDire->btFlags == 0x80 )
	{
		pE->m_dwFlags |= DIRE_DELETED;
	}

	ADos2MsDos( pE->m_szFname, pDire->acAtariName );

	//sprintf( pE->m_szAscData, "%02X %02X %02X %04X", pDire->btSecStart, pDire->btSecCount, pDire->btFlags, pDire->wFileLen );
	sprintf( pE->m_szAscData, "%02X %04X", pDire->btFlags, pDire->wFileLen );

	pE->m_btSecStart = pDire->btSecStart;
	pE->m_btSecCount = pDire->btSecCount;
	pE->m_btFlags = pDire->btFlags;
	pE->m_wFileLen = pDire->wFileLen;

	if ( pE->m_dwFlags & DIRE_DELETED )
	{
	}
	else if ( ExportFile( NULL, pE ) )
		m_iFilesValid++;
	else
		m_iFilesInvalid++;

	return pE;
}

BOOL CDos3::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	int hOutfile = -1;

	if ( szOutFile )
	{
		hOutfile = open( szOutFile, O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE );

		if ( -1 == hOutfile )
		{
			sprintf( m_szLastError, "DOS3: Unable to create file '%s'!", szOutFile );
			return FALSE;
		}
	}

	BYTE abtBuff[ 0x80 * 8 ];

	BYTE btSecCount = ((CDos3DirEntry*)pDirE) ->m_btSecCount;
	BYTE btSector = ((CDos3DirEntry*)pDirE) ->m_btSecStart;
	WORD wFileLen = ((CDos3DirEntry*)pDirE) ->m_wFileLen;

	while( btSecCount )
	{
		WORD wToCopy = ( wFileLen < 0x400 ) ? wFileLen : 0x400;

		if ( !m_pDisk->ReadSectors( abtBuff, ( btSector * 8 ) + FAT + 1, 8 ) )
		{
			{
				sprintf( m_szLastError, "DOS3: Corrupted file '%s'\n%s\n", szOutFile, m_pDisk->GetLastError() );
				return FALSE;
			}
		}

		if ( -1 != hOutfile )
			write( hOutfile, abtBuff, wToCopy );

		btSecCount--;
		wFileLen -= wToCopy;

		btSector = m_abtFat[ btSector ];

		if ( wToCopy == 0x400 )
		{
			if ( btSector >= 0xFD )
			{
				sprintf( m_szLastError, "DOS3: Corrupted file '%s' (unexpected EOF)", szOutFile );
				return FALSE;
			}

		}
		else
		{
			if ( btSector != 0xFD )
			{
				sprintf( m_szLastError, "DOS3: Corrupted file '%s' (missed EOF) %02X", szOutFile, btSector );
				return FALSE;
			}

		}


	}


	if ( -1 != hOutfile )
		close( hOutfile );

	return TRUE;
}

