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

#ifndef __CDISK_H__
#define __CDISK_H__

#include "wintypes.h"
#include "cobj.h"

#define CDISK_ERROR_CANT_OPEN ( CDISK_BASE + 0 )

typedef enum
{
	DISK_AUTO,
	DISK_ATR,
	DISK_ATRb,
	DISK_XFD,
	DISK_XFDb,
	DISK_SCP,
	DISK_DCM
} DISK_TYPE;

typedef enum
{
	DI_RET_OK,
	DI_RET_CANT_CONTINUE,
	DI_RET_CONTINUE
} DISKINIT_RETCODE;

typedef struct
{
	int iSides;
	int iTracks;
	int iSectorsPerTrack;
	int iBytesPerSector;
	int iSectors;
} DISK_GEOMETRY;

class CDisk : public CObj
{
public:
	CDisk();
	virtual ~CDisk();

	virtual BOOL Load( char*, BOOL = FALSE, BOOL = FALSE ) = 0;

	#ifdef __CDISK_SAVE__
	virtual BOOL Save( char*, BOOL ) = 0;
	#endif

	BOOL Format( DISK_GEOMETRY* );
	
	DISK_GEOMETRY* GetGeometry() { return &m_geometry; };
	int GetSectorSize() { return m_geometry.iBytesPerSector; };
	int GetSectorCount() { return m_geometry.iSectors; };

	BOOL ReadSector( void*, int );
	BOOL WriteSector( int, void* );

	BOOL ReadSectors( void*, int, int );
	BOOL WriteSectors( int, void*, int );

	BOOL Duplicate( CDisk* );

	char* GetImageName() { return m_szFname; };

protected:
	DISK_GEOMETRY m_geometry;
	BOOL m_bOpened;
	BYTE *m_pbtMemory;
	int m_iAllocated;

	char m_szFname[ 255 ];
};

void GuessClassicSizes( int, int, DISK_GEOMETRY* );

char* GetDiskTypeName( DISK_TYPE disktype );
char* GetDiskTypeExt( DISK_TYPE disktype );

DISKINIT_RETCODE InitializeDisk( CDisk**, DISK_TYPE, char*, BOOL bVerbose, BOOL, BOOL );

#endif
