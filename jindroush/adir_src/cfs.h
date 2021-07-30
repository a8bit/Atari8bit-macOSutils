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

#ifndef __CFS_H__
#define __CFS_H__

#include "adsk.h"
#include "cobj.h"

#define DIRE_DELETED		0x00000001
#define DIRE_SUBDIR		0x00000002

class CDirEntry
{
public:
	CDirEntry();
	~CDirEntry();

	char m_szFname[ 256 ];
	char m_szAscData[ 256 ];
	DWORD m_dwFlags;
	CDirEntry* m_pSubdir;
	CDirEntry* m_pNext;
	CDirEntry* m_pPrev;
};

class CFs : public CObj
{
public:

	CFs();
	virtual ~CFs();

	virtual BOOL	Mount( ADisk* ) = 0;
	virtual void	Dismount() = 0;
	virtual BOOL	ExportFile( char*, CDirEntry* ) = 0;
	CDirEntry* GetRoot() { return m_pRoot; };

	int GetInvalidPercent() { return (m_iFilesInvalid+m_iFilesValid) ? ( m_iFilesInvalid * 100 ) / ( m_iFilesValid + m_iFilesInvalid ) : 0; };

	void DeleteList( CDirEntry* );

protected:
	int m_iFilesValid;
	int m_iFilesInvalid;


	CDirEntry*	m_pRoot;
	ADisk*		m_pDisk;
};

#endif

