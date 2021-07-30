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

#ifndef __COBJ_H__
#define __COBJ_H__

#include "wintypes.h"

class CObj
{
public:
	CObj();
	const char* GetLastError( char* p = NULL ) { if ( p ) strcpy( p, m_szLastError ); return m_szLastError; };
	void SetLastError( char* p ) { strcpy( m_szLastError, p ); };
	int	GetErrorCode() { return m_iErrorCode; };
	
protected:
	char m_szLastError[ 4096 ];
	int m_iErrorCode;
};

#endif
