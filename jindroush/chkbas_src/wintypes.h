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

#ifndef __WINTYPES_H__
#define __WINTYPES_H__

#include <errno.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#ifdef __DJGPP__
	#include <conio.h>
	#include <direct.h> 
	#include <io.h>
	#include <dos.h>
#else
	#define tell(x) lseek(x,0,SEEK_CUR)
	#define _fixpath(x,y) strcpy(y,x)
	#define getch() getchar()

#endif

#ifndef _WINDOWS
typedef int BOOL;
#define TRUE ( 1==1 )
#define FALSE ( !TRUE )

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

typedef unsigned char UBYTE;
typedef unsigned short UWORD;
typedef unsigned long UDWORD;

typedef char CHAR;
typedef long LONG;
typedef short SHORT;

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

typedef char SBYTE;
typedef short SWORD;
typedef long SDWORD;

typedef WORD WCHAR;
typedef CHAR TCHAR;

typedef WORD wchar_t;

typedef const char* LPCSTR;
typedef const WCHAR* LPCWSTR;
typedef const TCHAR* LPCTSTR;

typedef TCHAR* LPTSTR;
typedef CHAR LPSTR;

#define PASCAL pascal
#else

#include <windows.h>
#endif

#endif //