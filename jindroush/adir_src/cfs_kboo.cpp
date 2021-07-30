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

#include "cfs_kboo.h"
#include "autil.h"

CKBoot::CKBoot() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CKBoot constructed: %08X\n", this );
	#endif
}

CKBoot::~CKBoot()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CKBoot destructed: %08X\n", this );
	#endif
}

BOOL CKBoot::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;
	m_pDisk = pDisk;
	m_pRoot = NULL;

	if ( m_pDisk->GetSectorSize() != 0x80 )
	{
		sprintf( m_szLastError, "KBOOT: Can't process DD disk!" );
		return FALSE;
	}

	m_pRoot = CreateEntry();

	return m_pRoot ? TRUE : FALSE;
}

void CKBoot::Dismount()
{
	DeleteList( m_pRoot );
}

CKBootDirEntry* CKBoot::CreateEntry()
{
	CKBootDirEntry* pE = new CKBootDirEntry();

	if ( !pE )
		return NULL;

	BYTE abtSec[ 0x100 ];

	if ( !m_pDisk->ReadSector( abtSec, 1 ) )
	{
		sprintf( m_szLastError, "KBOOT: Can't read boot sector because\n%s", m_pDisk->GetLastError() );
		delete pE;
		return NULL;
	}

	pE->m_dwFileLen = abtSec[ 9 ] + ( abtSec[ 10 ] << 8 ) + ( abtSec[ 11 ] << 16 );

	if ( !m_pDisk->ReadSector( abtSec, 4 ) )
	{
		sprintf( m_szLastError, "KBOOT: Can't read first post boot sector because\n%s", m_pDisk->GetLastError() );
		delete pE;
		return NULL;
	}

	if ( ( abtSec[ 0 ] != 0xFF ) || ( abtSec[ 1 ] != 0xFF ) )
	{
		sprintf( m_szLastError, "KBOOT: Not a KBoot image!" );
		delete pE;
		return NULL;
	}

	int iSectors = 3 + ( ( pE->m_dwFileLen + 0x7F ) / 0x80 );

	if ( iSectors > m_pDisk->GetSectorCount() )
	{
		sprintf( m_szLastError, "KBOOT: Not a KBoot image or invalid! (%d<>%d)", iSectors, m_pDisk->GetSectorCount() );
		delete pE;
		return NULL;
	}

	sprintf( pE->m_szAscData, "%06lX", pE->m_dwFileLen );

	GuessBestFnameFromPC( pE->m_szFname, m_pDisk->GetImageName(), "axe" );

	return pE;
}

BOOL CKBoot::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	int hOutfile = -1;

	if ( szOutFile )
	{
		hOutfile = open( szOutFile, O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE );

		if ( -1 == hOutfile )
		{
			sprintf( m_szLastError, "KBOOT: Unable to create file '%s'!", szOutFile );
			return FALSE;
		}
	}

	DWORD dwFileLen = ((CKBootDirEntry*)pDirE) ->m_dwFileLen;

	BYTE abtBuff[ 0x80 ];

	int iSector = 4;

	while( dwFileLen )
	{
		WORD wToCopy = ( dwFileLen < 0x80 ) ? dwFileLen : 0x80;

		if ( !m_pDisk->ReadSector( abtBuff, iSector ) )
		{
			sprintf( m_szLastError, "KBOOT: Can't read sector because\n%s", m_pDisk->GetLastError() );
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

