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

#include "adsk.h"

ADisk::ADisk() : CDisk()
{
	#ifdef _MEMORY_DUMP_
		printf( "ADisk constructed: %p\n", this );
	#endif
}

ADisk::~ADisk()
{
	#ifdef _MEMORY_DUMP_
		printf( "ADisk destructed: %p\n", this );
	#endif
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

void ForceClassicSize( DISK_GEOMETRY* pGeometry )
{
	ForceClassicSize( pGeometry->iSectors, pGeometry->iBytesPerSector, pGeometry );
}

void ForceClassicSize( int iSectors, int iSectorSize, DISK_GEOMETRY* pGeometry )
{
	pGeometry->iSides = 1;
	pGeometry->iBytesPerSector = iSectorSize;

	pGeometry->iTracks = 1;
	pGeometry->iSectorsPerTrack = iSectors;

	switch( iSectorSize )
	{
		case 0x80:
			if ( iSectors <= 720 )
			{
				pGeometry->iTracks = 40;
				pGeometry->iSectorsPerTrack = 18;
			}
			else if ( ( iSectors > 720 ) && ( iSectors <= 1040 ) )
			{
				pGeometry->iTracks = 40;
				pGeometry->iSectorsPerTrack = 26;
			}
			else
			{
				GuessClassicSizes( iSectors, iSectorSize, pGeometry );
			}
			break;

		case 0x100:
		default:
			if ( iSectors <= 720 )
			{
				pGeometry->iTracks = 40;
				pGeometry->iSectorsPerTrack = 18;
			}
			else
			{
				GuessClassicSizes( iSectors, iSectorSize, pGeometry );
			}
			break;
	}

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

		case DISK_DI:
			return "DI";

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

		case DISK_DI:
			return "di";

		default:
			return "xxx";
	}
}

#ifndef __CDISK_NOLOAD__
DISKINIT_RETCODE InitializeDisk( ADisk** ppDisk, DISK_TYPE disktype, char* szFname, BOOL bVerbose, BOOL bRepair, BOOL bRepairAuto )
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

		case DISK_DI:
			*ppDisk = new CDi();
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
			case CDI_FORMAT_VIOLATED:
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
#endif

BOOL ADisk::ReadSectors( void* pBuf, int iStartSec, int iSecs )
{
	while( iSecs )
	{
		if ( !ReadSector( pBuf, iStartSec ) )
			return FALSE;

		pBuf = (BYTE*)pBuf + ( ( iStartSec <= 3 ) ? 0x80 :m_geometry.iBytesPerSector );
		iStartSec++;
		iSecs--;
	}

	return TRUE;
}

BOOL ADisk::WriteSectors( int iStartSec, void* pBuf, int iSecs )
{
	while( iSecs )
	{
		if ( !WriteSector( iStartSec, pBuf ) )
			return FALSE;

		pBuf = (BYTE*)pBuf + ( ( iStartSec <= 3 ) ? 0x80 :m_geometry.iBytesPerSector );
		iStartSec++;
		iSecs--;
	}

	return TRUE;
}

int ADisk::GetBootSectorCount()
{
	return m_pbtMemory[ 1 ];
}

int ADisk::GetBootSectorSize()
{
	int iSecCount = GetBootSectorCount();

	if ( iSecCount > 3 )
		return ( ( iSecCount - 3 ) * GetSectorSize() + 0x180 );
	else
		return iSecCount * 0x80;
}

BOOL ADisk::GetBootSector( BYTE* pbtData )
{
	return ReadSectors( pbtData, 1, GetBootSectorCount() );
}

