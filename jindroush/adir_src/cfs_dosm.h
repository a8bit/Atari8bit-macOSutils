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

#ifndef __CDOSM_H__
#define __CDOSM_H__

#include "wintypes.h"
#include "cfs.h"

#pragma pack(1)
typedef struct
{
	BYTE btFlags;
	WORD wSecCount; //Number of sectors in file
	WORD wSecStart; //First sector in file
	CHAR acAtariName[11];
} DOSM_DIRENT;

#pragma pack()

//flags bits:
// 0x80 7->deleted  
// 0x40 6->in use  
// 0x20 5->locked  
// 0x10 4->mydos subdir
// 0x08 3->
// 0x04 2->
// 0x02 1->
// 0x01 0->write open

class CDosMDirEntry : public CDirEntry
{
public:
	BYTE m_btFlags;
	WORD m_wSecCount; //Number of sectors in file
	WORD m_wSecStart; //First sector in file
};

class CDosM : public CFs
{
public:
	CDosM();
	~CDosM();

	BOOL	Mount( ADisk* );
	void	Dismount();
	BOOL	ExportFile( char*, CDirEntry* );
	BOOL	ReadDir( int, CDosMDirEntry** );

private:
	CDosMDirEntry* CreateEntry( DOSM_DIRENT* );
};

#endif
