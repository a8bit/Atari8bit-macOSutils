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

#ifndef __CGENFILE_H__
#define __CGENFILE_H__

class CGenFile
{
public:
	virtual ~CGenFile() {};

	virtual BOOL Read( void*, int, int* = NULL ) = 0;
	virtual BOOL Write( void*, int, int* = NULL ) = 0;

	virtual BOOL Seek( long, int ) = 0;

	virtual void Close() = 0;

	void writeb( BYTE );
	void writeLEw( WORD );
	void writeBEw( WORD );
	void writeLEdw( DWORD );

	BYTE readb();
	WORD readLEw();
	WORD readBEw();
	DWORD readLEdw();

	WORD  readw();
	DWORD readd();

	BOOL	skip( long );

	long Tell();

	LONG	m_lCurrPtr;
	
private:
};

#endif
