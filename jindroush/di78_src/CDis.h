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

#ifndef __AT_DIS_H__
#define __AT_DIS_H__

#include "wintypes.h"

#define SYMBOLS_NAME_XA "system.inc"
#define SYMBOLS_NAME_XASM "symbols.asx"
#define SYMBOLS_NAME_DASM "symbols.inc"

typedef enum
{
	ADDR_IMPLIED,
	ADDR_INDIRECT,
	ADDR_BRANCH,
	ADDR_DBDATA,
	ADDR_IMMEDIATE,
	ADDR_ZPAGE,
	ADDR_ZPAGEX,
	ADDR_ZPAGEY,
	ADDR_ABSOLUTE,
	ADDR_ABSOLUTEX,
	ADDR_ABSOLUTEY,
	ADDR_INDIRECTX,
	ADDR_INDIRECTY,
	ADDR_ACCUMULATOR
} ADDR_TYPES;

typedef enum
{
	ACCESS_WRIT,
	ACCESS_READ
} MEMORY_ACCESS;

typedef enum
{
	DISS_PLAIN,
	DISS_XA,
	DISS_XASM,
	DISS_DASM
} DISS_TYPE;

typedef enum
{
	UNDOC_NONE,
	UNDOC_FULL
} UNDOC_TYPE;

typedef enum
{
	PROC_6502_PLAIN
} PROC_TYPE;

#define A8_ZPAGE_LO 		0x0001
#define A8_ZPAGE_HI		0x0002
#define A8_OSVARS			0x0004
#define A8_DOSVARS		0x0008
#define A8_CHIPS			0x0010
#define A8_MATHROM		0x0020
#define A8_OSROM			0x0040
#define A5_CHIPS			0x0080
#define A5_ZPAGE			0x0100
#define A5_OSVARS			0x0200
#define A7_CHIPS			0x0400

#define A8_DEFAULT ( A8_ZPAGE_LO | A8_OSVARS | A8_CHIPS | A8_OSROM )
#define A5_DEFAULT ( A5_CHIPS | A5_ZPAGE | A5_OSVARS )
#define A7_DEFAULT ( A7_CHIPS )

class CDis
{
public:
	void Init( DISS_TYPE dtype, PROC_TYPE ptype, UNDOC_TYPE utype, DWORD dwAllowMask );
	void OutputBlockDiss( BYTE* pbtBlock, WORD wOffset, WORD wEnd );
	void OutputBlockDBs( BYTE*, WORD, WORD, WORD );

	void DumpSymbols();

	void PrintRem();
	void PrintBlockHeader( WORD, WORD );
	void PrintExeBlockHeader( WORD, WORD );
	void PrintExeRunAddr( WORD wAddr );
	void PrintExeInitAddr( WORD wAddr );
	void PrintExeHeader();
	void PrintDirectives();
	void PrintWord( WORD );


private:

	void GetOperandString( char* szIMod, char* szRet, ADDR_TYPES type, int iLen, MEMORY_ACCESS, BYTE*, WORD wRelPC );
	char* SymbolFind( WORD, int, MEMORY_ACCESS );
	void PrintSym( FILE*, char* szName, WORD wAddr );

	DISS_TYPE m_asm_type;
	PROC_TYPE m_proc_type;
	UNDOC_TYPE m_undoc_type;
};

void OutputBlockDiss( DISS_TYPE, int, BYTE*, WORD, WORD );
void DumpSymbols( char* szFilename, DISS_TYPE diss, int iMemClassAllow );

#endif

