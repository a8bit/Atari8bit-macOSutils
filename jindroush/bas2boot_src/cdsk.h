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

	#ifndef __CDISK_NOLOAD__
	virtual BOOL Load( char*, BOOL = FALSE, BOOL = FALSE ) = 0;
	#endif

	#ifdef __CDISK_SAVE__
	virtual BOOL Save( char*, BOOL ) = 0;
	#endif

	BOOL Format( DISK_GEOMETRY* );
	
	DISK_GEOMETRY* GetGeometry() { return &m_geometry; };
	int GetSectorSize() { return m_geometry.iBytesPerSector; };
	int GetSectorCount() { return m_geometry.iSectors; };

	BOOL ReadSector( void*, int );
	BOOL WriteSector( int, void* );

	BOOL Duplicate( CDisk*, DISK_GEOMETRY* = NULL );

	char* GetImageName() { return m_szFname; };

protected:
	DISK_GEOMETRY m_geometry;
	BOOL m_bHasData;
	BYTE *m_pbtMemory;
	int m_iAllocated;

	char m_szFname[ 255 ];

	int	m_iMaxSectorWritten;

};

#endif
