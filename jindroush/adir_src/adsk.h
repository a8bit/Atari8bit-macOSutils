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

#ifndef __ADSK_H__
#define __ADSK_H__

#include "wintypes.h"
#include "cobj.h"
#include "cdsk.h"

#define MAX_ATARI_SECTOR_LEN 0x100

typedef enum
{
	DISK_AUTO,
	DISK_ATR,
	DISK_ATRb,
	DISK_XFD,
	DISK_XFDb,
	DISK_SCP,
	DISK_DCM,
	DISK_DI
} DISK_TYPE;

typedef enum
{
	DI_RET_OK,
	DI_RET_CANT_CONTINUE,
	DI_RET_CONTINUE
} DISKINIT_RETCODE;

class ADisk : public CDisk
{
public:
	ADisk();
	~ADisk();

	BOOL ReadSectors( void*, int, int );
	BOOL WriteSectors( int, void*, int );

	int GetBootSectorCount();
	int GetBootSectorSize();
	BOOL GetBootSector( BYTE* );
};

void GuessClassicSizes( int, int, DISK_GEOMETRY* );
void ForceClassicSize( int, int, DISK_GEOMETRY* );
void ForceClassicSize( DISK_GEOMETRY* );

char* GetDiskTypeName( DISK_TYPE disktype );
char* GetDiskTypeExt( DISK_TYPE disktype );

DISKINIT_RETCODE InitializeDisk( ADisk**, DISK_TYPE, char*, BOOL bVerbose, BOOL, BOOL );


#include "adsk_atr.h"
#include "adsk_xfd.h"
#include "adsk_scp.h"
#include "adsk_dcm.h"
#include "adsk_di.h"

#endif //_ADSK_H__

