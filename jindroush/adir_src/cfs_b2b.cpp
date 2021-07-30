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

#include "cfs_b2b.h"
#include "autil.h"

CBas2Boot::CBas2Boot() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CBas2Boot constructed: %08X\n", this );
	#endif
}

CBas2Boot::~CBas2Boot()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CBas2Boot destructed: %08X\n", this );
	#endif
}

BOOL CBas2Boot::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;
	m_pDisk = pDisk;
	m_pRoot = NULL;

	if ( m_pDisk->GetSectorSize() != 0x80 )
	{
		sprintf( m_szLastError, "BAS2BOOT: Can't process DD disk!" );
		return FALSE;
	}

	m_pRoot = CreateEntry();

	return m_pRoot ? TRUE : FALSE;
}

void CBas2Boot::Dismount()
{
	DeleteList( m_pRoot );
}

CBas2BootDirEntry* CBas2Boot::CreateEntry()
{
	CBas2BootDirEntry* pE = new CBas2BootDirEntry();

	if ( !pE )
		return NULL;

	BYTE abtSec[ 0x100 ];

	if ( !m_pDisk->ReadSector( abtSec, 1 ) )
	{								
		sprintf( m_szLastError, "BAS2BOOT: Can't read boot sector because\n%s", m_pDisk->GetLastError() );
		delete pE;
		return NULL;
	}

	pE->m_dwFileLen = abtSec[ 8 ] + ( abtSec[ 9 ] << 8 );

	if ( !m_pDisk->ReadSector( abtSec, 2 ) )
	{
		sprintf( m_szLastError, "BAS2BOOT: Can't read boot sector because\n%s", m_pDisk->GetLastError() );
		delete pE;
		return NULL;
	}

	int iSectors = ( ( pE->m_dwFileLen - 0x0E + 0x7F ) / 0x80 );
	int iStoredSectors = abtSec[ 0x70 ] + ( abtSec[ 0x71 ] << 8 );

	if ( ( iSectors - 1 ) != iStoredSectors )
	{
		sprintf( m_szLastError, "BAS2BOOT: Not a Bas2Boot image or invalid! (%d<>%d)", iSectors - 1, iStoredSectors );
		delete pE;
		return NULL;
	}


	if ( ( iSectors + 2 ) > m_pDisk->GetSectorCount() )
	{
		sprintf( m_szLastError, "BAS2BOOT: Not a Bas2Boot image or invalid! (%d<>%d)", iSectors + 2, m_pDisk->GetSectorCount() );
		delete pE;
		return NULL;
	}

	sprintf( pE->m_szAscData, "%06lX", pE->m_dwFileLen );

	GuessBestFnameFromPC( pE->m_szFname, m_pDisk->GetImageName(), "bas" );

	return pE;
}

BOOL CBas2Boot::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	int hOutfile = -1;

	if ( szOutFile )
	{
		hOutfile = open( szOutFile, O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE );

		if ( -1 == hOutfile )
		{
			sprintf( m_szLastError, "BAS2BOOT: Unable to create file '%s'!", szOutFile );
			return FALSE;
		}
	}

	DWORD dwFileLen = ((CBas2BootDirEntry*)pDirE) ->m_dwFileLen;

	BYTE abtBuff[ 0x80 ];

	int iSector = 3;

	m_pDisk->ReadSector( abtBuff, 2 );
	if ( -1 != hOutfile )
		write( hOutfile, abtBuff + 0x72, 0x0E );

	dwFileLen -= 0xE;

	while( dwFileLen )
	{
		WORD wToCopy = ( dwFileLen < 0x80 ) ? dwFileLen : 0x80;

		if ( !m_pDisk->ReadSector( abtBuff, iSector ) )
		{
			sprintf( m_szLastError, "BAS2BOOT: Can't read sector because\n%s", m_pDisk->GetLastError() );
			return FALSE;
		}

		if ( -1 != hOutfile )
			write( hOutfile, abtBuff, wToCopy );

		dwFileLen -= wToCopy;
		iSector++;

	}
	if ( -1 != hOutfile )
		close( hOutfile );

	return TRUE;
}

