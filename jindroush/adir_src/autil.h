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

#ifndef __AUTIL_H__
#define __AUTIL_H__

#include "wintypes.h"

void ADos2MsDos( char*, char* );
void GuessBestFnameFromPC( char* szDest, char* szSrc, char* szExt, char* szAdd = NULL );
void GuessBestFnameFromAtari( char* szDest, char* szSrc, char* szExt );

BOOL IsBlockEmpty( BYTE*, int );

#define MGET_B( p ) ( *(p++ ) );
#define MGET_LEW( p ) ( *p + ((*(p+1) ) <<8) ),p+=2;
#define MGET_BEW( p ) ( *(p+1) + ((*(p) ) <<8) ),p+=2;

#define MREAD_B( p ) ( *p );
#define MREAD_LEW( p ) ( *p + ((*(p+1) ) <<8) );
#define MREAD_BEW( p ) ( *(p+1) + ((*(p) ) <<8) );

#endif
