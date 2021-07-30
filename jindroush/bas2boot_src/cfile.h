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

#include "wintypes.h"
#include "cgenfile.h"

#ifndef __CFILE_H__
#define __CFILE_H__


class CFile : public CGenFile
{
public:
	CFile();
	~CFile();

	BOOL Open( char* );
	BOOL Create( char* );

	BOOL Read( void*, int, int* = NULL );
	BOOL Write( void*, int, int* = NULL );

	BOOL Seek( long, int );
	long GetLength() { return m_lLength; };
	DWORD GetLastError() { return m_dwLastError; };
	void Close();

	BOOL IsDirty() { return m_bIsDirty; };

private:
	BOOL	m_bOpened;
	BOOL	m_bIsDirty;

	int	m_hFile;

	DWORD m_dwLastError;

	LONG	m_lLength;

};

#endif //__CFILE_H__
