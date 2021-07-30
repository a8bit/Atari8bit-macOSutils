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

#include "cfs_robc.h"
#include "autil.h"
#include "adsk_atr.h"

#define ROBC_MAX_NAME 20

CRobc::CRobc() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CRobc constructed: %08X\n", this );
	#endif
}

CRobc::~CRobc()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CRobc destructed: %08X\n", this );
	#endif
}

BOOL CRobc::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;
	m_pDisk = pDisk;
	m_pRoot = NULL;

	if ( m_pDisk->GetSectorSize() != 0x80 )
	{
		sprintf( m_szLastError, "ROBC: Can't process DD disk!" );
		return FALSE;
	}

	switch( m_pDisk->GetBootSectorCount() )
	{
		case 0x04:
			break;

		default:
		{
			sprintf( m_szLastError, "ROBC: Can't identify disk as a RobC menu!" );
			return FALSE;
		}
	}

	CRobcDirEntry* pPrev = NULL;

	BYTE abtSec[ 0x180 ];

	if ( !m_pDisk->ReadSectors( abtSec, 0x02, 3 ) )
	{
			sprintf( m_szLastError, "ROBC: Can't read directory because\n%s", m_pDisk->GetLastError() );
			return FALSE;
	}

	BYTE* abtStartSecLo = abtSec + 0x77;
	BYTE* abtStartSecHi = abtSec + 0x6E;
	BYTE* pbtName = abtSec + 0xB9;

	for( int i = 0; i < 10; i++ )
	{
		WORD wSec = *abtStartSecLo + ( *abtStartSecHi << 8 );

		//printf( "%d) %04X\n", i, wSec );

		if ( ! wSec )
			continue;

		BYTE abtLocSec[ 0x80 ];
		BYTE btSecCount;

		if( !m_pDisk->ReadSector( abtLocSec, wSec ) )
		{
			sprintf( m_szLastError, "ROBC: Can't read dir entry %d!", i );
			return FALSE;
		}

		btSecCount = abtLocSec[ 1 ];

		if ( ! btSecCount )
			break;

		CRobcDirEntry* pE = CreateEntry( btSecCount, wSec, pbtName + 2 );

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

		abtStartSecLo++;
		abtStartSecHi++;
		pbtName += ROBC_MAX_NAME;
	}

	return TRUE;
}

void CRobc::Dismount()
{
	DeleteList( m_pRoot );
}

CRobcDirEntry* CRobc::CreateEntry( BYTE btSecCount, WORD wStartSec, BYTE* btName )
{
	CRobcDirEntry* pE = new CRobcDirEntry();

	if ( !pE )
		return NULL;

	pE->m_iStartSec = wStartSec;
	pE->m_iSecCount = btSecCount;

	sprintf( pE->m_szAscData, "%04X %02X", wStartSec, btSecCount );

	char szTemp[ ROBC_MAX_NAME + 1 - 2 ];
	memcpy( szTemp, btName, ROBC_MAX_NAME - 2 );
	szTemp[ ROBC_MAX_NAME - 2 ] = '\0';

	//printf( "%s\n", szTemp );

	char* szT = szTemp;

	while( *szT )
	{
		*szT &= 0x7F;
	  	*szT -= 0x20;
		szT++;
	}

	GuessBestFnameFromAtari( pE->m_szFname, szTemp, "atr" );

	//printf( "%s\n", pE->m_szFname );

	if ( ExportFile( NULL, pE ) )
		m_iFilesValid++;
	else
		m_iFilesInvalid++;

	return pE;
}

BOOL CRobc::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	int iSecCount = ((CRobcDirEntry*)pDirE) ->m_iSecCount;
	int iStartSec = ((CRobcDirEntry*)pDirE) ->m_iStartSec;

	BYTE abtBuff[ 0x80 ];

	if ( ( iStartSec + iSecCount - 1 ) > m_pDisk->GetSectorCount() )
	{
		sprintf( m_szLastError, "ROBC: File '%s' goes past last sector!", szOutFile );
		return FALSE;
	}

	m_pDisk->ReadSector( abtBuff, iStartSec );

	if ( abtBuff[ 1 ] != iSecCount )
	{
		sprintf( m_szLastError, "ROBC: File '%s' Mismatched length (%d<>%d)!", szOutFile, abtBuff[ 1 ], iSecCount );
		return FALSE;
	}

	if ( !szOutFile )
		return TRUE;

	CAtr newdisk;

	DISK_GEOMETRY dg;

	dg.iSides = 1;
	dg.iTracks = 1;
	dg.iSectorsPerTrack = iSecCount;
	dg.iBytesPerSector = 0x80;

	if ( !newdisk.Format( &dg ) )
	{
		sprintf( m_szLastError, "ROBC: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
		return FALSE;
	}

	int iCurrSec = 1;

	while( iSecCount )
	{
		if( !m_pDisk->ReadSector( abtBuff, iStartSec++ ) )
		{
			sprintf( m_szLastError, "ROBC: File '%s' can't create because\n%s", szOutFile, m_pDisk->GetLastError() );
			return FALSE;
		}

		if ( !newdisk.WriteSector( iCurrSec++, abtBuff ) )
		{
			sprintf( m_szLastError, "ROBC: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
			return FALSE;
		}

		iSecCount--;
	}

	if ( !newdisk.Save( szOutFile, FALSE ) )
	{
		sprintf( m_szLastError, "ROBC: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
		return FALSE;
	}

	return TRUE;
}

