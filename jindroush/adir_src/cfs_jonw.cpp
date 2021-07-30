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

#include "cfs_jonw.h"
#include "autil.h"
#include "adsk_atr.h"

#define JONW_MAX_NAME 20

CJonw::CJonw() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CJonw constructed: %08X\n", this );
	#endif
}

CJonw::~CJonw()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CJonw destructed: %08X\n", this );
	#endif
}

BOOL CJonw::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;
	m_pDisk = pDisk;
	m_pRoot = NULL;

	if ( m_pDisk->GetSectorSize() != 0x80 )
	{
		sprintf( m_szLastError, "JONW: Can't process DD disk!" );
		return FALSE;
	}

	switch( m_pDisk->GetBootSectorCount() )
	{
		case 0x28:
		case 0x29:
		case 0x2F:
			break;

		default:
		{
			sprintf( m_szLastError, "JONW: Can't identify disk as a JonW menu!" );
			return FALSE;
		}
	}

	CJonwDirEntry* pPrev = NULL;

	BYTE abtSec[ 0x100 ];

	if ( !m_pDisk->ReadSectors( abtSec, 0x30, 2 ) )
	{
			sprintf( m_szLastError, "JONW: Can't read directory because\n%s", m_pDisk->GetLastError() );
			return FALSE;
	}

	BYTE* abtSecCounts = abtSec;
	BYTE* abtStartSecLo = abtSec + 10;
	BYTE* abtStartSecHi = abtSec + 20;
	BYTE* pbtName = abtSec + 30;

	for( int i = 0; i < 10; i++ )
	{
		if ( ! *abtSecCounts )
			break;

		CJonwDirEntry* pE = CreateEntry( *abtSecCounts, *abtStartSecLo + ( *abtStartSecHi << 8 ), pbtName );

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

		abtSecCounts++;
		abtStartSecLo++;
		abtStartSecHi++;
		pbtName += JONW_MAX_NAME;
	}

	return TRUE;
}

void CJonw::Dismount()
{
	DeleteList( m_pRoot );
}

CJonwDirEntry* CJonw::CreateEntry( BYTE btSecCount, WORD wStartSec, BYTE* btName )
{
	CJonwDirEntry* pE = new CJonwDirEntry();

	if ( !pE )
		return NULL;

	pE->m_iStartSec = wStartSec;
	pE->m_iSecCount = btSecCount;

	sprintf( pE->m_szAscData, "%04X %02X", wStartSec, btSecCount );

	char szTemp[ 21 ];
	strncpy( szTemp, (char*)btName, 20 );
	szTemp[ 20 ] = '\0';

	GuessBestFnameFromAtari( pE->m_szFname, szTemp, "atr" );

	if ( ExportFile( NULL, pE ) )
		m_iFilesValid++;
	else
		m_iFilesInvalid++;

	return pE;
}

BOOL CJonw::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	int iSecCount = ((CJonwDirEntry*)pDirE) ->m_iSecCount;
	int iStartSec = ((CJonwDirEntry*)pDirE) ->m_iStartSec;

	BYTE abtBuff[ 0x80 ];

	if ( ( iStartSec + iSecCount - 1 ) > m_pDisk->GetSectorCount() )
	{
		sprintf( m_szLastError, "JONW: File '%s' goes past last sector!", szOutFile );
		return FALSE;
	}

	m_pDisk->ReadSector( abtBuff, iStartSec );

	if ( abtBuff[ 1 ] != iSecCount )
	{
		sprintf( m_szLastError, "JONW: File '%s' Mismatched length (%d<>%d)!", szOutFile, abtBuff[ 1 ], iSecCount );
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
		sprintf( m_szLastError, "JONW: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
		return FALSE;
	}

	int iCurrSec = 1;

	while( iSecCount )
	{
		if( !m_pDisk->ReadSector( abtBuff, iStartSec++ ) )
		{
			sprintf( m_szLastError, "JONW: File '%s' can't create because\n%s", szOutFile, m_pDisk->GetLastError() );
			return FALSE;
		}

		if ( !newdisk.WriteSector( iCurrSec++, abtBuff ) )
		{
			sprintf( m_szLastError, "JONW: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
			return FALSE;
		}

		iSecCount--;
	}

	if ( !newdisk.Save( szOutFile, FALSE ) )
	{
		sprintf( m_szLastError, "JONW: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
		return FALSE;
	}

	return TRUE;
}

