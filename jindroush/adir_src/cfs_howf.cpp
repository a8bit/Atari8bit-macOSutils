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

#include "cfs_howf.h"
#include "autil.h"
#include "adsk_atr.h"

#define HOWF_MAX_NAME 32
#define HOWF_REASONABLE_NAME ( HOWF_MAX_NAME - 7 )

CHowf::CHowf() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CHowf constructed: %08X\n", this );
	#endif
}

CHowf::~CHowf()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CHowf destructed: %08X\n", this );
	#endif
}

BOOL CHowf::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;
	m_pDisk = pDisk;
	m_pRoot = NULL;

	if ( m_pDisk->GetSectorSize() != 0x80 )
	{
		sprintf( m_szLastError, "HOWF: Can't process DD disk!" );
		return FALSE;
	}

	switch( m_pDisk->GetBootSectorCount() )
	{
		case 0x0A:
			break;

		default:
		{
			sprintf( m_szLastError, "HOWF: Can't identify disk as a HowFen menu!" );
			return FALSE;
		}
	}

	CHowfDirEntry* pPrev = NULL;

	BYTE abtSec[ 0xA * 0x80 ];

	if ( !m_pDisk->ReadSectors( abtSec, 0x1, 0xA ) )
	{
			sprintf( m_szLastError, "HOWF: Can't read directory because\n%s", m_pDisk->GetLastError() );
			return FALSE;
	}

	BYTE* pbtName = abtSec + 0x89;
	BYTE* abtStartSecLo = abtSec + 0x32A;
	BYTE* abtStartSecHi = abtSec + 0x33E;

	BYTE btRecords = abtSec[ 0x329 ];

	for( int i = 0; i < btRecords; i++ )
	{
		WORD wStartSec = *abtStartSecLo + ( *abtStartSecHi << 8 );

		if ( !wStartSec )
			break;

		CHowfDirEntry* pE = CreateEntry( wStartSec, pbtName );

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
		pbtName += HOWF_MAX_NAME;
	}

	return TRUE;
}

void CHowf::Dismount()
{
	DeleteList( m_pRoot );
}

CHowfDirEntry* CHowf::CreateEntry( WORD wStartSec, BYTE* btName )
{
	CHowfDirEntry* pE = new CHowfDirEntry();

	if ( !pE )
		return NULL;

	pE->m_iStartSec = wStartSec;

	sprintf( pE->m_szAscData, "%04X", wStartSec );

	char szTemp[ HOWF_REASONABLE_NAME + 1 ];
	memcpy( szTemp, (char*)btName + 3, HOWF_REASONABLE_NAME );
	szTemp[ HOWF_REASONABLE_NAME ] = '\0';

	for( int i = 0; i < HOWF_REASONABLE_NAME; i++ )
	{
		char c = szTemp[ i ];
		if ( ( c >= 0x00 ) && ( c <= 0x40 ) )
			c += 0x20;

		szTemp[ i ] = c;
	}

	GuessBestFnameFromAtari( pE->m_szFname, szTemp, "atr" );

	if ( ExportFile( NULL, pE ) )
		m_iFilesValid++;
	else
		m_iFilesInvalid++;

	return pE;
}

BOOL CHowf::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	int iSecCount;
	int iStartSec = ((CHowfDirEntry*)pDirE) ->m_iStartSec;

	BYTE abtBuff[ 0x80 ];

	m_pDisk->ReadSector( abtBuff, iStartSec );

	iSecCount = abtBuff[1];

	if ( ( iStartSec + iSecCount - 1 ) > m_pDisk->GetSectorCount() )
	{
		sprintf( m_szLastError, "HOWF: File '%s' goes past last sector!", szOutFile );
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
		sprintf( m_szLastError, "HOWF: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
		return FALSE;
	}

	int iCurrSec = 1;

	while( iSecCount )
	{
		if( !m_pDisk->ReadSector( abtBuff, iStartSec++ ) )
		{
			sprintf( m_szLastError, "HOWF: File '%s' can't create because\n%s", szOutFile, m_pDisk->GetLastError() );
			return FALSE;
		}

		if ( !newdisk.WriteSector( iCurrSec++, abtBuff ) )
		{
			sprintf( m_szLastError, "HOWF: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
			return FALSE;
		}

		iSecCount--;
	}

	if ( !newdisk.Save( szOutFile, FALSE ) )
	{
		sprintf( m_szLastError, "HOWF: File '%s' can't create because\n%s", szOutFile, newdisk.GetLastError() );
		return FALSE;
	}

	return TRUE;
}

