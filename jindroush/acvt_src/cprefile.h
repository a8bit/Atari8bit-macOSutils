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

#ifndef __CPREFILE_H__
#define __CPREFILE_H__

#include "wintypes.h"
#include "cgenfile.h"

class CPreFile : public CGenFile
{
public:
	CPreFile();
	~CPreFile();

	BOOL Open( CGenFile*, int );

	BOOL Read( void*, int, int* = NULL );
	BOOL Write( void*, int, int* = NULL );

	BOOL Seek( long, int );
	long GetLength() { return m_lLength; };
	DWORD GetLastError() { return m_dwLastError; };
	void Close();

private:
	BOOL	PreRead();

	BOOL	m_bOpened;

	DWORD m_dwLastError;
	
	long	m_lCacheStartOffset;
	long	m_lCacheEndOffset;

	long	m_lLength;
	BYTE*	m_pbtCache;
	long	m_lCacheLen;

	CGenFile* m_pcf;
};

#endif //__CPreFILE_H__
