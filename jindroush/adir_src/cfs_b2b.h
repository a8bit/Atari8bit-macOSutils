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

#ifndef __CBAS2BOOT_H__
#define __CBAS2BOOT_H__

#include "wintypes.h"
#include "cfs.h"

class CBas2BootDirEntry : public CDirEntry
{
public:
	DWORD m_dwFileLen;
};

class CBas2Boot : public CFs
{
public:
	CBas2Boot();
	~CBas2Boot();

	BOOL	Mount( ADisk* );
	void	Dismount();
	BOOL	ExportFile( char*, CDirEntry* );

private:
	CBas2BootDirEntry* CreateEntry();
};

#endif
