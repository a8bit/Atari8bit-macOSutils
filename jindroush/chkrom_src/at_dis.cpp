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
#include "at_dis.h"

struct
{
	char	*instruct;
	int	length;
	int	branch;
	int	immed;
	int	write;
} instable[] =
{
	{ "BRK",				0, 0, 1, 0 },	//00
	{ "ORA\t(*,X)",	1, 0, 0, 0 },	//01
	{ "???",				0, 0, 1, 0 },	//02
	{ "???",				0, 0, 1, 0 },	//03
	{ "???",				0, 0, 1, 0 },	//04
	{ "ORA\t*",			1, 0, 0, 0 },	//05
	{ "ASL\t*",			1, 0, 0, 0 },	//06
	{ "???",				0, 0, 1, 0 },	//07
	{ "PHP",				0, 0, 1, 0 },	//08
	{ "ORA\t#*",		1, 0, 1, 0 },	//09
	{ "ASL",				0, 0, 1, 0 },	//0A
	{ "???",				0, 0, 1, 0 },	//0B
	{ "???",				0, 0, 1, 0 },	//0C
	{ "ORA\t*",			2, 0, 0, 0 },	//0D
	{ "ASL\t*",			2, 0, 0, 0 },	//0E
	{ "???",				0, 0, 1, 0 },	//0F
	{ "BPL\t$*",		1, 1, 1, 0 },	//10
	{ "ORA\t(*),Y",	1, 0, 0, 0 },	//11
	{ "???",				0, 0, 1, 0 },	//12
	{ "???",				0, 0, 1, 0 },	//13
	{ "???",				0, 0, 1, 0 },	//14
	{ "ORA\t*,X",		1, 0, 0, 0 },	//15
	{ "ASL\t*,X",		1, 0, 0, 0 },	//16
	{ "???",				0, 0, 1, 0 },	//17
	{ "CLC",				0, 0, 1, 0 },	//18
	{ "ORA\t*,Y",		2, 0, 0, 0 },	//19
	{ "???",				0, 0, 1, 0 },	//1A
	{ "???",				0, 0, 1, 0 },	//1B
	{ "???",				0, 0, 1, 0 },	//1C
	{ "ORA\t*,X",		2, 0, 0, 0 },	//1D
	{ "ASL\t*,X",		2, 0, 0, 0 },	//1E
	{ "???",				0, 0, 1, 0 },	//1F
	{ "JSR\t*",			2, 0, 0, 0 },	//20
	{ "AND\t(*,X)",	1, 0, 0, 0 },	//21
	{ "???",				0, 0, 1, 0 },	//22
	{ "???",				0, 0, 1, 0 },	//23
	{ "BIT\t*",			1, 0, 0, 0 },	//24
	{ "AND\t*",			1, 0, 0, 0 },	//25
	{ "ROL\t*",			1, 0, 0, 0 },	//26
	{ "???",				0, 0, 1, 0 },	//27
	{ "PLP",				0, 0, 1, 0 },	//28
	{ "AND\t#*",		1, 0, 1, 0 },	//29
	{ "ROL",				0, 0, 1, 0 },	//2A
	{ "???",				0, 0, 1, 0 },	//2B
	{ "BIT\t*",			2, 0, 0, 0 },	//2C
	{ "AND\t*",			2, 0, 0, 0 },	//2D
	{ "ROL\t*",			2, 0, 0, 0 },	//2E
	{ "???",				0, 0, 1, 0 },	//2F
	{ "BMI\t$*",		1, 1, 1, 0 },	//30
	{ "AND\t(*),Y",	1, 0, 0, 0 },	//31
	{ "???",				0, 0, 1, 0 },	//32
	{ "???",				0, 0, 1, 0 },	//33
	{ "???",				0, 0, 1, 0 },	//34
	{ "AND\t*,X",		1, 0, 0, 0 },	//35
	{ "ROL\t*,X",		1, 0, 0, 0 },	//36
	{ "???",				0, 0, 1, 0 },	//37
	{ "SEC",				0, 0, 1, 0 },	//38
	{ "AND\t*,Y",		2, 0, 0, 0 },	//39
	{ "???",				0, 0, 1, 0 },	//3A
	{ "???",				0, 0, 1, 0 },	//3B
	{ "???",				0, 0, 1, 0 },	//3C
	{ "AND\t*,X",		2, 0, 0, 0 },	//3D
	{ "ROL\t*,X",		2, 0, 0, 0 },	//3E
	{ "???",				0, 0, 1, 0 },	//3F
	{ "RTI",				0, 0, 1, 0 },	//40
	{ "EOR\t(*,X)",	1, 0, 0, 0 },	//41
	{ "???",				0, 0, 1, 0 },	//42
	{ "???",				0, 0, 1, 0 },	//43
	{ "???",				0, 0, 1, 0 },	//44
	{ "EOR\t*",			1, 0, 0, 0 },	//45
	{ "LSR\t*",			1, 0, 0, 0 },	//46
	{ "???",				0, 0, 1, 0 },	//47
	{ "PHA",				0, 0, 1, 0 },	//48
	{ "EOR\t#*",		1, 0, 1, 0 },	//49
	{ "LSR",				0, 0, 1, 0 },	//4A
	{ "???",				0, 0, 1, 0 },	//4B
	{ "JMP\t*",			2, 0, 0, 0 },	//4C
	{ "EOR\t*",			2, 0, 0, 0 },	//4D
	{ "LSR\t*",			2, 0, 0, 0 },	//4E
	{ "???",				0, 0, 1, 0 },	//4F
	{ "BVC\t$*",		1, 1, 1, 0 },	//50
	{ "EOR\t(*),Y",	1, 0, 0, 0 },	//51
	{ "???",				0, 0, 1, 0 },	//52
	{ "???",				0, 0, 1, 0 },	//53
	{ "???",				0, 0, 1, 0 },	//54
	{ "EOR\t*,X",		1, 0, 0, 0 },	//55
	{ "LSR\t*,X",		1, 0, 0, 0 },	//56
	{ "???",				0, 0, 1, 0 },	//57
	{ "CLI",				0, 0, 1, 0 },	//58
	{ "EOR\t*,Y",		2, 0, 0, 0 },	//59
	{ "???",				0, 0, 1, 0 },	//5A
	{ "???",				0, 0, 1, 0 },	//5B
	{ "???",				0, 0, 1, 0 },	//5C
	{ "EOR\t*,X",		2, 0, 0, 0 },	//5D
	{ "LSR\t*,X",		2, 0, 0, 0 },	//5E
	{ "???",				0, 0, 1, 0 },	//5F
	{ "RTS",				0, 0, 1, 0 },	//60
	{ "ADC\t(*,X)",	1, 0, 0, 0 },	//61
	{ "???",				0, 0, 1, 0 },	//62
	{ "???",				0, 0, 1, 0 },	//63
	{ "???",				0, 0, 1, 0 },	//64
	{ "ADC\t*",			1, 0, 0, 0 },	//65
	{ "ROR\t*",			1, 0, 0, 0 },	//66
	{ "???",				0, 0, 1, 0 },	//67
	{ "PLA",				0, 0, 1, 0 },	//68
	{ "ADC\t#*",		1, 0, 1, 0 },	//69
	{ "ROR",				0, 0, 1, 0 },	//6A
	{ "???",				0, 0, 1, 0 },	//6B
	{ "JMP\t(*)",		2, 0, 0, 0 },	//6C
	{ "ADC\t*",			2, 0, 0, 0 },	//6D
	{ "ROR\t*",			2, 0, 0, 0 },	//6E
	{ "???",				0, 0, 1, 0 },	//6F
	{ "BVS\t$*",		1, 1, 1, 0 },	//70
	{ "ADC\t(*),Y",	1, 0, 0, 0 },	//71
	{ "???",				0, 0, 1, 0 },	//72
	{ "???",				0, 0, 1, 0 },	//73
	{ "???",				0, 0, 1, 0 },	//74
	{ "ADC\t*,X",		1, 0, 0, 0 },	//75
	{ "ROR\t*,X",		1, 0, 0, 0 },	//76
	{ "???",				0, 0, 1, 0 },	//77
	{ "SEI",				0, 0, 1, 0 },	//78
	{ "ADC\t*,Y",		2, 0, 0, 0 },	//79
	{ "???",				0, 0, 1, 0 },	//7A
	{ "???",				0, 0, 1, 0 },	//7B
	{ "SKW",				1, 0, 1, 0 },	//7C
	{ "ADC\t*,X",		2, 0, 1, 0 },	//7D
	{ "ROR\t*,X",		2, 0, 1, 0 },	//7E
	{ "???",				0, 0, 1, 0 },	//7F
	{ "???",				0, 0, 1, 0 },	//80
	{ "STA\t(*,X)",	1, 0, 0, 1 },	//81
	{ "???",				0, 0, 1, 0 },	//82
	{ "???",				0, 0, 1, 0 },	//83
	{ "STY\t*",	  		1, 0, 0, 1 },	//84
	{ "STA\t*",			1, 0, 0, 1 },	//85
	{ "STX\t*",			1, 0, 0, 1 },	//86
	{ "???",				0, 0, 1, 0 },	//87
	{ "DEY",				0, 0, 1, 0 },	//88
	{ "???",				0, 0, 1, 0 },	//89
	{ "TXA",				0, 0, 1, 0 },	//8A
	{ "???",				0, 0, 1, 0 },	//8B
	{ "STY\t*",			2, 0, 0, 1 },	//8C
	{ "STA\t*",			2, 0, 0, 1 },	//8D
	{ "STX\t*",			2, 0, 0, 1 },	//8E
	{ "???",				0, 0, 1, 0 },	//8F
	{ "BCC\t$*",		1, 1, 1, 0 },	//90
	{ "STA\t(*),Y",	1, 0, 0, 1 },	//91
	{ "???",				0, 0, 1, 0 },	//92
	{ "???",				0, 0, 1, 0 },	//93
	{ "STY\t*,X",		1, 0, 0, 1 },	//94
	{ "STA\t*,X",		1, 0, 0, 1 },	//95
	{ "STX\t*,Y",		1, 0, 0, 1 },	//96
	{ "???",				0, 0, 1, 0 },	//97
	{ "TYA",				0, 0, 1, 0 },	//98
	{ "STA\t*,Y",		2, 0, 0, 1 },	//99
	{ "TXS",				0, 0, 1, 0 },	//9A
	{ "???",				0, 0, 1, 0 },	//9B
	{ "???",				0, 0, 1, 0 },	//9C
	{ "STA\t*,X",		2, 0, 0, 1 },	//9D
	{ "???",				0, 0, 1, 0 },	//9E
	{ "???",				0, 0, 1, 0 },	//9F
	{ "LDY\t#*",		1, 0, 1, 0 },	//A0
	{ "LDA\t(*,X)",	1, 0, 0, 0 },	//A1
	{ "LDX\t#*",		1, 0, 1, 0 },	//A2
	{ "???",				0, 0, 1, 0 },	//A3
	{ "LDY\t*",			1, 0, 0, 0 },	//A4
	{ "LDA\t*",			1, 0, 0, 0 },	//A5
	{ "LDX\t*",			1, 0, 0, 0 },	//A6
	{ "???",				0, 0, 1, 0 },	//A7
	{ "TAY",				0, 0, 1, 0 },	//A8
	{ "LDA\t#*",		1, 0, 1, 0 },	//A9
	{ "TAX",				0, 0, 1, 0 },	//AA
	{ "???",				0, 0, 1, 0 },	//AB
	{ "LDY\t*",			2, 0, 0, 0 },	//AC
	{ "LDA\t*",			2, 0, 0, 0 },	//AD
	{ "LDX\t*",			2, 0, 0, 0 },	//AE
	{ "???",				0, 0, 1, 0 },	//AF
	{ "BCS\t$*",		1, 1, 1, 0 },	//B0
	{ "LDA\t(*),Y",	1, 0, 0, 0 },	//B1
	{ "???",				0, 0, 1, 0 },	//B2
	{ "???",				0, 0, 1, 0 },	//B3
	{ "LDY\t*,X",		1, 0, 0, 0 },	//B4
	{ "LDA\t*,X",		1, 0, 0, 0 },	//B5
	{ "LDX\t*,Y",		1, 0, 0, 0 },	//B6
	{ "???",				0, 0, 1, 0 },	//B7
	{ "CLV",				0, 0, 1, 0 },	//B8
	{ "LDA\t*,Y",		2, 0, 0, 0 },	//B9
	{ "TSX",				0, 0, 1, 0 },	//BA
	{ "???",				0, 0, 1, 0 },	//BB
	{ "LDY\t*,X",		2, 0, 0, 0 },	//BC
	{ "LDA\t*,X",		2, 0, 0, 0 },	//BD
	{ "LDX\t*,Y",		2, 0, 0, 0 },	//BE
	{ "???",				0, 0, 1, 0 },	//BF
	{ "CPY\t#*",		1, 0, 1, 0 },	//C0
	{ "CMP\t(*,X)",	1, 0, 0, 0 },	//C1
	{ "???",				0, 0, 1, 0 },	//C2
	{ "???",				0, 0, 1, 0 },	//C3
	{ "CPY\t*",			1, 0, 0, 0 },	//C4
	{ "CMP\t*",			1, 0, 0, 0 },	//C5
	{ "DEC\t*",			1, 0, 0, 0 },	//C6
	{ "???",				0, 0, 1, 0 },	//C7
	{ "INY",				0, 0, 1, 0 },	//C8
	{ "CMP\t#*",		1, 0, 1, 0 },	//C9
	{ "DEX",				0, 0, 1, 0 },	//CA
	{ "???",				0, 0, 1, 0 },	//CB
	{ "CPY\t*",			2, 0, 0, 0 },	//CC
	{ "CMP\t*",			2, 0, 0, 0 },	//CD
	{ "DEC\t*",			2, 0, 0, 0 },	//CE
	{ "???",				0, 0, 1, 0 },	//CF
	{ "BNE\t$*",		1, 1, 1, 0 },	//D0
	{ "CMP\t(*),Y",	1, 0, 0, 0 },	//D1
	{ "???",				0, 0, 1, 0 },	//D2
	{ "???",				0, 0, 1, 0 },	//D3
	{ "???",				0, 0, 1, 0 },	//D4
	{ "CMP\t*,X",		1, 0, 0, 0 },	//D5
	{ "DEC\t*,X",		1, 0, 0, 0 },	//D6
	{ "???",				0, 0, 1, 0 },	//D7
	{ "CLD",				0, 0, 1, 0 },	//D8
	{ "CMP\t*,Y",		2, 0, 0, 0 },	//D9
	{ "???",				0, 0, 1, 0 },	//DA
	{ "???",				0, 0, 1, 0 },	//DB
	{ "???",				0, 0, 1, 0 },	//DC
	{ "CMP\t*,X",		2, 0, 0, 0 },	//DD
	{ "DEC\t*,X",		2, 0, 0, 0 },	//DE
	{ "???",				0, 0, 1, 0 },	//DF
	{ "CPX\t#*",		1, 0, 1, 0 },	//E0
	{ "SBC\t(*,X)",	1, 0, 0, 0 },	//E1
	{ "???",				0, 0, 1, 0 },	//E2
	{ "???",				0, 0, 1, 0 },	//E3
	{ "CPX\t*",			1, 0, 0, 0 },	//E4
	{ "SBC\t*",			1, 0, 0, 0 },	//E5
	{ "INC\t*",			1, 0, 0, 0 },	//E6
	{ "???",				0, 0, 1, 0 },	//E7
	{ "INX",				0, 0, 1, 0 },	//E8
	{ "SBC\t#*",		1, 0, 1, 0 },	//E9
	{ "NOP",				0, 0, 1, 0 },	//EA
	{ "???",				0, 0, 1, 0 },	//EB
	{ "CPX\t*",			2, 0, 0, 0 },	//EC
	{ "SBC\t*",			2, 0, 0, 0 },	//ED
	{ "INC\t*",			2, 0, 0, 0 },	//EE
	{ "???",				0, 0, 1, 0 },	//EF
	{ "BEQ\t$*",		1, 1, 1, 0 },	//F0
	{ "SBC\t(*),Y",	1, 0, 0, 0 },	//F1
	{ "???",				0, 0, 1, 0 },	//F2
	{ "???",				0, 0, 1, 0 },	//F3
	{ "???",				0, 0, 1, 0 },	//F4
	{ "SBC\t*,X",		1, 0, 0, 0 },	//F5
	{ "INC\t*,X",		1, 0, 0, 0 },	//F6
	{ "???",				0, 0, 1, 0 },	//F7
	{ "SED",				0, 0, 1, 0 },	//F8
	{ "SBC\t*,Y",		2, 0, 0, 0 },	//F9
	{ "???",				0, 0, 1, 0 },	//FA
	{ "???",				0, 0, 1, 0 },	//FB
	{ "???",				0, 0, 1, 0 },	//FC
	{ "SBC\t*,X",		2, 0, 0, 0 },	//FD
	{ "INC\t*,X",		2, 0, 0, 0 },	//FE
	{ "???",				0, 0, 1, 0 },	//FF
	{ "\0",				0, 0, 1, 0 }
};

typedef struct
{
	char* name;
	WORD addr;
} symtable_rec;

symtable_rec symtable[] = { 
	{ "NGFLAG",		0x0001 },			
	{ "CASINI",		0x0002 },			
	{ "CASINI+1",	0x0003 },			
	{ "RAMLO",		0x0004 },			
	{ "RAMLO+1",	0x0005 },			
	{ "TRAMSZ",		0x0006 },			
	{ "CMCMD",		0x0007 },			
	{ "WARMST",		0x0008 },			
	{ "BOOT",		0x0009 },			
	{ "DOSVEC",		0x000A },			
	{ "DOSVEC+1",	0x000B },			
	{ "DOSINI",		0x000C },			
	{ "DOSINI+1",	0x000D },			
	{ "APPMHI",		0x000E },			
	{ "APPMHI+1",	0x000F },			
	{ "POKMSK",		0x0010 },			
	{ "BRKKEY",		0x0011 },			
	{ "RTCLOK",		0x0012 },			
	{ "RTCLOK+1",	0x0013 },			
	{ "RTCLOK+2",	0x0014 },			
	{ "BUFADR",		0x0015 },			
	{ "BUFADR+1",	0x0016 },			
	{ "ICCOMT",		0x0017 },			
	{ "DSKFMS",		0x0018 },			
	{ "DSKFMS+1",	0x0019 },			
	{ "DSKUTL",		0x001A },			
	{ "DSKUTL+1",	0x001B },			
	{ "ABUFPT",		0x001C },			
	{ "ABUFPT+1",	0x001D },			
	{ "ABUFPT+2",	0x001E },			
	{ "ABUFPT+3",	0x001F },			
	{ "ICHIDZ",		0x0020 },			
	{ "ICDNOZ",		0x0021 },			
	{ "ICCOMZ",		0x0022 },			
	{ "ICSTAZ",		0x0023 },			
	{ "ICBALZ",		0x0024 },			
	{ "ICBAHZ",		0x0025 },			
	{ "ICPTLZ",		0x0026 },			
	{ "ICPTHZ",		0x0027 },			
	{ "ICBLLZ",		0x0028 },			
	{ "ICBLHZ",		0x0029 },			
	{ "ICAX1Z",		0x002A },			
	{ "ICAX2Z",		0x002B },			
	{ "ICAX3Z",		0x002C },			
	{ "ICAX4Z",		0x002D },			
	{ "ICAX5Z",		0x002E },			
	{ "ICAX6Z",		0x002F },			
	{ "STATUS",		0x0030 },			
	{ "CHKSUM",		0x0031 },			
	{ "BUFRLO",		0x0032 },			
	{ "BUFRHI",		0x0033 },			
	{ "BFENLO",		0x0034 },			
	{ "BFENHI",		0x0035 },			
	{ "LTEMP",		0x0036 },			
	{ "LTEMP+1",	0x0037 },			
	{ "BUFRFL",		0x0038 },			
	{ "RECVDN",		0x0039 },			
	{ "XMTDON",		0x003A },			
	{ "CHKSNT",		0x003B },			
	{ "NOCKSM",		0x003C },			
	{ "BPTR",		0x003D },			
	{ "FTYPE",		0x003E },			
	{ "FEOF",		0x003F },			
	{ "FREQ",		0x0040 },			
	{ "SOUNDR",		0x0041 },			
	{ "CRITIC",		0x0042 },			
	{ "FMSZPG",		0x0043 },			
	{ "FMSZPG+1",	0x0044 },			
	{ "FMSZPG+2",	0x0045 },			
	{ "FMSZPG+3",	0x0046 },			
	{ "FMSZPG+4",	0x0047 },			
	{ "FMSZPG+5",	0x0048 },			
	{ "FMSZPG+6",	0x0049 },			
	{ "ZCHAIN",		0x004A },			
	{ "ZCHAIN+1",	0x004B },			
	{ "DSTAT",		0x004C },			
	{ "ATRACT",		0x004D },			
	{ "DRKMSK",		0x004E },			
	{ "COLRSH",		0x004F },			
	{ "TEMP",		0x0050 },			
	{ "HOLD1",		0x0051 },			
	{ "LMARGN",		0x0052 },			
	{ "RMARGN",		0x0053 },			
	{ "ROWCRS",		0x0054 },			
	{ "COLCRS",		0x0055 },			
	{ "COLCRS+1",	0x0056 },			
	{ "DINDEX",		0x0057 },			
	{ "SAVMSC",		0x0058 },			
	{ "SAVMSC+1",	0x0059 },			
	{ "OLDROW",		0x005A },			
	{ "OLDCOL",		0x005B },			
	{ "OLDCOL+1",	0x005C },			
	{ "OLDCHR",		0x005D },			
	{ "OLDADR",		0x005E },			
	{ "OLDADR+1",	0x005F },			
	{ "FKDEF",		0x0060 },			
	{ "FKDEF+1",	0x0061 },			
	{ "PALNTS",		0x0062 },			
	{ "LOGCOL",		0x0063 },			
	{ "ADRESS",		0x0064 },			
	{ "ADRESS+1",	0x0065 },			
	{ "MLTTMP",		0x0066 },			
	{ "MLTTMP+1",	0x0067 },			
	{ "SAVADR",		0x0068 },			
	{ "SAVADR+1",	0x0069 },			
	{ "RAMTOP",		0x006A },			
	{ "BUFCNT",		0x006B },			
	{ "BUFSTR",		0x006C },			
	{ "BUFSTR+1",	0x006D },			
	{ "BITMSK",		0x006E },			
	{ "SHFAMT",		0x006F },			
	{ "ROWAC",		0x0070 },			
	{ "ROWAC+1",	0x0071 },			
	{ "COLAC",		0x0072 },			
	{ "COLAC+1",	0x0073 },			
	{ "ENDPT",		0x0074 },			
	{ "ENDPT+1",	0x0075 },			
	{ "DELTAR",		0x0076 },			
	{ "DELTAC",		0x0077 },			
	{ "DELTAC+1",	0x0078 },			
	{ "KEYDEF",		0x0079 },			
	{ "KEYDEF+1",	0x007A },			
	{ "SWPFLG",		0x007B },			
	{ "HOLDCH",		0x007C },			
	{ "INSDAT",		0x007D },			
	{ "COUNTR",		0x007E },			
	{ "COUNTR+1",	0x007F },			
	{ "LOMEM",		0x0080 },			
	{ "LOMEM+1",	0x0081 },			
	{ "VNTP",		0x0082 },			
	{ "VNTP+1",		0x0083 },			
	{ "VNTD",		0x0084 },			
	{ "VNTD+1",		0x0085 },			
	{ "VVTP",		0x0086 },			
	{ "VVTP+1",		0x0087 },			
	{ "STMTAB",		0x0088 },			
	{ "STMTAB+1",	0x0089 },			
	{ "STMCUR",		0x008A },			
	{ "STMCUR+1",	0x008B },			
	{ "STARP",		0x008C },			
	{ "STARP+1",	0x008D },			
	{ "RUNSTK",		0x008E },			
	{ "RUNSTK+1",	0x008F },			
	{ "TOPSTK",		0x0090 },			
	{ "TOPSTK+1",	0x0091 },			
	{ "MEOLFLG",	0x0092 },			
	{ "POKADR",		0x0095 },			
	{ "POKADR+1",	0x0096 },			
	{ "DATAD",		0x00B6 },			
	{ "DATALN",		0x00B7 },			
	{ "DATALN+1",	0x00B8 },			
	{ "STOPLN",		0x00BA },			
	{ "STOPLN+1",	0x00BB },			
	{ "SAVCUR",		0x00BE },			
	{ "IOCMD",		0x00C0 },			
	{ "IODVC",		0x00C1 },			
	{ "PROMPT",		0x00C2 },			
	{ "ERRSAVE",	0x00C3 },			
	{ "COLOUR",		0x00C8 },			
	{ "PTABW",		0x00C9 },			
	{ "LOADFLG",	0x00CA },			
	{ "FR0",			0x00D4 },			
	{ "FR0+1",		0x00D5 },			
	{ "FR0+2",		0x00D6 },			
	{ "FR0+3",		0x00D7 },			
	{ "FR0+4",		0x00D8 },			
	{ "FR0+5",		0x00D9 },			
	{ "FRE",			0x00DA },			
	{ "FRE+1",		0x00DB },			
	{ "FRE+2",		0x00DC },			
	{ "FRE+3",		0x00DD },			
	{ "FRE+4",		0x00DE },			
	{ "FRE+5",		0x00DF },			
	{ "FR1",			0x00E0 },			
	{ "FR1+1",		0x00E1 },			
	{ "FR1+2",		0x00E2 },			
	{ "FR1+3",		0x00E3 },			
	{ "FR1+4",		0x00E4 },			
	{ "FR1+5",		0x00E5 },			
	{ "FR2",			0x00E6 },			
	{ "FR2+1",		0x00E7 },			
	{ "FR2+2",		0x00E8 },			
	{ "FR2+3",		0x00E9 },			
	{ "FR2+4",		0x00EA },			
	{ "FR2+5",		0x00EB },			
	{ "FRX",			0x00EC },			
	{ "EEXP",		0x00ED },			
	{ "NSIGN",		0x00EE },			
	{ "ESIGN",		0x00EF },			
	{ "FCHRFLG",	0x00F0 },			
	{ "DIGRT",		0x00F1 },			
	{ "CIX",			0x00F2 },			
	{ "INBUFF",		0x00F3 },			
	{ "INBUFF+1",	0x00F4 },			
	{ "ZTEMP1",		0x00F5 },			
	{ "ZTEMP1+1",	0x00F6 },			
	{ "ZTEMP4",		0x00F7 },			
	{ "ZTEMP4+1",	0x00F8 },			
	{ "ZTEMP3",		0x00F9 },			
	{ "ZTEMP3+1",	0x00FA },			
	{ "RADFLG",		0x00FB },			
	{ "FLPTR",		0x00FC },			
	{ "FLPTR+1",	0x00FD },			
	{ "FPTR2",		0x00FE },			
	{ "FPTR2+1",	0x00FF },			

	{ "VDSLST",		0x0200 },			
	{ "VDSLST+1",	0x0201 },			
	{ "VPRCED",		0x0202 },			
	{ "VPRCED+1",	0x0203 },			
	{ "VINTER",		0x0204 },			
	{ "VINTER+1",	0x0205 },			
	{ "VBREAK",		0x0206 },			
	{ "VBREAK+1",	0x0207 },			
	{ "VKEYBD",		0x0208 },			
	{ "VKEYBD+1",	0x0209 },			
	{ "VSERIN",		0x020A },			
	{ "VSERIN+1",	0x020B },			
	{ "VSEROR",		0x020C },			
	{ "VSEROR+1",	0x020D },			
	{ "VSEROC",		0x020E },			
	{ "VSEROC+1",	0x020F },			
	{ "VTIMR1",		0x0210 },			
	{ "VTIMR1+1",	0x0211 },			
	{ "VTIMR2",		0x0212 },			
	{ "VTIMR2+1",	0x0213 },			
	{ "VTIMR4",		0x0214 },			
	{ "VTIMR4+1",	0x0215 },			
	{ "VIMIRQ",		0x0216 },			
	{ "VIMIRQ+1",	0x0217 },			
	{ "CDTMV1",		0x0218 },			
	{ "CDTMV1+1",	0x0219 },			
	{ "CDTMV2",		0x021A },			
	{ "CDTMV2+1",	0x021B },			
	{ "CDTMV3",		0x021C },			
	{ "CDTMV3+1",	0x021D },			
	{ "CDTMV4",		0x021E },			
	{ "CDTMV4+1",	0x021F },			
	{ "CDTMV5",		0x0220 },			
	{ "CDTMV5+1",	0x0221 },			
	{ "VVBLKI",		0x0222 },			
	{ "VVBLKI+1",	0x0223 },			
	{ "VVBLKD",		0x0224 },			
	{ "VVBLKD+1",	0x0225 },			
	{ "CDTMA1",		0x0226 },			
	{ "CDTMA1+1",	0x0227 },			
	{ "CDTMA2",		0x0228 },			
	{ "CDTMA2+1",	0x0229 },			
	{ "CDTMF3",		0x022A },			
	{ "SRTIMR",		0x022B },			
	{ "CDTMF4",		0x022C },			
	{ "INTEMP",		0x022D },			
	{ "CDTMF5",		0x022E },			
	{ "SDMCTL",		0x022F },			
	{ "SDLSTL",		0x0230 },			
	{ "SDLSTH",		0x0231 },			
	{ "SSKCTL",		0x0232 },			
	{ "SPARE",		0x0233 },			
	{ "LPENH",		0x0234 },			
	{ "LPENV",		0x0235 },			
	{ "BRKKY",		0x0236 },			
	{ "BRKKY+1",	0x0237 },			
	{ "VPIRQ",		0x0238 },			
	{ "VPIRQ+1",	0x0239 },			
	{ "CDEVIC",		0x023A },			
	{ "CCOMND",		0x023B },			
	{ "CAUX1",		0x023C },			
	{ "CAUX2",		0x023D },			
	{ "TMPSIO",		0x023E },			
	{ "ERRFLG",		0x023F },			
	{ "DFLAGS",		0x0240 },			
	{ "DBSECT",		0x0241 },			
	{ "BOOTAD",		0x0242 },			
	{ "BOOTAD+1",	0x0243 },			
	{ "COLDST",		0x0244 },			
	{ "RECLEN",		0x0245 },			
	{ "DSKTIM",		0x0246 },			
	{ "PDVMSK",		0x0247 },			
	{ "SHPDVS",		0x0248 },			
	{ "PDMSK",		0x0249 },			
	{ "RELADR",		0x024A },			
	{ "RELADR+1",	0x024B },			
	{ "PPTMPA",		0x024C },			
	{ "PPTMPX",		0x024D },			
	{ "CHSALT",		0x026B },			
	{ "VSFLAG",		0x026C },			
	{ "KEYDIS",		0x026D },			
	{ "FINE",		0x026E },			
	{ "GPRIOR",		0x026F },			
	{ "PADDL0",		0x0270 },			
	{ "PADDL1",		0x0271 },			
	{ "PADDL2",		0x0272 },			
	{ "PADDL3",		0x0273 },			
	{ "PADDL4",		0x0274 },			
	{ "PADDL5",		0x0275 },			
	{ "PADDL6",		0x0276 },			
	{ "PADDL7",		0x0277 },			
	{ "STICK0",		0x0278 },			
	{ "STICK1",		0x0279 },			
	{ "STICK2",		0x027A },			
	{ "STICK3",		0x027B },			
	{ "PTRIG0",		0x027C },			
	{ "PTRIG1",		0x027D },			
	{ "PTRIG2",		0x027E },			
	{ "PTRIG3",		0x027F },			
	{ "PTRIG4",		0x0280 },			
	{ "PTRIG5",		0x0281 },			
	{ "PTRIG6",		0x0282 },			
	{ "PTRIG7",		0x0283 },			
	{ "STRIG0",		0x0284 },			
	{ "STRIG1",		0x0285 },			
	{ "STRIG2",		0x0286 },			
	{ "STRIG3",		0x0287 },			
	{ "HIBYTE",		0x0288 },			
	{ "WMODE",		0x0289 },			
	{ "BLIM",		0x028A },			
	{ "IMASK",		0x028B },			
	{ "JVECK",		0x028C },			
	{ "NEWADR",		0x028E },			
	{ "TXTROW",		0x0290 },			
	{ "TXTCOL",		0x0291 },			
	{ "TXTCOL+1",	0x0292 },			
	{ "TINDEX",		0x0293 },			
	{ "TXTMSC",		0x0294 },			
	{ "TXTMSC+1",	0x0295 },			
	{ "TXTOLD",		0x0296 },			
	{ "TXTOLD+1",	0x0297 },			
	{ "TXTOLD+2",	0x0298 },			
	{ "TXTOLD+3",	0x0299 },			
	{ "TXTOLD+4",	0x029A },			
	{ "TXTOLD+5",	0x029B },			
	{ "CRETRY",		0x029C },			
	{ "HOLD3",		0x029D },			
	{ "SUBTMP",		0x029E },			
	{ "HOLD2",		0x029F },			
	{ "DMASK",		0x02A0 },			
	{ "TMPLBT",		0x02A1 },			
	{ "ESCFLG",		0x02A2 },			
	{ "TABMAP",		0x02A3 },			
	{ "TABMAP+1",	0x02A4 },			
	{ "TABMAP+2",	0x02A5 },			
	{ "TABMAP+3",	0x02A6 },			
	{ "TABMAP+4",	0x02A7 },			
	{ "TABMAP+5",	0x02A8 },			
	{ "TABMAP+6",	0x02A9 },			
	{ "TABMAP+7",	0x02AA },			
	{ "TABMAP+8",	0x02AB },			
	{ "TABMAP+9",	0x02AC },			
	{ "TABMAP+A",	0x02AD },			
	{ "TABMAP+B",	0x02AE },			
	{ "TABMAP+C",	0x02AF },			
	{ "TABMAP+D",	0x02B0 },			
	{ "TABMAP+E",	0x02B1 },			
	{ "LOGMAP",		0x02B2 },			
	{ "LOGMAP+1",	0x02B3 },			
	{ "LOGMAP+2",	0x02B4 },			
	{ "LOGMAP+3",	0x02B5 },			
	{ "INVFLG",		0x02B6 },			
	{ "FILFLG",		0x02B7 },			
	{ "TMPROW",		0x02B8 },			
	{ "TMPCOL",		0x02B9 },			
	{ "TMPCOL+1",	0x02BA },			
	{ "SCRFLG",		0x02BB },			
	{ "HOLD4",		0x02BC },			
	{ "DRETRY",		0x02BD },			
	{ "SHFLOC",		0x02BE },			
	{ "BOTSCR",		0x02BF },			
	{ "PCOLR0",		0x02C0 },			
	{ "PCOLR1",		0x02C1 },			
	{ "PCOLR2",		0x02C2 },			
	{ "PCOLR3",		0x02C3 },			
	{ "COLOR0",		0x02C4 },			
	{ "COLOR1",		0x02C5 },			
	{ "COLOR2",		0x02C6 },			
	{ "COLOR3",		0x02C7 },			
	{ "COLOR4",		0x02C8 },			
	{ "RUNADR",		0x02C9 },			
	{ "RUNADR+1",	0x02CA },			
	{ "HIUSED",		0x02CB },			
	{ "HIUSED+1",	0x02CC },			
	{ "ZHIUSE",		0x02CD },			
	{ "ZHIUSE+1",	0x02CE },			
	{ "GBYTEA",		0x02CF },			
	{ "GBYTEA+1",	0x02D0 },			
	{ "LOADAD",		0x02D1 },			
	{ "LOADAD+1",	0x02D2 },			
	{ "ZLOADA",		0x02D3 },			
	{ "ZLOADA+1",	0x02D4 },			
	{ "DSCTLN",		0x02D5 },			
	{ "DSCTLN+1",	0x02D6 },			
	{ "ACMISR",		0x02D7 },			
	{ "ACMISR+1",	0x02D8 },			
	{ "KRPDER",		0x02D9 },			
	{ "KEYREP",		0x02DA },			
	{ "NOCLIK",		0x02DB },			
	{ "HELPFG",		0x02DC },			
	{ "DMASAV",		0x02DD },			
	{ "PBPNT",		0x02DE },			
	{ "PBUFSZ",		0x02DF },			
	{ "RUNAD",		0x02E0 },			
	{ "RUNAD+1",	0x02E1 },			
	{ "INITAD",		0x02E2 },			
	{ "INITAD+1",	0x02E3 },			
	{ "RAMSIZ",		0x02E4 },			
	{ "MEMTOP",		0x02E5 },			
	{ "MEMTOP+1",	0x02E6 },			
	{ "MEMLO",		0x02E7 },			
	{ "MEMLO+1",	0x02E8 },			
	{ "HNDLOD",		0x02E9 },			
	{ "DVSTAT",		0x02EA },			
	{ "DVSTAT+1",	0x02EB },			
	{ "DVSTAT+2",	0x02EC },			
	{ "DVSTAT+3",	0x02ED },			
	{ "CBAUDL",		0x02EE },			
	{ "CBAUDH",		0x02EF },			
	{ "CRSINH",		0x02F0 },			
	{ "KEYDEL",		0x02F1 },			
	{ "CH1",			0x02F2 },			
	{ "CHACT",		0x02F3 },			
	{ "CHBAS",		0x02F4 },			
	{ "NEWROW",		0x02F5 },			
	{ "NEWCOL",		0x02F6 },			
	{ "NEWCOL+1",	0x02F7 },			
	{ "ROWINC",		0x02F8 },			
	{ "COLINC",		0x02F9 },			
	{ "CHAR",		0x02FA },			
	{ "ATACHR",		0x02FB },			
	{ "CH",			0x02FC },			
	{ "FILDAT",		0x02FD },			
	{ "DSPFLG",		0x02FE },			
	{ "SSFLAG",		0x02FF },			


	{ "DDEVIC",		0x0300 },			
	{ "DUNIT",		0x0301 },			
	{ "DCOMND",		0x0302 },			
	{ "DSTATS",		0x0303 },			
	{ "DBUFLO",		0x0304 },			
	{ "DBUFHI",		0x0305 },			
	{ "DTIMLO",		0x0306 },			
	{ "DUNUSE",		0x0307 },			
	{ "DBYTLO",		0x0308 },			
	{ "DBYTHI",		0x0309 },			
	{ "DAUX1",		0x030A },			
	{ "DAUX2",		0x030B },			
	{ "TIMER1",		0x030C },			
	{ "TIMER1+1",	0x030D },			
	{ "ADDCOR",		0x030E },			
	{ "CASFLG",		0x030F },			
	{ "TIMER2",		0x0310 },			
	{ "TIMER2+1",	0x0311 },			
	{ "TEMP1",		0x0312 },			
	{ "TEMP1+1",	0x0313 },			
	{ "TEMP2",		0x0314 },			
	{ "TEMP3",		0x0315 },			
	{ "SAVIO",		0x0316 },			
	{ "TIMFLG",		0x0317 },			
	{ "STACKP",		0x0318 },			
	{ "TSTAT",		0x0319 },			
	{ "HATABS",		0x031A },			/*HATABS 1-34*/
	{ "PUTBT1",		0x033D },			
	{ "PUTBT2",		0x033E },			
	{ "PUTBT3",		0x033F },			
	{ "B0-ICHID",	0x0340 },			
	{ "B0-ICDNO",	0x0341 },			
	{ "B0-ICCOM",	0x0342 },			
	{ "B0-ICSTA",	0x0343 },			
	{ "B0-ICBAL",	0x0344 },			
	{ "B0-ICBAH",	0x0345 },			
	{ "B0-ICPTL",	0x0346 },			
	{ "B0-ICPTH",	0x0347 },			
	{ "B0-ICBLL",	0x0348 },			
	{ "B0-ICBLH",	0x0349 },			
	{ "B0-ICAX1",	0x034A },			
	{ "B0-ICAX2",	0x034B },			
	{ "B0-ICAX3",	0x034C },			
	{ "B0-ICAX4",	0x034D },			
	{ "B0-ICAX5",	0x034E },			
	{ "B0-ICAX6",	0x034F },			
	{ "B1-ICHID",	0x0350 },			
	{ "B1-ICDNO",	0x0351 },			
	{ "B1-ICCOM",	0x0352 },			
	{ "B1-ICSTA",	0x0353 },			
	{ "B1-ICBAL",	0x0354 },			
	{ "B1-ICBAH",	0x0355 },			
	{ "B1-ICPTL",	0x0356 },			
	{ "B1-ICPTH",	0x0357 },			
	{ "B1-ICBLL",	0x0358 },			
	{ "B1-ICBLH",	0x0359 },			
	{ "B1-ICAX1",	0x035A },			
	{ "B1-ICAX2",	0x035B },			
	{ "B1-ICAX3",	0x035C },			
	{ "B1-ICAX4",	0x035D },			
	{ "B1-ICAX5",	0x035E },			
	{ "B1-ICAX6",	0x035F },			
	{ "B2-ICHID",	0x0360 },			
	{ "B2-ICDNO",	0x0361 },			
	{ "B2-ICCOM",	0x0362 },			
	{ "B2-ICSTA",	0x0363 },			
	{ "B2-ICBAL",	0x0364 },			
	{ "B2-ICBAH",	0x0365 },			
	{ "B2-ICPTL",	0x0366 },			
	{ "B2-ICPTH",	0x0367 },			
	{ "B2-ICBLL",	0x0368 },			
	{ "B2-ICBLH",	0x0369 },			
	{ "B2-ICAX1",	0x036A },			
	{ "B2-ICAX2",	0x036B },			
	{ "B2-ICAX3",	0x036C },			
	{ "B2-ICAX4",	0x036D },			
	{ "B2-ICAX5",	0x036E },			
	{ "B2-ICAX6",	0x036F },			
	{ "B3-ICHID",	0x0370 },			
	{ "B3-ICDNO",	0x0371 },			
	{ "B3-ICCOM",	0x0372 },			
	{ "B3-ICSTA",	0x0373 },			
	{ "B3-ICBAL",	0x0374 },			
	{ "B3-ICBAH",	0x0375 },			
	{ "B3-ICPTL",	0x0376 },			
	{ "B3-ICPTH",	0x0377 },			
	{ "B3-ICBLL",	0x0378 },			
	{ "B3-ICBLH",	0x0379 },			
	{ "B3-ICAX1",	0x037A },			
	{ "B3-ICAX2",	0x037B },			
	{ "B3-ICAX3",	0x037C },			
	{ "B3-ICAX4",	0x037D },			
	{ "B3-ICAX5",	0x037E },			
	{ "B3-ICAX6",	0x037F },			
	{ "B4-ICHID",	0x0380 },			
	{ "B4-ICDNO",	0x0381 },			
	{ "B4-ICCOM",	0x0382 },			
	{ "B4-ICSTA",	0x0383 },			
	{ "B4-ICBAL",	0x0384 },			
	{ "B4-ICBAH",	0x0385 },			
	{ "B4-ICPTL",	0x0386 },			
	{ "B4-ICPTH",	0x0387 },			
	{ "B4-ICBLL",	0x0388 },			
	{ "B4-ICBLH",	0x0389 },			
	{ "B4-ICAX1",	0x038A },			
	{ "B4-ICAX2",	0x038B },			
	{ "B4-ICAX3",	0x038C },			
	{ "B4-ICAX4",	0x038D },			
	{ "B4-ICAX5",	0x038E },			
	{ "B4-ICAX6",	0x038F },			
	{ "B5-ICHID",	0x0390 },			
	{ "B5-ICDNO",	0x0391 },			
	{ "B5-ICCOM",	0x0392 },			
	{ "B5-ICSTA",	0x0393 },			
	{ "B5-ICBAL",	0x0394 },			
	{ "B5-ICBAH",	0x0395 },			
	{ "B5-ICPTL",	0x0396 },			
	{ "B5-ICPTH",	0x0397 },			
	{ "B5-ICBLL",	0x0398 },			
	{ "B5-ICBLH",	0x0399 },			
	{ "B5-ICAX1",	0x039A },			
	{ "B5-ICAX2",	0x039B },			
	{ "B5-ICAX3",	0x039C },			
	{ "B5-ICAX4",	0x039D },			
	{ "B5-ICAX5",	0x039E },			
	{ "B5-ICAX6",	0x039F },			
	{ "B6-ICHID",	0x03A0 },			
	{ "B6-ICDNO",	0x03A1 },			
	{ "B6-ICCOM",	0x03A2 },			
	{ "B6-ICSTA",	0x03A3 },			
	{ "B6-ICBAL",	0x03A4 },			
	{ "B6-ICBAH",	0x03A5 },			
	{ "B6-ICPTL",	0x03A6 },			
	{ "B6-ICPTH",	0x03A7 },			
	{ "B6-ICBLL",	0x03A8 },			
	{ "B6-ICBLH",	0x03A9 },			
	{ "B6-ICAX1",	0x03AA },			
	{ "B6-ICAX2",	0x03AB },			
	{ "B6-ICAX3",	0x03AC },			
	{ "B6-ICAX4",	0x03AD },			
	{ "B6-ICAX5",	0x03AE },			
	{ "B6-ICAX6",	0x03AF },			
	{ "B7-ICHID",	0x03B0 },			
	{ "B7-ICDNO",	0x03B1 },			
	{ "B7-ICCOM",	0x03B2 },			
	{ "B7-ICSTA",	0x03B3 },			
	{ "B7-ICBAL",	0x03B4 },			
	{ "B7-ICBAH",	0x03B5 },			
	{ "B7-ICPTL",	0x03B6 },			
	{ "B7-ICPTH",	0x03B7 },			
	{ "B7-ICBLL",	0x03B8 },			
	{ "B7-ICBLH",	0x03B9 },			
	{ "B7-ICAX1",	0x03BA },			
	{ "B7-ICAX2",	0x03BB },			
	{ "B7-ICAX3",	0x03BC },			
	{ "B7-ICAX4",	0x03BD },			
	{ "B7-ICAX5",	0x03BE },			
	{ "B7-ICAX6",	0x03BF },			
	{ "PRNBUF",		0x03C0 },			/*PRNBUF 1-39 */
	{ "SUPERF",		0x03E8 },			
	{ "CKEY",		0x03E9 },			
	{ "CASSBT",		0x03EA },			
	{ "CARTCK",		0x03EB },			
	{ "DERRF",		0x03EC },			
	{ "ACMVAR",		0x03ED },			/*ACMVAR 1-10*/
	{ "BASICF",		0x03F8 },			
	{ "MINTLK",		0x03F9 },			
	{ "GINTLK",		0x03FA },			
	{ "CHLINK",		0x03FB },			
	{ "CHLINK+1",	0x03FC },			
	{ "CASBUF",		0x03FD },			
 
	{ "M0PF",		0xD000 },			
	{ "HPOSP0",		0xD000 },			
	{ "M1PF",		0xD001 },			
	{ "HPOSP1",		0xD001 },			
	{ "M2PF",		0xD002 },			
	{ "HPOSP2",		0xD002 },			
	{ "M3PF",		0xD003 },			
	{ "HPOSP3",		0xD003 },			
	{ "P0PF",		0xD004 },			
	{ "HPOSM0",		0xD004 },			
	{ "P1PF",		0xD005 },			
	{ "HPOSM1",		0xD005 },			
	{ "P2PF",		0xD006 },			
	{ "HPOSM2",		0xD006 },			
	{ "P3PF",		0xD007 },			
	{ "HPOSM3",		0xD007 },			
	{ "M0PL",		0xD008 },			
	{ "SIZEP0",		0xD008 },			
	{ "M1PL",		0xD009 },			
	{ "SIZEP1",		0xD009 },			
	{ "M2PL",		0xD00A },			
	{ "HPOSP2",		0xD00A },			
	{ "M3PL",		0xD00B },			
	{ "HPOSP3",		0xD00B },			
	{ "P0PL",		0xD00C },			
	{ "SIZEM",		0xD00C },			
	{ "P1PL",		0xD00D },			
	{ "GRAFP0",		0xD00D },			
	{ "P2PL",		0xD00E },			
	{ "GRAFP1",		0xD00E },			
	{ "P3PL",		0xD00F },			
	{ "GRAFP2",		0xD00F },			
	{ "TRIG0",		0xD010 },			
	{ "GRAFP3",		0xD010 },			
	{ "TRIG1",		0xD011 },			
	{ "GRAFM",		0xD011 },			
	{ "TRIG2",		0xD012 },			
	{ "COLPM0",		0xD012 },			
	{ "TRIG3",		0xD013 },			
	{ "COLPM1",		0xD013 },			
	{ "PAL",			0xD014 },			
	{ "COLPM2",		0xD014 },			
	{ "COLPM3",		0xD015 },			
	{ "COLPF0",		0xD016 },			
	{ "COLPF1",		0xD017 },			
	{ "COLPF2",		0xD018 },			
	{ "COLPF3",		0xD019 },			
	{ "COLBK",		0xD01A },			
	{ "PRIOR",		0xD01B },			
	{ "VDELAY",		0xD01C },			
	{ "GRACTL",		0xD01D },			
	{ "HITCLR",		0xD01E },			
	{ "CONSOL",		0xD01F },			
	
	{ "POT0",		0xD200 },			
	{ "AUDF1",		0xD200 },			
	{ "POT1",		0xD201 },			
	{ "AUDC1",		0xD201 },			
	{ "POT2",		0xD202 },			
	{ "AUDF2",		0xD202 },			
	{ "POT3",		0xD203 },			
	{ "AUDC2",		0xD203 },			
	{ "POT4",		0xD204 },			
	{ "AUDF3",		0xD204 },			
	{ "POT5",		0xD205 },			
	{ "AUDC3",		0xD205 },			
	{ "POT6",		0xD206 },			
	{ "AUDF4",		0xD206 },			
	{ "POT7",		0xD207 },			
	{ "AUDC4",		0xD207 },			
	{ "ALLPOT",		0xD208 },			
	{ "AUDCTL",		0xD208 },			
	{ "KBCODE",		0xD209 },			
	{ "STIMER",		0xD209 },			
	{ "RANDOM",		0xD20A },			
	{ "SKREST",		0xD20A },			
	{ "POTGO",		0xD20B },			
	{ "SERIN",		0xD20D },			
	{ "SEROUT",		0xD20D },			
	{ "IRQST",		0xD20E },			
	{ "IRQEN",		0xD20E },			
	{ "SKSTAT",		0xD20F },			
	{ "SKCTL",		0xD20F },			
	
	{ "PORTA",		0xD300 },			
	{ "PORTB",		0xD301 },			
	{ "PACTL",		0xD302 },			
	{ "PBCTL",		0xD303 },			
	
	{ "DMACTL",		0xD400 },			
	{ "CHACTL",		0xD401 },			
	{ "DLISTL",		0xD402 },			
	{ "DLISTH",		0xD403 },			
	{ "HSCROL",		0xD404 },			
	{ "VSCROL",		0xD405 },			
	{ "PMBASE",		0xD407 },			
	{ "CHBASE",		0xD409 },			
	{ "WSYNC",		0xD40A },			
	{ "VCOUNT",		0xD40B },			
	{ "PENH",		0xD40C },			
	{ "PENL",		0xD40D },			
	{ "NMIEN",		0xD40E },			
	{ "NMIST",		0xD40F },			
	{ "NMIRES",		0xD40F },			
	
	{ "AFP",			0xD800 },			
	{ "FASC",		0xD8E6 },			
	{ "IFP",			0xD9AA },			
	{ "FPI",			0xD9D2 },			
	{ "ZPRO",		0xDA44 },			
	{ "ZF1",			0xDA46 },			
	{ "FSUB",		0xDA60 },			
	{ "FADD",		0xDA66 },			
	{ "FMUL",		0xDADB },			
	{ "FDIV",		0xDB28 },			
	{ "PLYEVL",		0xDD40 },			
	{ "FLD0R",		0xDD89 },			
	{ "FLD0R",		0xDD8D },			
	{ "FLD1R",		0xDD98 },			
	{ "FLD1P",		0xDD9C },			
	{ "FST0R",		0xDDA7 },			
	{ "FST0P",		0xDDAB },			
	{ "FMOVE",		0xDDB6 },			
	{ "EXP",			0xDDC0 },			
	{ "EXP10",		0xDDCC },			
	{ "LOG",			0xDECD },			
	{ "LOG10",		0xDED1 },			
	
	{ "DSKINV",		0xE453 },			
	{ "CIOV",		0xE456 },			
	{ "SIOV",		0xE459 },			
	{ "SETVBV",		0xE45C },			
	{ "SYSVBV",		0xE45F },			
	{ "XITVBV",		0xE462 },			
	{ "SIOINV",		0xE465 },			
	{ "SENDEV",		0xE468 },			
	{ "INTINV",		0xE46B },			
	{ "CIOINV",		0xE46E },			
	{ "SELFSV",		0xE471 },			
	{ "WARMSV",		0xE474 },			
	{ "COLDSV",		0xE477 },			
	{ "RBLOKV",		0xE47A },			
	{ "CSOPIV",		0xE47D },			
	{ "PUPDIV",		0xE480 },			
	{ "SELFTSV",	0xE483 },			
	{ "PENTV",		0xE486 },			
	{ "PHUNLV",		0xE489 },			
	{ "PHINIV",		0xE48C },			
	{ "GPDVV",		0xE48F }
};

int symtable_size=sizeof(symtable)/sizeof(symtable_rec);


void OutputBlockDiss( BYTE* pbtBlock, WORD wOffset, WORD wEnd )
{
	WORD wPC_curr = 0;
	WORD wPC_end = wEnd - wOffset;
	WORD wInsLen = 0;
	WORD wInsCode = 0;
	BOOL bInsIncomplete = FALSE;

	while ( wPC_curr <= wPC_end )
	{
		wInsCode = pbtBlock[ wPC_curr ];
		wInsLen = instable[ wInsCode ].length;

		printf("%04X  ", wPC_curr + wOffset );

		if ( wPC_curr + wInsLen > wPC_end )
		{
			wInsLen = wPC_end - wPC_curr;
			bInsIncomplete = TRUE;
		}

		for ( int i = 0; i <= wInsLen; i++ )
		{
			printf("%02X ", pbtBlock[ wPC_curr + i ] );
		}

		for ( int i = 0; i < ( 3 - wInsLen ); i++ )
		{
			printf("   ");
		}

		char obuf[256];
		char *p;

		char* szSymbol = NULL;

		strcpy( obuf, instable[ wInsCode ].instruct );

		for (p = obuf; *p; p++)
		{
			if (*p == '*')
			{
				*p = '\0';
				p++;
				break;
			}
		}

		if ( !instable[ pbtBlock[ wPC_curr ] ].length )
		{
			printf( "%s\n", instable[pbtBlock[wPC_curr]].instruct );
		}
		else
		{
			if ( bInsIncomplete )
			{
				printf( "!!!\n" );
				break;
			}

	  		printf( "%s",obuf );

	  		if ( instable[wInsCode].branch )
			{
		  		int uPdata = (int) (signed char)pbtBlock[ wPC_curr + 1 ];
		  		printf( "%04X", wPC_curr + 2 + uPdata + wOffset );
	  		}
			else
			{
		  		if ( instable[wInsCode].length == 1 )
				{
					WORD wAddr = (BYTE)pbtBlock[ wPC_curr + 1 ];

					if ( !instable[ wInsCode ].immed )
						szSymbol = SymbolFind( wAddr, instable[ wInsCode ].write ? TRUE : FALSE );

					printf( "$%02X", wAddr );

		  		}
				else if ( instable[wInsCode].length == 2 )
				{
					WORD wAddr = (WORD) pbtBlock[ wPC_curr + 1 ] |
				 		(WORD)( pbtBlock[ wPC_curr + 2 ] << 8 );

					if ( !instable[ wInsCode ].immed )
						szSymbol = SymbolFind( wAddr, instable[ wInsCode ].write ? TRUE : FALSE );

					printf( "$%04X", wAddr );

		  		}
	  		}

			if ( szSymbol )
	  			printf("%s\t;%s\n", p, szSymbol );
			else
	  			printf("%s\n",p);
		}

		wPC_curr += ( wInsLen + 1 );
	}

}

char* SymbolFind( WORD addr, BOOL bWrite )
{
	int lo = 0;
	int mi = 0;
	int hi = symtable_size - 1;

	while ( lo < hi )
	{
		mi = ( lo + hi ) / 2;

		if ( symtable[ mi ].addr == addr )
		{
			break;
		}
		else
		{
			if ( symtable[ mi ].addr > addr )
				hi=mi;
			else
				lo=mi+1;
		}
	}

	if ( symtable[mi].addr == addr )
	{
		if ( mi>0 && symtable[mi-1].addr == addr )
		{
			mi--;
		}

		if ( bWrite )
		{
			if ( mi < symtable_size - 1 )
				if ( symtable[ mi + 1 ].addr == addr )
					mi++;
		}

	 	return symtable[ mi ].name;
	}

	else
		return NULL;
}



