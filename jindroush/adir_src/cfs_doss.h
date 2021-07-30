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

#ifndef __CDOSS_H__
#define __CDOSS_H__

#include "wintypes.h"
#include "cfs.h"

#pragma pack(1)
typedef struct
{
	BYTE btFlags;
	WORD wSector;
	WORD wSizeLo;
	BYTE btSizeHi;
	CHAR acAtariName[11];
	BYTE btDay;
	BYTE btMonth;
	BYTE btYear;
	BYTE btHour;
	BYTE btMinute;
	BYTE btSecond;
} DOSS_DIRENT;

#pragma pack()

class CDosSDirEntry : public CDirEntry
{
public:
	BYTE m_btFlags;
	int m_iLength;
	int m_iLinkSector;
};

class CDosS : public CFs
{
public:
	CDosS();
	~CDosS();

	BOOL	Mount( ADisk* );
	void	Dismount();
	BOOL	ExportFile( char*, CDirEntry* );
	BOOL	ReadDir( int, CDosSDirEntry** );

private:
	CDosSDirEntry* CreateEntry( DOSS_DIRENT* );
	int GetSectorLinkEntry( int, int );

	BYTE* MapFile( int, int );
	void	UnMapFile( BYTE* );

	WORD	m_wMainDirStart;
	WORD	m_wSectorSize;
};

#endif
