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

#include "cdisk.h"
#include "cdsk_atr.h"
#include "cdsk_xfd.h"
#include "cdsk_dcm.h"
#include "cdsk_scp.h"


CDisk::CDisk()
{ 
	m_bOpened = FALSE; 
	m_pbtMemory = NULL;

	#ifdef _MEMORY_DUMP_
		printf( "CDisk constructed: %08X\n", this );
	#endif
}

CDisk::~CDisk()
{
	if ( m_pbtMemory )
	{
		delete [] m_pbtMemory;
		m_pbtMemory = NULL;
	}

	#ifdef _MEMORY_DUMP_
		printf( "CDisk destructed: %08X\n", this );
	#endif
}

BOOL CDisk::Duplicate( CDisk* pDisk )
{
	if ( !Format( & ( pDisk->m_geometry ) ) )
		return FALSE;

	memcpy( m_pbtMemory, pDisk->m_pbtMemory, pDisk->m_iAllocated );
	return TRUE;
}

BOOL CDisk::Format( DISK_GEOMETRY* pgeo )
{
	int iSectors = pgeo->iSides * pgeo->iTracks * pgeo->iSectorsPerTrack;

	if ( m_pbtMemory )
		delete [] m_pbtMemory;

	m_iAllocated = iSectors * pgeo->iBytesPerSector;
	m_pbtMemory = new BYTE [ m_iAllocated ];

	if ( !m_pbtMemory )
	{
		sprintf( m_szLastError, "DISK: Can't format - Not enough memory!" );
		return FALSE;
	}

	memset( m_pbtMemory, 0, m_iAllocated );

	m_geometry.iSides = pgeo->iSides;
	m_geometry.iTracks = pgeo->iTracks;
	m_geometry.iSectorsPerTrack = pgeo->iSectorsPerTrack;
	m_geometry.iBytesPerSector = pgeo->iBytesPerSector;
	m_geometry.iSectors = iSectors;

	return TRUE;
}

BOOL CDisk::ReadSector( void* pBuf, int iStartSec )
{
	if ( iStartSec && ( iStartSec <= m_geometry.iSectors ) )
		memcpy( pBuf, m_pbtMemory + ( iStartSec - 1 ) * m_geometry.iBytesPerSector, m_geometry.iBytesPerSector );
	else
	{
		sprintf( m_szLastError, "DISK: Reading non-existent sector: %04X", iStartSec );
		return FALSE;
	}

	return TRUE;
}

BOOL CDisk::ReadSectors( void* pBuf, int iStartSec, int iSecs )
{
	while( iSecs )
	{
		if ( !ReadSector( pBuf, iStartSec ) )
			return FALSE;

		iStartSec++;
		iSecs--;
		pBuf = (BYTE*)pBuf + m_geometry.iBytesPerSector;
	}

	return TRUE;
}

BOOL CDisk::WriteSector( int iStartSec, void* pBuf )
{
	//printf( " ws: %d ", iStartSec );
	if ( iStartSec && ( iStartSec <= m_geometry.iSectors ) )
		memcpy( m_pbtMemory + ( iStartSec - 1 ) * m_geometry.iBytesPerSector, pBuf, m_geometry.iBytesPerSector );
	else
	{
		sprintf( m_szLastError, "DISK: Writing non-existent sector: %04X", iStartSec );
		return FALSE;
	}

	return TRUE;
}

void GuessClassicSizes( int iSectors, int iSectorSize, DISK_GEOMETRY* pGeometry )
{
	pGeometry->iSides = 1;
	pGeometry->iBytesPerSector = iSectorSize;

	pGeometry->iTracks = 1;
	pGeometry->iSectorsPerTrack = iSectors;

	switch( iSectors )
	{
		case 720:
			switch( iSectorSize )
			{
				case 0x80:
					pGeometry->iTracks = 40;
					pGeometry->iSectorsPerTrack = 18;
					break;

				case 0x100:
					pGeometry->iTracks = 40;
					pGeometry->iSectorsPerTrack = 18;
					break;
			}
			break;

		case 1040:
			pGeometry->iTracks = 40;
			pGeometry->iSectorsPerTrack = 26;
			break;

	}

}

//returns ptr to disk type name
char* GetDiskTypeName( DISK_TYPE disktype )
{
	switch( disktype )
	{
		case DISK_XFD:
			return "XFD";

		case DISK_ATR:
			return "ATR";

		case DISK_XFDb:
			return "XFDb";

		case DISK_ATRb:
			return "ATRb";

		case DISK_DCM:
			return "DCM";

		case DISK_SCP:
			return "SCP";

		default:
			return "None";
	}
}

//returns ptr to disk type extension
char* GetDiskTypeExt( DISK_TYPE disktype )
{
	switch( disktype )
	{
		case DISK_XFD:
		case DISK_XFDb:
			return "xfd";

		case DISK_ATR:
		case DISK_ATRb:
			return "atr";

		case DISK_DCM:
			return "dcm";

		case DISK_SCP:
			return "scp";

		default:
			return "xxx";
	}
}

DISKINIT_RETCODE InitializeDisk( CDisk** ppDisk, DISK_TYPE disktype, char* szFname, BOOL bVerbose, BOOL bRepair, BOOL bRepairAuto )
{
	switch( disktype )
	{
		case DISK_ATR:
			*ppDisk = new CAtr();
			break;

		case DISK_SCP:
			*ppDisk = new CScp();
			break;

		case DISK_DCM:
			*ppDisk = new CDcm();
			break;

		case DISK_XFD:
			*ppDisk = new CXfd();
			break;

		default:
			if ( bVerbose )
				fprintf( stderr, "Invalid disk type specified!\n" );
			return DI_RET_CANT_CONTINUE;
	}

	if ( !*ppDisk )
	{
		if ( bVerbose )
		 	fprintf( stderr, "Can't initialize disk driver!\n" );
	 	return DI_RET_CONTINUE;
	}

	if ( !(*ppDisk)->Load( szFname, bRepair, bRepairAuto ) )
	{
		int iError = (*ppDisk)->GetErrorCode();

		DISKINIT_RETCODE ret = DI_RET_CONTINUE;

		switch( iError )
		{
			case CATR_FORMAT_VIOLATED:
			case CXFD_FORMAT_VIOLATED:
			case CDISK_ERROR_CANT_OPEN:
			case CDCM_FORMAT_VIOLATED:
			case CSCP_FORMAT_VIOLATED:
				ret = DI_RET_CANT_CONTINUE;
				break;
		}

		if ( bVerbose || ( ret == DI_RET_CANT_CONTINUE ) )
		{
			printf( "Input file '%s' ", szFname );
			printf( "(%s)\n", GetDiskTypeName( disktype ) );
			printf( "Load failed because:\n%s\n", (*ppDisk)->GetLastError() );
		}
		if ( ret != DI_RET_OK )
		{
			delete *ppDisk;
			return ret;
		}
	}

	return DI_RET_OK;
}


