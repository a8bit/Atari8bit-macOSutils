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

#ifndef __SWITCHES_H__
#define __SWITCHES_H__

#include "wintypes.h"

typedef struct
{
	char* szName;
	BOOL (*pfbSwitchFunction)( void* );
	void* pPar1;
	char* szPars;
	char* szHelp;
} SWITCH_REC;

BOOL SWITCHES_Init( SWITCH_REC*, int* pargc, char* argv[] );
BOOL SWFN_HELP( void* );
BOOL SWFN_BOOLT( void* );
BOOL SWFN_BOOLF( void* );
BOOL SWFN_NUMH( void* );
BOOL SWFN_PATH( void* );

#endif

