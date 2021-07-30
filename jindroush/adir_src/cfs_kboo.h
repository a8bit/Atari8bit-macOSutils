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

#ifndef __CKBOOT_H__
#define __CKBOOT_H__

#include "wintypes.h"
#include "cfs.h"

class CKBootDirEntry : public CDirEntry
{
public:
	DWORD m_dwFileLen;
};

class CKBoot : public CFs
{
public:
	CKBoot();
	~CKBoot();

	BOOL	Mount( ADisk* );
	void	Dismount();
	BOOL	ExportFile( char*, CDirEntry* );

private:
	CKBootDirEntry* CreateEntry();
};

#endif
