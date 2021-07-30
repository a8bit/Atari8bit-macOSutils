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

#include "cfs_boot.h"
#include "autil.h"
#include "adsk_xfd.h"

CBoot::CBoot() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CBoot constructed: %08X\n", this );
	#endif
}

CBoot::~CBoot()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CBoot destructed: %08X\n", this );
	#endif
}

BOOL CBoot::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;
	m_pDisk = pDisk;
	m_pRoot = NULL;

	m_pRoot = CreateEntry();

	return m_pRoot ? TRUE : FALSE;
}

void CBoot::Dismount()
{
	DeleteList( m_pRoot );
}

CBootDirEntry* CBoot::CreateEntry()
{
	CBootDirEntry* pE = new CBootDirEntry();

	if ( !pE )
		return NULL;

	pE->m_iSectorCount = m_pDisk->GetBootSectorCount();

	if ( pE->m_iSectorCount > m_pDisk->GetSectorCount() )
	{
		sprintf( m_szLastError, "BOOT: Overlaps end of file!" );
		delete pE;
		return NULL;
	}

	sprintf( pE->m_szAscData, "%08X", pE->m_iSectorCount );

	GuessBestFnameFromPC( pE->m_szFname, m_pDisk->GetImageName(), "xfd", "_boot" );

	return pE;
}

BOOL CBoot::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	if ( !szOutFile )
		return TRUE;

	CXfd newdisk;

	DISK_GEOMETRY dg;

	int iSecCount = ((CBootDirEntry*)pDirE)->m_iSectorCount;

	dg.iSides = 1;
	dg.iTracks = 1;
	dg.iSectorsPerTrack = iSecCount;
	dg.iBytesPerSector = 0x80;

	if ( !newdisk.Format( &dg ) )
	{
		sprintf( m_szLastError, "BOOT: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
		return FALSE;
	}

	int iStartSec = 1;
	int iCurrSec = 1;

	BYTE abtBuff[ MAX_ATARI_SECTOR_LEN ];

	while( iSecCount )
	{
		if( !m_pDisk->ReadSector( abtBuff, iStartSec++ ) )
		{
			sprintf( m_szLastError, "BOOT: File '%s' can't create because\n%s", szOutFile, m_pDisk->GetLastError() );
			return FALSE;
		}

		if ( !newdisk.WriteSector( iCurrSec++, abtBuff ) )
		{
			sprintf( m_szLastError, "BOOT: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
			return FALSE;
		}

		iSecCount--;
	}

	if ( !newdisk.Save( szOutFile, FALSE ) )
	{
		sprintf( m_szLastError, "BOOT: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
		return FALSE;
	}


	return TRUE;
}

