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

#ifndef __CDOS3_H__
#define __CDOS3_H__

#include "wintypes.h"
#include "cfs.h"

#pragma pack(1)
typedef struct
{
	BYTE btFlags;
	CHAR acAtariName[11];
	BYTE btSecCount;
	BYTE btSecStart;
	WORD wFileLen;
} DOS3_DIRENT;

#pragma pack()

class CDos3DirEntry : public CDirEntry
{
public:
	BYTE m_btSecStart;
	BYTE m_btSecCount;
	BYTE m_btFlags;
	WORD m_wFileLen;
};

class CDos3 : public CFs
{
public:
	CDos3();
	~CDos3();

	BOOL	Mount( ADisk* );
	void	Dismount();
	BOOL	ExportFile( char*, CDirEntry* );

private:
	CDos3DirEntry* CreateEntry( DOS3_DIRENT* );

	BYTE m_abtFat[ 0x0100 ];
};

#endif
