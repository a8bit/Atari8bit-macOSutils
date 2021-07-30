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
#include "CDis.h"

typedef struct
{
	ADDR_TYPES	type;
	MEMORY_ACCESS	write;
	BOOL undoc;
} INSTRUCTION;

char* mnem_table_gen[] =
{
"BRK", //00
"ORA", //01
"CIM", //02	
"ASO", //03

"SKB", //04
"ORA", //05
"ASL", //06
"ASO", //07

"PHP", //08	
"ORA", //09
"ASL", //0A	
"ASO", //0B	

"SKW", //0C	
"ORA", //0D
"ASL", //0E
"ASO", //0F

"BPL", //10
"ORA", //11
"CIM", //12	
"ASO", //13

"SKB", //14
"ORA", //15
"ASL", //16
"ASO", //17	

"CLC", //18	
"ORA", //19
"NOP", //1A
"ASO", //1B

"SKW", //1C	
"ORA", //1D
"ASL", //1E
"ASO", //1F

"JSR", //20
"AND", //21
"CIM", //22	
"RLA", //23

"BIT", //24
"AND", //25
"ROL", //26
"RLA", //27	

"PLP", //28	
"AND", //29
"ROL", //2A	
"RLA", //2B

"BIT", //2C
"AND", //2D
"ROL", //2E
"RLA", //2F

"BMI", //30
"AND", //31
"CIM", //32	
"RLA", //33

"SKB", //34
"AND", //35
"ROL", //36
"RLA", //37

"SEC", //38	
"AND", //39
"NOP", //3A
"RLA", //3B

"SKW", //3C	
"AND", //3D
"ROL", //3E
"RLA", //3F

"RTI", //40	
"EOR", //41
"CIM", //42	
"LSE", //43

"SKB", //44
"EOR", //45
"LSR", //46
"LSE", //47	

"PHA", //48	
"EOR", //49
"LSR", //4A	
"ALR", //4B

"JMP", //4C
"EOR", //4D
"LSR", //4E
"LSE", //4F

"BVC", //50
"EOR", //51
"CIM", //52	
"LSE", //53	

"SKB", //54
"EOR", //55
"LSR", //56
"LSE", //57

"CLI", //58	
"EOR", //59
"NOP", //5A
"LSE", //5B	

"SKW", //5C	
"EOR", //5D
"LSR", //5E
"LSE", //5F

"RTS", //60	
"ADC", //61
"CIM", //62	
"RRA", //63	

"SKB", //64
"ADC", //65
"ROR", //66
"RRA", //67	

"PLA", //68	
"ADC", //69
"ROR", //6A	
"AAR", //6B	

"JMP", //6C
"ADC", //6D
"ROR", //6E
"RRA", //6F	

"BVS", //70
"ADC", //71
"CIM", //72	
"RRA", //73

"SKB", //74
"ADC", //75
"ROR", //76
"RRA", //77

"SEI", //78	
"ADC", //79
"NOP", //7A
"RRA", //7B	

"SKW", //7C	
"ADC", //7D
"ROR", //7E
"RRA", //7F	

"SKB", //80
"STA", //81
"SKB", //82
"AXS", //83	

"STY", //84
"STA", //85
"STX", //86
"AXS", //87	

"DEY", //88	
"SKB", //89
"TXA", //8A	
"XAA", //8B	

"STY", //8C
"STA", //8D
"STX", //8E
"AXS", //8F	

"BCC", //90
"STA", //91
"CIM", //92	
"AXS", //93

"STY", //94
"STA", //95
"STX", //96
"AXS", //97	

"TYA", //98	
"STA", //99
"TXS", //9A	
"XAA", //9B

"SKW", //9C	
"STA", //9D
"MKX", //9E	
"MKA", //9F	

"LDY", //A0
"LDA", //A1
"LDX", //A2
"LAX", //A3	

"LDY", //A4
"LDA", //A5
"LDX", //A6
"LAX", //A7

"TAY", //A8	
"LDA", //A9
"TAX", //AA	
"OAL", //AB	

"LDY", //AC
"LDA", //AD
"LDX", //AE
"LAX", //AF	

"BCS", //B0
"LDA", //B1
"CIM", //B2	
"LAX", //B3	

"LDY", //B4
"LDA", //B5
"LDX", //B6
"LAX", //B7	

"CLV", //B8	
"LDA", //B9
"TSX", //BA	
"AXA", //BB

"LDY", //BC
"LDA", //BD
"LDX", //BE
"LAX", //BF

"CPY", //C0
"CMP", //C1
"SKB", //C2
"DCM", //C3	

"CPY", //C4
"CMP", //C5
"DEC", //C6
"DCM", //C7	

"INY", //C8	
"CMP", //C9
"DEX", //CA	
"SAX", //CB	

"CPY", //CC
"CMP", //CD
"DEC", //CE
"DCM", //CF	

"BNE", //D0
"CMP", //D1
"CIM", //D2	
"DCM", //D3

"SKB", //D4
"CMP", //D5
"DEC", //D6
"DCM", //D7	

"CLD", //D8	
"CMP", //D9
"NOP", //DA
"DCM", //DB	

"SKW", //DC	
"CMP", //DD
"DEC", //DE
"DCM", //DF

"CPX", //E0
"SBC", //E1
"SKB", //E2
"INS", //E3	

"CPX", //E4
"SBC", //E5
"INC", //E6
"INS", //E7	

"INX", //E8	
"SBC", //E9
"NOP", //EA
"SBC", //EB

"CPX", //EC
"SBC", //ED
"INC", //EE
"INS", //EF	

"BEQ", //F0
"SBC", //F1
"CIM", //F2	
"INS", //F3	

"SKB", //F4
"SBC", //F5
"INC", //F6
"INS", //F7

"SED", //F8	
"SBC", //F9
"NOP", //FA
"INS", //FB

"SKW", //FC	
"SBC", //FD
"INC", //FE
"INS", //FF
NULL
};

char* mnem_table_dasm[] =
{
"BRK", //00
"ORA", //01
"JAM", //02	
"SLO", //03

"NOOP", //04
"ORA", //05
"ASL", //06
"SLO", //07

"PHP", //08	
"ORA", //09
"ASL", //0A	
"SLO", //0B	

"NOOP", //0C	
"ORA", //0D
"ASL", //0E
"SLO", //0F

"BPL", //10
"ORA", //11
"JAM", //12	
"SLO", //13

"NOOP", //14
"ORA", //15
"ASL", //16
"SLO", //17	

"CLC", //18	
"ORA", //19
"NOP", //1A
"SLO", //1B

"NOOP", //1C	
"ORA", //1D
"ASL", //1E
"SLO", //1F

"JSR", //20
"AND", //21
"JAM", //22	
"RLA", //23

"BIT", //24
"AND", //25
"ROL", //26
"RLA", //27	

"PLP", //28	
"AND", //29
"ROL", //2A	
"RLA", //2B

"BIT", //2C
"AND", //2D
"ROL", //2E
"RLA", //2F

"BMI", //30
"AND", //31
"JAM", //32	
"RLA", //33

"NOOP", //34
"AND", //35
"ROL", //36
"RLA", //37

"SEC", //38	
"AND", //39
"NOP", //3A
"RLA", //3B

"NOOP", //3C	
"AND", //3D
"ROL", //3E
"RLA", //3F

"RTI", //40	
"EOR", //41
"JAM", //42	
"SRE", //43

"NOOP", //44
"EOR", //45
"LSR", //46
"SRE", //47	

"PHA", //48	
"EOR", //49
"LSR", //4A	
"ASR", //4B

"JMP", //4C
"EOR", //4D
"LSR", //4E
"SRE", //4F

"BVC", //50
"EOR", //51
"JAM", //52	
"SRE", //53	

"NOOP", //54
"EOR", //55
"LSR", //56
"SRE", //57

"CLI", //58	
"EOR", //59
"NOP", //5A
"SRE", //5B	

"NOOP", //5C	
"EOR", //5D
"LSR", //5E
"SRE", //5F

"RTS", //60	
"ADC", //61
"JAM", //62	
"RRA", //63	

"NOOP", //64
"ADC", //65
"ROR", //66
"RRA", //67	

"PLA", //68	
"ADC", //69
"ROR", //6A	
"ARR", //6B	

"JMP", //6C
"ADC", //6D
"ROR", //6E
"RRA", //6F	

"BVS", //70
"ADC", //71
"JAM", //72	
"RRA", //73

"NOOP", //74
"ADC", //75
"ROR", //76
"RRA", //77

"SEI", //78	
"ADC", //79
"NOP", //7A
"RRA", //7B	

"NOOP", //7C	
"ADC", //7D
"ROR", //7E
"RRA", //7F	

"NOOP", //80
"STA", //81
"NOOP", //82
"SAX", //83	

"STY", //84
"STA", //85
"STX", //86
"SAX", //87	

"DEY", //88	
"NOOP", //89
"TXA", //8A	
"ANE", //8B	

"STY", //8C
"STA", //8D
"STX", //8E
"SAX", //8F	

"BCC", //90
"STA", //91
"JAM", //92	
"SAX", //93

"STY", //94
"STA", //95
"STX", //96
"SAX", //97	

"TYA", //98	
"STA", //99
"TXS", //9A	
"SHS", //9B

"SHY", //9C	
"STA", //9D
"SHX", //9E	
"SHA", //9F	

"LDY", //A0
"LDA", //A1
"LDX", //A2
"LAX", //A3	

"LDY", //A4
"LDA", //A5
"LDX", //A6
"LAX", //A7

"TAY", //A8	
"LDA", //A9
"TAX", //AA	
"LXA", //AB	

"LDY", //AC
"LDA", //AD
"LDX", //AE
"LAX", //AF	

"BCS", //B0
"LDA", //B1
"JAM", //B2	
"LAX", //B3	

"LDY", //B4
"LDA", //B5
"LDX", //B6
"LAX", //B7	

"CLV", //B8	
"LDA", //B9
"TSX", //BA	
"LAS", //BB

"LDY", //BC
"LDA", //BD
"LDX", //BE
"LAX", //BF

"CPY", //C0
"CMP", //C1
"NOOP", //C2
"DCP", //C3	

"CPY", //C4
"CMP", //C5
"DEC", //C6
"DCP", //C7	

"INY", //C8	
"CMP", //C9
"DEX", //CA	
"SBX", //CB	

"CPY", //CC
"CMP", //CD
"DEC", //CE
"DCP", //CF	

"BNE", //D0
"CMP", //D1
"JAM", //D2	
"DCP", //D3

"NOOP", //D4
"CMP", //D5
"DEC", //D6
"DCP", //D7	

"CLD", //D8	
"CMP", //D9
"NOP", //DA
"DCP", //DB	

"NOOP", //DC	
"CMP", //DD
"DEC", //DE
"DCP", //DF

"CPX", //E0
"SBC", //E1
"NOOP", //E2
"ISB", //E3	

"CPX", //E4
"SBC", //E5
"INC", //E6
"ISB", //E7	

"INX", //E8	
"SBC", //E9
"NOP", //EA
"USBC", //EB

"CPX", //EC
"SBC", //ED
"INC", //EE
"ISB", //EF	

"BEQ", //F0
"SBC", //F1
"JAM", //F2	
"ISB", //F3	

"NOOP", //F4
"SBC", //F5
"INC", //F6
"ISB", //F7

"SED", //F8	
"SBC", //F9
"NOP", //FA
"ISB", //FB

"NOOP", //FC	
"SBC", //FD
"INC", //FE
"ISB", //FF
NULL
};

INSTRUCTION instable[] =
{
	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//00 BRK
	{ ADDR_INDIRECTX,		ACCESS_READ, FALSE },	//01 ORA
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//02 CIM
	{ ADDR_INDIRECTX,		ACCESS_READ, TRUE  },	//03 ASO

	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//04 SKB
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//05 ORA
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//06 ASL
	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//07 ASO

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//08 PHP
	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//09 ORA
	{ ADDR_ACCUMULATOR,	ACCESS_READ, FALSE },	//0A ASL
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//0B ASO

	{ ADDR_ABSOLUTE,		ACCESS_READ, TRUE  },	//0C SKW
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//0D ORA
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//0E ASL
	{ ADDR_ABSOLUTE,		ACCESS_READ, TRUE  },	//0F ASO

	{ ADDR_BRANCH,			ACCESS_READ, FALSE },	//10 BPL
	{ ADDR_INDIRECTY,		ACCESS_READ, FALSE },	//11 ORA
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//12 CIM
	{ ADDR_INDIRECTY,		ACCESS_READ, TRUE  },	//13 ASO

	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//14 SKB
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//15 ORA
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//16 ASL
	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//17 ASO

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//18 CLC
	{ ADDR_ABSOLUTEY,		ACCESS_READ, FALSE },	//19 ORA
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//1A NOP
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//1B ASO

	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//1C SKW
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//1D ORA
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//1E ASL
	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//1F ASO

	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//20 JSR
	{ ADDR_INDIRECTX,		ACCESS_READ, FALSE },	//21 AND
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//22 CIM
	{ ADDR_INDIRECTX,		ACCESS_READ, TRUE  },	//23 RLA

	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//24 BIT
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//25 AND
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//26 ROL
	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//27 RLA

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//28 PLP
	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//29 AND
	{ ADDR_ACCUMULATOR,	ACCESS_READ, FALSE },	//2A ROL
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//2B RLA

	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//2C BIT
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//2D AND
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//2E ROL
	{ ADDR_ABSOLUTE,		ACCESS_READ, TRUE  },	//2F RLA

	{ ADDR_BRANCH,			ACCESS_READ, FALSE },	//30 BMI
	{ ADDR_INDIRECTY,		ACCESS_READ, FALSE },	//31 AND
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//32 CIM
	{ ADDR_INDIRECTY,		ACCESS_READ, TRUE  },	//33 RLA

	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//34 SKB
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//35 AND
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//36 ROL
	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//37 RLA

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//38 SEC
	{ ADDR_ABSOLUTEY,		ACCESS_READ, FALSE },	//39 AND
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//3A NOP
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//3B RLA

	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//3C SKW
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//3D AND
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//3E ROL
	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//3F RLA

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//40 RTI
	{ ADDR_INDIRECTX,		ACCESS_READ, FALSE },	//41 EOR
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//42 CIM
	{ ADDR_INDIRECTX,		ACCESS_READ, TRUE  },	//43 LSE

	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//44 SKB
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//45 EOR
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//46 LSR
	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//47 LSE

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//48 PHA
	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//49 EOR
	{ ADDR_ACCUMULATOR,	ACCESS_READ, FALSE },	//4A LSR
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//4B ALR

	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//4C JMP
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//4D EOR
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//4E LSR
	{ ADDR_ABSOLUTE,		ACCESS_READ, TRUE  },	//4F LSE

	{ ADDR_BRANCH,			ACCESS_READ, FALSE },	//50 BVC
	{ ADDR_INDIRECTY,		ACCESS_READ, FALSE },	//51 EOR
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//52 CIM
	{ ADDR_INDIRECTY,		ACCESS_READ, TRUE  },	//53 LSE

	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//54 SKB
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//55 EOR
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//56 LSR
	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//57 LSE

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//58 CLI
	{ ADDR_ABSOLUTEY,		ACCESS_READ, FALSE },	//59 EOR
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//5A NOP
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//5B LSE

	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//5C SKW
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//5D EOR
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//5E LSR
	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//5F LSE

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//60 RTS
	{ ADDR_INDIRECTX,		ACCESS_READ, FALSE },	//61 ADC
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//62 CIM
	{ ADDR_INDIRECTX,		ACCESS_READ, TRUE  },	//63 RRA

	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//64 SKB
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//65 ADC
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//66 ROR
	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//67 RRA

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//68 PLA
	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//69 ADC
	{ ADDR_ACCUMULATOR,	ACCESS_READ, FALSE },	//6A ROR
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//6B AAR

	{ ADDR_INDIRECT,		ACCESS_READ, FALSE },	//6C JMP
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//6D ADC
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//6E ROR
	{ ADDR_ABSOLUTE,		ACCESS_READ, TRUE  },	//6F RRA

	{ ADDR_BRANCH,			ACCESS_READ, FALSE },	//70 BVS
	{ ADDR_INDIRECTY,		ACCESS_READ, FALSE },	//71 ADC
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//72 CIM
	{ ADDR_INDIRECTY,		ACCESS_READ, TRUE  },	//73 RRA

	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//74 SKB
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//75 ADC
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//76 ROR
	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//77 RRA

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//78 SEI
	{ ADDR_ABSOLUTEY,		ACCESS_READ, FALSE },	//79 ADC
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//7A NOP
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//7B RRA

	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//7C SKW
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//7D ADC
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//7E ROR
	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//7F RRA

	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//80 SKB
	{ ADDR_INDIRECTX,		ACCESS_WRIT, FALSE },	//81 STA
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//82 SKB
	{ ADDR_INDIRECTX,		ACCESS_READ, TRUE  },	//83 AXS

	{ ADDR_ZPAGE,			ACCESS_WRIT, FALSE },	//84 STY
	{ ADDR_ZPAGE,			ACCESS_WRIT, FALSE },	//85 STA
	{ ADDR_ZPAGE,			ACCESS_WRIT, FALSE },	//86 STX
	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//87 AXS

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//88 DEY
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//89 SKB
	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//8A TXA
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//8B XAA

	{ ADDR_ABSOLUTE,		ACCESS_WRIT, FALSE },	//8C STY
	{ ADDR_ABSOLUTE,		ACCESS_WRIT, FALSE },	//8D STA
	{ ADDR_ABSOLUTE,		ACCESS_WRIT, FALSE },	//8E STX
	{ ADDR_ABSOLUTE,		ACCESS_READ, TRUE  },	//8F AXS

	{ ADDR_BRANCH,			ACCESS_READ, FALSE },	//90 BCC
	{ ADDR_INDIRECTY,		ACCESS_WRIT, FALSE },	//91 STA
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//92 CIM
	{ ADDR_INDIRECTY,		ACCESS_READ, TRUE  },	//93 AXS

	{ ADDR_ZPAGEX,			ACCESS_WRIT, FALSE },	//94 STY
	{ ADDR_ZPAGEX,			ACCESS_WRIT, FALSE },	//95 STA
	{ ADDR_ZPAGEY,			ACCESS_WRIT, FALSE },	//96 STX
	{ ADDR_ZPAGEY,			ACCESS_READ, TRUE  },	//97 AXS

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//98 TYA
	{ ADDR_ABSOLUTEY,		ACCESS_WRIT, FALSE },	//99 STA
	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//9A TXS
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//9B XAA

	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//9C SKW
	{ ADDR_ABSOLUTEX,		ACCESS_WRIT, FALSE },	//9D STA
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//9E MKX
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//9F MKA

	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//A0 LDY
	{ ADDR_INDIRECTX,		ACCESS_READ, FALSE },	//A1 LDA
	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//A2 LDX
	{ ADDR_INDIRECTX,		ACCESS_READ, TRUE  },	//A3 LAX

	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//A4 LDY
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//A5 LDA
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//A6 LDX
	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//A7 LAX

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//A8 TAY
	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//A9 LDA
	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//AA TAX
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//AB OAL

	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//AC LDY
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//AD LDA
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//AE LDX
	{ ADDR_ABSOLUTE,		ACCESS_READ, TRUE  },	//AF LAX

	{ ADDR_BRANCH,			ACCESS_READ, FALSE },	//B0 BCS
	{ ADDR_INDIRECTY,		ACCESS_READ, FALSE },	//B1 LDA
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//B2 CIM
	{ ADDR_INDIRECTY,		ACCESS_READ, TRUE  },	//B3 LAX

	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//B4 LDY
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//B5 LDA
	{ ADDR_ZPAGEY,			ACCESS_READ, FALSE },	//B6 LDX
	{ ADDR_ZPAGEY,			ACCESS_READ, TRUE  },	//B7 LAX

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//B8 CLV
	{ ADDR_ABSOLUTEY,		ACCESS_READ, FALSE },	//B9 LDA
	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//BA TSX
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//BB AXA

	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//BC LDY
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//BD LDA
	{ ADDR_ABSOLUTEY,		ACCESS_READ, FALSE },	//BE LDX
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//BF LAX

	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//C0 CPY
	{ ADDR_INDIRECTX,		ACCESS_READ, FALSE },	//C1 CMP
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//C2 SKB
	{ ADDR_INDIRECTX,		ACCESS_READ, TRUE  },	//C3 DCM

	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//C4 CPY
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//C5 CMP
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//C6 DEC
	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//C7 DCM

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//C8 INY
	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//C9 CMP
	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//CA DEX
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//CB SAX

	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//CC CPY
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//CD CMP
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//CE DEC
	{ ADDR_ABSOLUTE,		ACCESS_READ, TRUE  },	//CF DCM

	{ ADDR_BRANCH,			ACCESS_READ, FALSE },	//D0 BNE
	{ ADDR_INDIRECTY,		ACCESS_READ, FALSE },	//D1 CMP
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//D2 CIM
	{ ADDR_INDIRECTY,		ACCESS_READ, TRUE  },	//D3 DCM

	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//D4 SKB
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//D5 CMP
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//D6 DEC
	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//D7 DCM

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//D8 CLD
	{ ADDR_ABSOLUTEY,		ACCESS_READ, FALSE },	//D9 CMP
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//DA NOP
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//DB DCM

	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//DC SKW
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//DD CMP
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//DE DEC
	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//DF DCM

	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//E0 CPX
	{ ADDR_INDIRECTX,		ACCESS_READ, FALSE },	//E1 SBC
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//E2 SKB
	{ ADDR_INDIRECTX,		ACCESS_READ, TRUE  },	//E3 INS

	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//E4 CPX
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//E5 SBC
	{ ADDR_ZPAGE,			ACCESS_READ, FALSE },	//E6 INC
	{ ADDR_ZPAGE,			ACCESS_READ, TRUE  },	//E7 INS

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//E8 INX
	{ ADDR_IMMEDIATE,		ACCESS_READ, FALSE },	//E9 SBC
	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//EA NOP
	{ ADDR_IMMEDIATE,		ACCESS_READ, TRUE  },	//EB SBC

	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//EC CPX
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//ED SBC
	{ ADDR_ABSOLUTE,		ACCESS_READ, FALSE },	//EE INC
	{ ADDR_ABSOLUTE,		ACCESS_READ, TRUE  },	//EF INS

	{ ADDR_BRANCH,			ACCESS_READ, FALSE },	//F0 BEQ
	{ ADDR_INDIRECTY,		ACCESS_READ, FALSE },	//F1 SBC
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//F2 CIM
	{ ADDR_INDIRECTY,		ACCESS_READ, TRUE  },	//F3 INS

	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//F4 SKB
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//F5 SBC
	{ ADDR_ZPAGEX,			ACCESS_READ, FALSE },	//F6 INC
	{ ADDR_ZPAGEX,			ACCESS_READ, TRUE  },	//F7 INS

	{ ADDR_IMPLIED,		ACCESS_READ, FALSE },	//F8 SED
	{ ADDR_ABSOLUTEY,		ACCESS_READ, FALSE },	//F9 SBC
	{ ADDR_IMPLIED,		ACCESS_READ, TRUE  },	//FA NOP
	{ ADDR_ABSOLUTEY,		ACCESS_READ, TRUE  },	//FB INS

	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//FC SKW
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//FD SBC
	{ ADDR_ABSOLUTEX,		ACCESS_READ, FALSE },	//FE INC
	{ ADDR_ABSOLUTEX,		ACCESS_READ, TRUE  },	//FF INS

	//{ NULL,	ADDR_IMPLIED,		ACCESS_READ, FALSE }
};

typedef struct
{
	char* m_szNameR;
	char* m_szNameW;
	WORD m_wAddr;
} symtable_rec;

typedef struct
{
	DWORD m_dwClass;
	symtable_rec* m_symtable;
	WORD m_wCnt;
	WORD m_wFirst;
	WORD m_wLast;
	BOOL m_bAllowed;
} supersymtable_rec;

#ifdef __A8_SYMBOLS__
symtable_rec symtable_a8zpage_lo[] = 
{ 
	{ "NGFLAG",		NULL,			0x0001 },
	{ "CASINI",		NULL,			0x0002 },			
	{ "CASINI+1",	NULL,			0x0003 },			
	{ "RAMLO",		NULL,			0x0004 },			
	{ "RAMLO+1",	NULL,			0x0005 },			
	{ "TRAMSZ",		NULL,			0x0006 },			
	{ "CMCMD",		NULL,			0x0007 },			
	{ "WARMST",		NULL,			0x0008 },			
	{ "BOOT",		NULL,			0x0009 },			
	{ "DOSVEC",		NULL,			0x000A },			
	{ "DOSVEC+1",	NULL,			0x000B },			
	{ "DOSINI",		NULL,			0x000C },			
	{ "DOSINI+1",	NULL,			0x000D },			
	{ "APPMHI",		NULL,			0x000E },			
	{ "APPMHI+1",	NULL,			0x000F },			
	{ "POKMSK",		NULL,			0x0010 },			
	{ "BRKKEY",		NULL,			0x0011 },			
	{ "RTCLOK",		NULL,			0x0012 },			
	{ "RTCLOK+1",	NULL,			0x0013 },			
	{ "RTCLOK+2",	NULL,			0x0014 },			
	{ "BUFADR",		NULL,			0x0015 },			
	{ "BUFADR+1",	NULL,			0x0016 },			
	{ "ICCOMT",		NULL,			0x0017 },			
	{ "DSKFMS",		NULL,			0x0018 },			
	{ "DSKFMS+1",	NULL,			0x0019 },			
	{ "DSKUTL",		NULL,			0x001A },			
	{ "DSKUTL+1",	NULL,			0x001B },			
	{ "ABUFPT",		NULL,			0x001C },			
	{ "ABUFPT+1",	NULL,			0x001D },			
	{ "ABUFPT+2",	NULL,			0x001E },			
	{ "ABUFPT+3",	NULL,			0x001F },			
	{ "ICHIDZ",		NULL,			0x0020 },			
	{ "ICDNOZ",		NULL,			0x0021 },			
	{ "ICCOMZ",		NULL,			0x0022 },			
	{ "ICSTAZ",		NULL,			0x0023 },			
	{ "ICBALZ",		NULL,			0x0024 },			
	{ "ICBAHZ",		NULL,			0x0025 },			
	{ "ICPTLZ",		NULL,			0x0026 },			
	{ "ICPTHZ",		NULL,			0x0027 },			
	{ "ICBLLZ",		NULL,			0x0028 },			
	{ "ICBLHZ",		NULL,			0x0029 },			
	{ "ICAX1Z",		NULL,			0x002A },			
	{ "ICAX2Z",		NULL,			0x002B },			
	{ "ICAX3Z",		NULL,			0x002C },			
	{ "ICAX4Z",		NULL,			0x002D },			
	{ "ICAX5Z",		NULL,			0x002E },			
	{ "ICAX6Z",		NULL,			0x002F },			
	{ "STATUS",		NULL,			0x0030 },			
	{ "CHKSUM",		NULL,			0x0031 },			
	{ "BUFRLO",		NULL,			0x0032 },			
	{ "BUFRHI",		NULL,			0x0033 },			
	{ "BFENLO",		NULL,			0x0034 },			
	{ "BFENHI",		NULL,			0x0035 },			
	{ "LTEMP",		NULL,			0x0036 },			
	{ "LTEMP+1",	NULL,			0x0037 },			
	{ "BUFRFL",		NULL,			0x0038 },			
	{ "RECVDN",		NULL,			0x0039 },			
	{ "XMTDON",		NULL,			0x003A },			
	{ "CHKSNT",		NULL,			0x003B },			
	{ "NOCKSM",		NULL,			0x003C },			
	{ "BPTR",		NULL,			0x003D },			
	{ "FTYPE",		NULL,			0x003E },			
	{ "FEOF",		NULL,			0x003F },			
	{ "FREQ",		NULL,			0x0040 },			
	{ "SOUNDR",		NULL,			0x0041 },			
	{ "CRITIC",		NULL,			0x0042 },			
	{ "FMSZPG",		NULL,			0x0043 },			
	{ "FMSZPG+1",	NULL,			0x0044 },			
	{ "FMSZPG+2",	NULL,			0x0045 },			
	{ "FMSZPG+3",	NULL,			0x0046 },			
	{ "FMSZPG+4",	NULL,			0x0047 },			
	{ "FMSZPG+5",	NULL,			0x0048 },			
	{ "FMSZPG+6",	NULL,			0x0049 },			
	{ "ZCHAIN",		NULL,			0x004A },			
	{ "ZCHAIN+1",	NULL,			0x004B },			
	{ "DSTAT",		NULL,			0x004C },			
	{ "ATRACT",		NULL,			0x004D },			
	{ "DRKMSK",		NULL,			0x004E },			
	{ "COLRSH",		NULL,			0x004F },			
	{ "TEMP",		NULL,			0x0050 },			
	{ "HOLD1",		NULL,			0x0051 },			
	{ "LMARGN",		NULL,			0x0052 },			
	{ "RMARGN",		NULL,			0x0053 },			
	{ "ROWCRS",		NULL,			0x0054 },			
	{ "COLCRS",		NULL,			0x0055 },			
	{ "COLCRS+1",	NULL,			0x0056 },			
	{ "DINDEX",		NULL,			0x0057 },			
	{ "SAVMSC",		NULL,			0x0058 },			
	{ "SAVMSC+1",	NULL,			0x0059 },			
	{ "OLDROW",		NULL,			0x005A },			
	{ "OLDCOL",		NULL,			0x005B },			
	{ "OLDCOL+1",	NULL,			0x005C },			
	{ "OLDCHR",		NULL,			0x005D },			
	{ "OLDADR",		NULL,			0x005E },			
	{ "OLDADR+1",	NULL,			0x005F },			
	{ "FKDEF",		NULL,			0x0060 },			
	{ "FKDEF+1",	NULL,			0x0061 },			
	{ "PALNTS",		NULL,			0x0062 },			
	{ "LOGCOL",		NULL,			0x0063 },			
	{ "ADRESS",		NULL,			0x0064 },			
	{ "ADRESS+1",	NULL,			0x0065 },			
	{ "MLTTMP",		NULL,			0x0066 },			
	{ "MLTTMP+1",	NULL,			0x0067 },			
	{ "SAVADR",		NULL,			0x0068 },			
	{ "SAVADR+1",	NULL,			0x0069 },			
	{ "RAMTOP",		NULL,			0x006A },			
	{ "BUFCNT",		NULL,			0x006B },			
	{ "BUFSTR",		NULL,			0x006C },			
	{ "BUFSTR+1",	NULL,			0x006D },			
	{ "BITMSK",		NULL,			0x006E },			
	{ "SHFAMT",		NULL,			0x006F },			
	{ "ROWAC",		NULL,			0x0070 },			
	{ "ROWAC+1",	NULL,			0x0071 },			
	{ "COLAC",		NULL,			0x0072 },			
	{ "COLAC+1",	NULL,			0x0073 },			
	{ "ENDPT",		NULL,			0x0074 },			
	{ "ENDPT+1",	NULL,			0x0075 },			
	{ "DELTAR",		NULL,			0x0076 },			
	{ "DELTAC",		NULL,			0x0077 },			
	{ "DELTAC+1",	NULL,			0x0078 },			
	{ "KEYDEF",		NULL,			0x0079 },			
	{ "KEYDEF+1",	NULL,			0x007A },			
	{ "SWPFLG",		NULL,			0x007B },			
	{ "HOLDCH",		NULL,			0x007C },			
	{ "INSDAT",		NULL,			0x007D },			
	{ "COUNTR",		NULL,			0x007E },			
	{ "COUNTR+1",	NULL,			0x007F }
};

symtable_rec symtable_a8zpage_hi[] =
{
	{ "LOMEM",		NULL,			0x0080 },			
	{ "LOMEM+1",	NULL,			0x0081 },			
	{ "VNTP",		NULL,			0x0082 },			
	{ "VNTP+1",		NULL,			0x0083 },			
	{ "VNTD",		NULL,			0x0084 },			
	{ "VNTD+1",		NULL,			0x0085 },			
	{ "VVTP",		NULL,			0x0086 },			
	{ "VVTP+1",		NULL,			0x0087 },			
	{ "STMTAB",		NULL,			0x0088 },			
	{ "STMTAB+1",	NULL,			0x0089 },			
	{ "STMCUR",		NULL,			0x008A },			
	{ "STMCUR+1",	NULL,			0x008B },			
	{ "STARP",		NULL,			0x008C },			
	{ "STARP+1",	NULL,			0x008D },			
	{ "RUNSTK",		NULL,			0x008E },			
	{ "RUNSTK+1",	NULL,			0x008F },			
	{ "TOPSTK",		NULL,			0x0090 },			
	{ "TOPSTK+1",	NULL,			0x0091 },			
	{ "MEOLFLG",	NULL,			0x0092 },			
	{ "POKADR",		NULL,			0x0095 },			
	{ "POKADR+1",	NULL,			0x0096 },			
	{ "DATAD",		NULL,			0x00B6 },			
	{ "DATALN",		NULL,			0x00B7 },			
	{ "DATALN+1",	NULL,			0x00B8 },			
	{ "STOPLN",		NULL,			0x00BA },			
	{ "STOPLN+1",	NULL,			0x00BB },			
	{ "SAVCUR",		NULL,			0x00BE },			
	{ "IOCMD",		NULL,			0x00C0 },			
	{ "IODVC",		NULL,			0x00C1 },			
	{ "PROMPT",		NULL,			0x00C2 },			
	{ "ERRSAVE",	NULL,			0x00C3 },			
	{ "COLOUR",		NULL,			0x00C8 },			
	{ "PTABW",		NULL,			0x00C9 },			
	{ "LOADFLG",	NULL,			0x00CA },			
	{ "FR0",			NULL,			0x00D4 },			
	{ "FR0+1",		NULL,			0x00D5 },			
	{ "FR0+2",		NULL,			0x00D6 },			
	{ "FR0+3",		NULL,			0x00D7 },			
	{ "FR0+4",		NULL,			0x00D8 },			
	{ "FR0+5",		NULL,			0x00D9 },			
	{ "FRE",			NULL,			0x00DA },			
	{ "FRE+1",		NULL,			0x00DB },			
	{ "FRE+2",		NULL,			0x00DC },			
	{ "FRE+3",		NULL,			0x00DD },			
	{ "FRE+4",		NULL,			0x00DE },			
	{ "FRE+5",		NULL,			0x00DF },			
	{ "FR1",			NULL,			0x00E0 },			
	{ "FR1+1",		NULL,			0x00E1 },			
	{ "FR1+2",		NULL,			0x00E2 },			
	{ "FR1+3",		NULL,			0x00E3 },			
	{ "FR1+4",		NULL,			0x00E4 },			
	{ "FR1+5",		NULL,			0x00E5 },			
	{ "FR2",			NULL,			0x00E6 },			
	{ "FR2+1",		NULL,			0x00E7 },			
	{ "FR2+2",		NULL,			0x00E8 },			
	{ "FR2+3",		NULL,			0x00E9 },			
	{ "FR2+4",		NULL,			0x00EA },			
	{ "FR2+5",		NULL,			0x00EB },			
	{ "FRX",			NULL,			0x00EC },			
	{ "EEXP",		NULL,			0x00ED },			
	{ "NSIGN",		NULL,			0x00EE },			
	{ "ESIGN",		NULL,			0x00EF },			
	{ "FCHRFLG",	NULL,			0x00F0 },			
	{ "DIGRT",		NULL,			0x00F1 },			
	{ "CIX",			NULL,			0x00F2 },			
	{ "INBUFF",		NULL,			0x00F3 },			
	{ "INBUFF+1",	NULL,			0x00F4 },			
	{ "ZTEMP1",		NULL,			0x00F5 },			
	{ "ZTEMP1+1",	NULL,			0x00F6 },			
	{ "ZTEMP4",		NULL,			0x00F7 },			
	{ "ZTEMP4+1",	NULL,			0x00F8 },			
	{ "ZTEMP3",		NULL,			0x00F9 },			
	{ "ZTEMP3+1",	NULL,			0x00FA },			
	{ "RADFLG",		NULL,			0x00FB },			
	{ "FLPTR",		NULL,			0x00FC },			
	{ "FLPTR+1",	NULL,			0x00FD },			
	{ "FPTR2",		NULL,			0x00FE },			
	{ "FPTR2+1",	NULL,			0x00FF },			
};

symtable_rec symtable_a8osvars[] = 
{ 
	{ "VDSLST",		NULL,			0x0200 },			
	{ "VDSLST+1",	NULL,			0x0201 },			
	{ "VPRCED",		NULL,			0x0202 },			
	{ "VPRCED+1",	NULL,			0x0203 },			
	{ "VINTER",		NULL,			0x0204 },			
	{ "VINTER+1",	NULL,			0x0205 },			
	{ "VBREAK",		NULL,			0x0206 },			
	{ "VBREAK+1",	NULL,			0x0207 },			
	{ "VKEYBD",		NULL,			0x0208 },			
	{ "VKEYBD+1",	NULL,			0x0209 },			
	{ "VSERIN",		NULL,			0x020A },			
	{ "VSERIN+1",	NULL,			0x020B },			
	{ "VSEROR",		NULL,			0x020C },			
	{ "VSEROR+1",	NULL,			0x020D },			
	{ "VSEROC",		NULL,			0x020E },			
	{ "VSEROC+1",	NULL,			0x020F },			
	{ "VTIMR1",		NULL,			0x0210 },			
	{ "VTIMR1+1",	NULL,			0x0211 },			
	{ "VTIMR2",		NULL,			0x0212 },			
	{ "VTIMR2+1",	NULL,			0x0213 },			
	{ "VTIMR4",		NULL,			0x0214 },			
	{ "VTIMR4+1",	NULL,			0x0215 },			
	{ "VIMIRQ",		NULL,			0x0216 },			
	{ "VIMIRQ+1",	NULL,			0x0217 },			
	{ "CDTMV1",		NULL,			0x0218 },			
	{ "CDTMV1+1",	NULL,			0x0219 },			
	{ "CDTMV2",		NULL,			0x021A },			
	{ "CDTMV2+1",	NULL,			0x021B },			
	{ "CDTMV3",		NULL,			0x021C },			
	{ "CDTMV3+1",	NULL,			0x021D },			
	{ "CDTMV4",		NULL,			0x021E },			
	{ "CDTMV4+1",	NULL,			0x021F },			
	{ "CDTMV5",		NULL,			0x0220 },			
	{ "CDTMV5+1",	NULL,			0x0221 },			
	{ "VVBLKI",		NULL,			0x0222 },			
	{ "VVBLKI+1",	NULL,			0x0223 },			
	{ "VVBLKD",		NULL,			0x0224 },			
	{ "VVBLKD+1",	NULL,			0x0225 },			
	{ "CDTMA1",		NULL,			0x0226 },			
	{ "CDTMA1+1",	NULL,			0x0227 },			
	{ "CDTMA2",		NULL,			0x0228 },			
	{ "CDTMA2+1",	NULL,			0x0229 },			
	{ "CDTMF3",		NULL,			0x022A },			
	{ "SRTIMR",		NULL,			0x022B },			
	{ "CDTMF4",		NULL,			0x022C },			
	{ "INTEMP",		NULL,			0x022D },			
	{ "CDTMF5",		NULL,			0x022E },			
	{ "SDMCTL",		NULL,			0x022F },			
	{ "SDLSTL",		NULL,			0x0230 },			
	{ "SDLSTH",		NULL,			0x0231 },			
	{ "SSKCTL",		NULL,			0x0232 },			
	{ "SPARE",		NULL,			0x0233 },			
	{ "LPENH",		NULL,			0x0234 },			
	{ "LPENV",		NULL,			0x0235 },			
	{ "BRKKY",		NULL,			0x0236 },			
	{ "BRKKY+1",	NULL,			0x0237 },			
	{ "VPIRQ",		NULL,			0x0238 },			
	{ "VPIRQ+1",	NULL,			0x0239 },			
	{ "CDEVIC",		NULL,			0x023A },			
	{ "CCOMND",		NULL,			0x023B },			
	{ "CAUX1",		NULL,			0x023C },			
	{ "CAUX2",		NULL,			0x023D },			
	{ "TMPSIO",		NULL,			0x023E },			
	{ "ERRFLG",		NULL,			0x023F },			
	{ "DFLAGS",		NULL,			0x0240 },			
	{ "DBSECT",		NULL,			0x0241 },			
	{ "BOOTAD",		NULL,			0x0242 },			
	{ "BOOTAD+1",	NULL,			0x0243 },			
	{ "COLDST",		NULL,			0x0244 },			
	{ "RECLEN",		NULL,			0x0245 },			
	{ "DSKTIM",		NULL,			0x0246 },			
	{ "PDVMSK",		NULL,			0x0247 },			
	{ "SHPDVS",		NULL,			0x0248 },			
	{ "PDMSK",		NULL,			0x0249 },			
	{ "RELADR",		NULL,			0x024A },			
	{ "RELADR+1",	NULL,			0x024B },			
	{ "PPTMPA",		NULL,			0x024C },			
	{ "PPTMPX",		NULL,			0x024D },			
	{ "CHSALT",		NULL,			0x026B },			
	{ "VSFLAG",		NULL,			0x026C },			
	{ "KEYDIS",		NULL,			0x026D },			
	{ "FINE",		NULL,			0x026E },			
	{ "GPRIOR",		NULL,			0x026F },			
	{ "PADDL0",		NULL,			0x0270 },			
	{ "PADDL1",		NULL,			0x0271 },			
	{ "PADDL2",		NULL,			0x0272 },			
	{ "PADDL3",		NULL,			0x0273 },			
	{ "PADDL4",		NULL,			0x0274 },			
	{ "PADDL5",		NULL,			0x0275 },			
	{ "PADDL6",		NULL,			0x0276 },			
	{ "PADDL7",		NULL,			0x0277 },			
	{ "STICK0",		NULL,			0x0278 },			
	{ "STICK1",		NULL,			0x0279 },			
	{ "STICK2",		NULL,			0x027A },			
	{ "STICK3",		NULL,			0x027B },			
	{ "PTRIG0",		NULL,			0x027C },			
	{ "PTRIG1",		NULL,			0x027D },			
	{ "PTRIG2",		NULL,			0x027E },			
	{ "PTRIG3",		NULL,			0x027F },			
	{ "PTRIG4",		NULL,			0x0280 },			
	{ "PTRIG5",		NULL,			0x0281 },			
	{ "PTRIG6",		NULL,			0x0282 },			
	{ "PTRIG7",		NULL,			0x0283 },			
	{ "STRIG0",		NULL,			0x0284 },			
	{ "STRIG1",		NULL,			0x0285 },			
	{ "STRIG2",		NULL,			0x0286 },			
	{ "STRIG3",		NULL,			0x0287 },			
	{ "HIBYTE",		NULL,			0x0288 },			
	{ "WMODE",		NULL,			0x0289 },			
	{ "BLIM",		NULL,			0x028A },			
	{ "IMASK",		NULL,			0x028B },			
	{ "JVECK",		NULL,			0x028C },			
	{ "NEWADR",		NULL,			0x028E },			
	{ "TXTROW",		NULL,			0x0290 },			
	{ "TXTCOL",		NULL,			0x0291 },			
	{ "TXTCOL+1",	NULL,			0x0292 },			
	{ "TINDEX",		NULL,			0x0293 },			
	{ "TXTMSC",		NULL,			0x0294 },			
	{ "TXTMSC+1",	NULL,			0x0295 },			
	{ "TXTOLD",		NULL,			0x0296 },			
	{ "TXTOLD+1",	NULL,			0x0297 },			
	{ "TXTOLD+2",	NULL,			0x0298 },			
	{ "TXTOLD+3",	NULL,			0x0299 },			
	{ "TXTOLD+4",	NULL,			0x029A },			
	{ "TXTOLD+5",	NULL,			0x029B },			
	{ "CRETRY",		NULL,			0x029C },			
	{ "HOLD3",		NULL,			0x029D },			
	{ "SUBTMP",		NULL,			0x029E },			
	{ "HOLD2",		NULL,			0x029F },			
	{ "DMASK",		NULL,			0x02A0 },			
	{ "TMPLBT",		NULL,			0x02A1 },			
	{ "ESCFLG",		NULL,			0x02A2 },			
	{ "TABMAP",		NULL,			0x02A3 },			
	{ "TABMAP+1",	NULL,			0x02A4 },			
	{ "TABMAP+2",	NULL,			0x02A5 },			
	{ "TABMAP+3",	NULL,			0x02A6 },			
	{ "TABMAP+4",	NULL,			0x02A7 },			
	{ "TABMAP+5",	NULL,			0x02A8 },			
	{ "TABMAP+6",	NULL,			0x02A9 },			
	{ "TABMAP+7",	NULL,			0x02AA },			
	{ "TABMAP+8",	NULL,			0x02AB },			
	{ "TABMAP+9",	NULL,			0x02AC },			
	{ "TABMAP+A",	NULL,			0x02AD },			
	{ "TABMAP+B",	NULL,			0x02AE },			
	{ "TABMAP+C",	NULL,			0x02AF },			
	{ "TABMAP+D",	NULL,			0x02B0 },			
	{ "TABMAP+E",	NULL,			0x02B1 },			
	{ "LOGMAP",		NULL,			0x02B2 },			
	{ "LOGMAP+1",	NULL,			0x02B3 },			
	{ "LOGMAP+2",	NULL,			0x02B4 },			
	{ "LOGMAP+3",	NULL,			0x02B5 },			
	{ "INVFLG",		NULL,			0x02B6 },			
	{ "FILFLG",		NULL,			0x02B7 },			
	{ "TMPROW",		NULL,			0x02B8 },			
	{ "TMPCOL",		NULL,			0x02B9 },			
	{ "TMPCOL+1",	NULL,			0x02BA },			
	{ "SCRFLG",		NULL,			0x02BB },			
	{ "HOLD4",		NULL,			0x02BC },			
	{ "DRETRY",		NULL,			0x02BD },			
	{ "SHFLOC",		NULL,			0x02BE },			
	{ "BOTSCR",		NULL,			0x02BF },			
	{ "PCOLR0",		NULL,			0x02C0 },			
	{ "PCOLR1",		NULL,			0x02C1 },			
	{ "PCOLR2",		NULL,			0x02C2 },			
	{ "PCOLR3",		NULL,			0x02C3 },			
	{ "COLOR0",		NULL,			0x02C4 },			
	{ "COLOR1",		NULL,			0x02C5 },			
	{ "COLOR2",		NULL,			0x02C6 },			
	{ "COLOR3",		NULL,			0x02C7 },			
	{ "COLOR4",		NULL,			0x02C8 },			
	{ "RUNADR",		NULL,			0x02C9 },			
	{ "RUNADR+1",	NULL,			0x02CA },			
	{ "HIUSED",		NULL,			0x02CB },			
	{ "HIUSED+1",	NULL,			0x02CC },			
	{ "ZHIUSE",		NULL,			0x02CD },			
	{ "ZHIUSE+1",	NULL,			0x02CE },			
	{ "GBYTEA",		NULL,			0x02CF },			
	{ "GBYTEA+1",	NULL,			0x02D0 },			
	{ "LOADAD",		NULL,			0x02D1 },			
	{ "LOADAD+1",	NULL,			0x02D2 },			
	{ "ZLOADA",		NULL,			0x02D3 },			
	{ "ZLOADA+1",	NULL,			0x02D4 },			
	{ "DSCTLN",		NULL,			0x02D5 },			
	{ "DSCTLN+1",	NULL,			0x02D6 },			
	{ "ACMISR",		NULL,			0x02D7 },			
	{ "ACMISR+1",	NULL,			0x02D8 },			
	{ "KRPDER",		NULL,			0x02D9 },			
	{ "KEYREP",		NULL,			0x02DA },			
	{ "NOCLIK",		NULL,			0x02DB },			
	{ "HELPFG",		NULL,			0x02DC },			
	{ "DMASAV",		NULL,			0x02DD },			
	{ "PBPNT",		NULL,			0x02DE },			
	{ "PBUFSZ",		NULL,			0x02DF },			
	{ "RUNAD",		NULL,			0x02E0 },			
	{ "RUNAD+1",	NULL,			0x02E1 },			
	{ "INITAD",		NULL,			0x02E2 },			
	{ "INITAD+1",	NULL,			0x02E3 },			
	{ "RAMSIZ",		NULL,			0x02E4 },			
	{ "MEMTOP",		NULL,			0x02E5 },			
	{ "MEMTOP+1",	NULL,			0x02E6 },			
	{ "MEMLO",		NULL,			0x02E7 },			
	{ "MEMLO+1",	NULL,			0x02E8 },			
	{ "HNDLOD",		NULL,			0x02E9 },			
	{ "DVSTAT",		NULL,			0x02EA },			
	{ "DVSTAT+1",	NULL,			0x02EB },			
	{ "DVSTAT+2",	NULL,			0x02EC },			
	{ "DVSTAT+3",	NULL,			0x02ED },			
	{ "CBAUDL",		NULL,			0x02EE },			
	{ "CBAUDH",		NULL,			0x02EF },			
	{ "CRSINH",		NULL,			0x02F0 },			
	{ "KEYDEL",		NULL,			0x02F1 },			
	{ "CH1",			NULL,			0x02F2 },			
	{ "CHACT",		NULL,			0x02F3 },			
	{ "CHBAS",		NULL,			0x02F4 },			
	{ "NEWROW",		NULL,			0x02F5 },			
	{ "NEWCOL",		NULL,			0x02F6 },			
	{ "NEWCOL+1",	NULL,			0x02F7 },			
	{ "ROWINC",		NULL,			0x02F8 },			
	{ "COLINC",		NULL,			0x02F9 },			
	{ "CHAR",		NULL,			0x02FA },			
	{ "ATACHR",		NULL,			0x02FB },			
	{ "CH",			NULL,			0x02FC },			
	{ "FILDAT",		NULL,			0x02FD },			
	{ "DSPFLG",		NULL,			0x02FE },			
	{ "SSFLAG",		NULL,			0x02FF },			

	{ "DDEVIC",		NULL,			0x0300 },			
	{ "DUNIT",		NULL,			0x0301 },			
	{ "DCOMND",		NULL,			0x0302 },			
	{ "DSTATS",		NULL,			0x0303 },			
	{ "DBUFLO",		NULL,			0x0304 },			
	{ "DBUFHI",		NULL,			0x0305 },			
	{ "DTIMLO",		NULL,			0x0306 },			
	{ "DUNUSE",		NULL,			0x0307 },			
	{ "DBYTLO",		NULL,			0x0308 },			
	{ "DBYTHI",		NULL,			0x0309 },			
	{ "DAUX1",		NULL,			0x030A },			
	{ "DAUX2",		NULL,			0x030B },			
	{ "TIMER1",		NULL,			0x030C },			
	{ "TIMER1+1",	NULL,			0x030D },			
	{ "ADDCOR",		NULL,			0x030E },			
	{ "CASFLG",		NULL,			0x030F },			
	{ "TIMER2",		NULL,			0x0310 },			
	{ "TIMER2+1",	NULL,			0x0311 },			
	{ "TEMP1",		NULL,			0x0312 },			
	{ "TEMP1+1",	NULL,			0x0313 },			
	{ "TEMP2",		NULL,			0x0314 },			
	{ "TEMP3",		NULL,			0x0315 },			
	{ "SAVIO",		NULL,			0x0316 },			
	{ "TIMFLG",		NULL,			0x0317 },			
	{ "STACKP",		NULL,			0x0318 },			
	{ "TSTAT",		NULL,			0x0319 },			
	{ "HATABS",		NULL,			0x031A },			/*HATABS 1-34*/
	{ "PUTBT1",		NULL,			0x033D },			
	{ "PUTBT2",		NULL,			0x033E },			
	{ "PUTBT3",		NULL,			0x033F },			
	{ "B0_ICHID",	NULL,			0x0340 },			
	{ "B0_ICDNO",	NULL,			0x0341 },			
	{ "B0_ICCOM",	NULL,			0x0342 },			
	{ "B0_ICSTA",	NULL,			0x0343 },			
	{ "B0_ICBAL",	NULL,			0x0344 },			
	{ "B0_ICBAH",	NULL,			0x0345 },			
	{ "B0_ICPTL",	NULL,			0x0346 },			
	{ "B0_ICPTH",	NULL,			0x0347 },			
	{ "B0_ICBLL",	NULL,			0x0348 },			
	{ "B0_ICBLH",	NULL,			0x0349 },			
	{ "B0_ICAX1",	NULL,			0x034A },			
	{ "B0_ICAX2",	NULL,			0x034B },			
	{ "B0_ICAX3",	NULL,			0x034C },			
	{ "B0_ICAX4",	NULL,			0x034D },			
	{ "B0_ICAX5",	NULL,			0x034E },			
	{ "B0_ICAX6",	NULL,			0x034F },			
	{ "B1_ICHID",	NULL,			0x0350 },			
	{ "B1_ICDNO",	NULL,			0x0351 },			
	{ "B1_ICCOM",	NULL,			0x0352 },			
	{ "B1_ICSTA",	NULL,			0x0353 },			
	{ "B1_ICBAL",	NULL,			0x0354 },			
	{ "B1_ICBAH",	NULL,			0x0355 },			
	{ "B1_ICPTL",	NULL,			0x0356 },			
	{ "B1_ICPTH",	NULL,			0x0357 },			
	{ "B1_ICBLL",	NULL,			0x0358 },			
	{ "B1_ICBLH",	NULL,			0x0359 },			
	{ "B1_ICAX1",	NULL,			0x035A },			
	{ "B1_ICAX2",	NULL,			0x035B },			
	{ "B1_ICAX3",	NULL,			0x035C },			
	{ "B1_ICAX4",	NULL,			0x035D },			
	{ "B1_ICAX5",	NULL,			0x035E },			
	{ "B1_ICAX6",	NULL,			0x035F },			
	{ "B2_ICHID",	NULL,			0x0360 },			
	{ "B2_ICDNO",	NULL,			0x0361 },			
	{ "B2_ICCOM",	NULL,			0x0362 },			
	{ "B2_ICSTA",	NULL,			0x0363 },			
	{ "B2_ICBAL",	NULL,			0x0364 },			
	{ "B2_ICBAH",	NULL,			0x0365 },			
	{ "B2_ICPTL",	NULL,			0x0366 },			
	{ "B2_ICPTH",	NULL,			0x0367 },			
	{ "B2_ICBLL",	NULL,			0x0368 },			
	{ "B2_ICBLH",	NULL,			0x0369 },			
	{ "B2_ICAX1",	NULL,			0x036A },			
	{ "B2_ICAX2",	NULL,			0x036B },			
	{ "B2_ICAX3",	NULL,			0x036C },			
	{ "B2_ICAX4",	NULL,			0x036D },			
	{ "B2_ICAX5",	NULL,			0x036E },			
	{ "B2_ICAX6",	NULL,			0x036F },			
	{ "B3_ICHID",	NULL,			0x0370 },			
	{ "B3_ICDNO",	NULL,			0x0371 },			
	{ "B3_ICCOM",	NULL,			0x0372 },			
	{ "B3_ICSTA",	NULL,			0x0373 },			
	{ "B3_ICBAL",	NULL,			0x0374 },			
	{ "B3_ICBAH",	NULL,			0x0375 },			
	{ "B3_ICPTL",	NULL,			0x0376 },			
	{ "B3_ICPTH",	NULL,			0x0377 },			
	{ "B3_ICBLL",	NULL,			0x0378 },			
	{ "B3_ICBLH",	NULL,			0x0379 },			
	{ "B3_ICAX1",	NULL,			0x037A },			
	{ "B3_ICAX2",	NULL,			0x037B },			
	{ "B3_ICAX3",	NULL,			0x037C },			
	{ "B3_ICAX4",	NULL,			0x037D },			
	{ "B3_ICAX5",	NULL,			0x037E },			
	{ "B3_ICAX6",	NULL,			0x037F },			
	{ "B4_ICHID",	NULL,			0x0380 },			
	{ "B4_ICDNO",	NULL,			0x0381 },			
	{ "B4_ICCOM",	NULL,			0x0382 },			
	{ "B4_ICSTA",	NULL,			0x0383 },			
	{ "B4_ICBAL",	NULL,			0x0384 },			
	{ "B4_ICBAH",	NULL,			0x0385 },			
	{ "B4_ICPTL",	NULL,			0x0386 },			
	{ "B4_ICPTH",	NULL,			0x0387 },			
	{ "B4_ICBLL",	NULL,			0x0388 },			
	{ "B4_ICBLH",	NULL,			0x0389 },			
	{ "B4_ICAX1",	NULL,			0x038A },			
	{ "B4_ICAX2",	NULL,			0x038B },			
	{ "B4_ICAX3",	NULL,			0x038C },			
	{ "B4_ICAX4",	NULL,			0x038D },			
	{ "B4_ICAX5",	NULL,			0x038E },			
	{ "B4_ICAX6",	NULL,			0x038F },			
	{ "B5_ICHID",	NULL,			0x0390 },			
	{ "B5_ICDNO",	NULL,			0x0391 },			
	{ "B5_ICCOM",	NULL,			0x0392 },			
	{ "B5_ICSTA",	NULL,			0x0393 },			
	{ "B5_ICBAL",	NULL,			0x0394 },			
	{ "B5_ICBAH",	NULL,			0x0395 },			
	{ "B5_ICPTL",	NULL,			0x0396 },			
	{ "B5_ICPTH",	NULL,			0x0397 },			
	{ "B5_ICBLL",	NULL,			0x0398 },			
	{ "B5_ICBLH",	NULL,			0x0399 },			
	{ "B5_ICAX1",	NULL,			0x039A },			
	{ "B5_ICAX2",	NULL,			0x039B },			
	{ "B5_ICAX3",	NULL,			0x039C },			
	{ "B5_ICAX4",	NULL,			0x039D },			
	{ "B5_ICAX5",	NULL,			0x039E },			
	{ "B5_ICAX6",	NULL,			0x039F },			
	{ "B6_ICHID",	NULL,			0x03A0 },			
	{ "B6_ICDNO",	NULL,			0x03A1 },			
	{ "B6_ICCOM",	NULL,			0x03A2 },			
	{ "B6_ICSTA",	NULL,			0x03A3 },			
	{ "B6_ICBAL",	NULL,			0x03A4 },			
	{ "B6_ICBAH",	NULL,			0x03A5 },			
	{ "B6_ICPTL",	NULL,			0x03A6 },			
	{ "B6_ICPTH",	NULL,			0x03A7 },			
	{ "B6_ICBLL",	NULL,			0x03A8 },			
	{ "B6_ICBLH",	NULL,			0x03A9 },			
	{ "B6_ICAX1",	NULL,			0x03AA },			
	{ "B6_ICAX2",	NULL,			0x03AB },			
	{ "B6_ICAX3",	NULL,			0x03AC },			
	{ "B6_ICAX4",	NULL,			0x03AD },			
	{ "B6_ICAX5",	NULL,			0x03AE },			
	{ "B6_ICAX6",	NULL,			0x03AF },			
	{ "B7_ICHID",	NULL,			0x03B0 },			
	{ "B7_ICDNO",	NULL,			0x03B1 },			
	{ "B7_ICCOM",	NULL,			0x03B2 },			
	{ "B7_ICSTA",	NULL,			0x03B3 },			
	{ "B7_ICBAL",	NULL,			0x03B4 },			
	{ "B7_ICBAH",	NULL,			0x03B5 },			
	{ "B7_ICPTL",	NULL,			0x03B6 },			
	{ "B7_ICPTH",	NULL,			0x03B7 },			
	{ "B7_ICBLL",	NULL,			0x03B8 },			
	{ "B7_ICBLH",	NULL,			0x03B9 },			
	{ "B7_ICAX1",	NULL,			0x03BA },			
	{ "B7_ICAX2",	NULL,			0x03BB },			
	{ "B7_ICAX3",	NULL,			0x03BC },			
	{ "B7_ICAX4",	NULL,			0x03BD },			
	{ "B7_ICAX5",	NULL,			0x03BE },			
	{ "B7_ICAX6",	NULL,			0x03BF },			
	{ "PRNBUF",		NULL,			0x03C0 },			/*PRNBUF 1-39 */
	{ "SUPERF",		NULL,			0x03E8 },			
	{ "CKEY",		NULL,			0x03E9 },			
	{ "CASSBT",		NULL,			0x03EA },			
	{ "CARTCK",		NULL,			0x03EB },			
	{ "DERRF",		NULL,			0x03EC },			
	{ "ACMVAR",		NULL,			0x03ED },			/*ACMVAR 1-10*/
	{ "BASICF",		NULL,			0x03F8 },			
	{ "MINTLK",		NULL,			0x03F9 },			
	{ "GINTLK",		NULL,			0x03FA },			
	{ "CHLINK",		NULL,			0x03FB },			
	{ "CHLINK+1",	NULL,			0x03FC },			
	{ "CASBUF",		NULL,			0x03FD },			
};

symtable_rec symtable_a8chips[] =
{ 
	{ "M0PF",		"HPOSP0",	0xD000 },			
	{ "M1PF",		"HPOSP1",	0xD001 },			
	{ "M2PF",		"HPOSP2",	0xD002 },			
	{ "M3PF",		"HPOSP3",	0xD003 },			
	{ "P0PF",		"HPOSM0",	0xD004 },			
	{ "P1PF",		"HPOSM1",	0xD005 },			
	{ "P2PF",		"HPOSM2",	0xD006 },			
	{ "P3PF",		"HPOSM3",	0xD007 },			
	{ "M0PL",		"SIZEP0",	0xD008 },			
	{ "M1PL",		"SIZEP1",	0xD009 },			
	{ "M2PL",		"SIZEP2",	0xD00A },			
	{ "M3PL",		"SIZEP3",	0xD00B },			
	{ "P0PL",		"SIZEM",		0xD00C },			
	{ "P1PL",		"GRAFP0",	0xD00D },			
	{ "P2PL",		"GRAFP1",	0xD00E },			
	{ "P3PL",		"GRAFP2",	0xD00F },			
	{ "TRIG0",		"GRAFP3",	0xD010 },			
	{ "TRIG1",		"GRAFM",		0xD011 },			
	{ "TRIG2",		"COLPM0",	0xD012 },			
	{ "TRIG3",		"COLPM1",	0xD013 },			
	{ "PAL",			"COLPM2",	0xD014 },			
	{ "COLPM3",		NULL,			0xD015 },			
	{ "COLPF0",		NULL,			0xD016 },			
	{ "COLPF1",		NULL,			0xD017 },			
	{ "COLPF2",		NULL,			0xD018 },			
	{ "COLPF3",		NULL,			0xD019 },			
	{ "COLBK",		NULL,			0xD01A },			
	{ "PRIOR",		NULL,			0xD01B },			
	{ "VDELAY",		NULL,			0xD01C },			
	{ "GRACTL",		NULL,			0xD01D },			
	{ "HITCLR",		NULL,			0xD01E },			
	{ "CONSOL",		NULL,			0xD01F },			
	
	{ "POT0",		"AUDF1",		0xD200 },			
	{ "POT1",		"AUDC1",		0xD201 },			
	{ "POT2",		"AUDF2",		0xD202 },			
	{ "POT3",		"AUDC2",		0xD203 },			
	{ "POT4",		"AUDF3",		0xD204 },			
	{ "POT5",		"AUDC3",		0xD205 },			
	{ "POT6",		"AUDF4",		0xD206 },			
	{ "POT7",		"AUDC4",		0xD207 },			
	{ "ALLPOT",		"AUDCTL",	0xD208 },			
	{ "KBCODE",		"STIMER",	0xD209 },			
	{ "RANDOM",		"SKREST",	0xD20A },			
	{ "POTGO",		NULL,			0xD20B },			
	{ "SERIN",		"SEROUT",	0xD20D },			
	{ "IRQST",		"IRQEN",		0xD20E },			
	{ "SKSTAT",		"SKCTL",		0xD20F },			

	{ "PORTA",		NULL,			0xD300 },			
	{ "PORTB",		NULL,			0xD301 },			
	{ "PACTL",		NULL,			0xD302 },			
	{ "PBCTL",		NULL,			0xD303 },			
	
	{ "DMACTL",		NULL,			0xD400 },			
	{ "CHACTL",		NULL,			0xD401 },			
	{ "DLISTL",		NULL,			0xD402 },			
	{ "DLISTH",		NULL,			0xD403 },			
	{ "HSCROL",		NULL,			0xD404 },			
	{ "VSCROL",		NULL,			0xD405 },			
	{ "PMBASE",		NULL,			0xD407 },			
	{ "CHBASE",		NULL,			0xD409 },			
	{ "WSYNC",		NULL,			0xD40A },			
	{ "VCOUNT",		NULL,			0xD40B },			
	{ "PENH",		NULL,			0xD40C },			
	{ "PENL",		NULL,			0xD40D },			
	{ "NMIEN",		NULL,			0xD40E },			
	{ "NMIST",		"NMIRES",	0xD40F },			
};

symtable_rec symtable_a8mathrom[] =
{ 
	{ "AFP",			NULL,		0xD800 },			
	{ "FASC",		NULL,		0xD8E6 },			
	{ "IFP",			NULL,		0xD9AA },			
	{ "FPI",			NULL,		0xD9D2 },			
	{ "ZPRO",		NULL,		0xDA44 },			
	{ "ZF1",			NULL,		0xDA46 },			
	{ "FSUB",		NULL,		0xDA60 },			
	{ "FADD",		NULL,		0xDA66 },			
	{ "FMUL",		NULL,		0xDADB },			
	{ "FDIV",		NULL,		0xDB28 },			
	{ "PLYEVL",		NULL,		0xDD40 },			
	{ "FLD0R",		NULL,		0xDD89 },			
	{ "FLD0R",		NULL,		0xDD8D },			
	{ "FLD1R",		NULL,		0xDD98 },			
	{ "FLD1P",		NULL,		0xDD9C },			
	{ "FST0R",		NULL,		0xDDA7 },			
	{ "FST0P",		NULL,		0xDDAB },			
	{ "FMOVE",		NULL,		0xDDB6 },			
	{ "EXP",			NULL,		0xDDC0 },			
	{ "EXP10",		NULL,		0xDDCC },			
	{ "LOG",			NULL,		0xDECD },			
	{ "LOG10",		NULL,		0xDED1 },			
};

symtable_rec symtable_a8osrom[] =
{ 
	{ "DSKINV",		NULL,		0xE453 },			
	{ "CIOV",		NULL,		0xE456 },			
	{ "SIOV",		NULL,		0xE459 },			
	{ "SETVBV",		NULL,		0xE45C },			
	{ "SYSVBV",		NULL,		0xE45F },			
	{ "XITVBV",		NULL,		0xE462 },			
	{ "SIOINV",		NULL,		0xE465 },			
	{ "SENDEV",		NULL,		0xE468 },			
	{ "INTINV",		NULL,		0xE46B },			
	{ "CIOINV",		NULL,		0xE46E },			
	{ "SELFSV",		NULL,		0xE471 },			
	{ "WARMSV",		NULL,		0xE474 },			
	{ "COLDSV",		NULL,		0xE477 },			
	{ "RBLOKV",		NULL,		0xE47A },			
	{ "CSOPIV",		NULL,		0xE47D },			
	{ "PUPDIV",		NULL,		0xE480 },			
	{ "SELFTSV",	NULL,		0xE483 },			
	{ "PENTV",		NULL,		0xE486 },			
	{ "PHUNLV",		NULL,		0xE489 },			
	{ "PHINIV",		NULL,		0xE48C },			
	{ "GPDVV",		NULL,		0xE48F }
};
#endif

#ifdef __A7_SYMBOLS__
symtable_rec symtable_a7chips[] =
{
	{ "VSYNC",		NULL,		0x0000 },
	{ "INPTCTRL",	NULL,		0x0001 },
	{ "WSYNC26",	NULL,		0x0002 },
	{ "RSYNC",		NULL,		0x0003 },
	{ "NUSIZ0",		NULL,		0x0004 },
	{ "NUSIZ1",		NULL,		0x0005 },
	{ "COLUP0",		NULL,		0x0006 },
	{ "COLUP1",		NULL,		0x0007 },
	{ "INPT0",		"COLUPF",0x0008 },
	{ "INPT1",		"COLUBK",0x0009 },
	{ "INPT2",		"CTRLPF",0x000A },
	{ "INPT3",		"REFP0",	0x000B },
	{ "INPT4",		"REFP1",	0x000C },
	{ "INPT5",		"PF0", 	0x000D },
	{ "PF1",			NULL,		0x000E },
	{ "PF2",			NULL,		0x000F },
	{ "RESP0",		NULL,		0x0010 },
	{ "RESP1",		NULL,		0x0011 },
	{ "RESM0",		NULL,		0x0012 },
	{ "RESM1",		NULL,		0x0013 },
	{ "RESBL",		NULL,		0x0014 },
	{ "AUDC0",		NULL,		0x0015 },
	{ "AUDC1",		NULL,		0x0016 },
	{ "AUDF0",		NULL, 	0x0017 },
	{ "AUDF1",		NULL, 	0x0018 },
	{ "AUDV0",		NULL, 	0x0019 },
	{ "AUDV1",		NULL, 	0x001A },
	{ "GRP0",		NULL,		0x001B },
	{ "GRP1",    	NULL,		0x001C },
	{ "ENAM0",    	NULL,		0x001D },
	{ "ENAM1",    	NULL,		0x001E },
	{ "ENAMBL",   	NULL,		0x001F },

	{ "BACKGRND",	NULL,		0x0020 },
	{ "P0C1",		NULL,		0x0021 },
	{ "P0C2",		NULL,		0x0022 },
	{ "P0C3",		NULL,		0x0023 },
	{ "WSYNC",		NULL,		0x0024 },
	{ "P1C1",		NULL,		0x0025 },
	{ "P1C2",		NULL,		0x0026 },
	{ "P1C3",		NULL,		0x0027 },
	{ "MSTAT",		NULL,		0x0028 },
	{ "P2C1",		NULL,		0x0029 },
	{ "P2C2",		NULL,		0x002A },
	{ "P2C3",		NULL,		0x002B },
	{ "DPPH",		NULL,		0x002C },
	{ "P3C1",		NULL,		0x002D },
	{ "P3C2",		NULL,		0x002E },
	{ "P3C3",		NULL,		0x002F },
	{ "DPPL",		NULL,		0x0030 },
	{ "P4C1",		NULL,		0x0031 },
	{ "P4C2",		NULL,		0x0032 },
	{ "P4C3",		NULL,		0x0033 },
	{ "CHARBASE",	NULL,		0x0034 },
	{ "P5C1",		NULL,		0x0035 },
	{ "P5C2",		NULL,		0x0036 },
	{ "P5C3",		NULL,		0x0037 },
	{ "OFFSET",		NULL,		0x0038 },
	{ "P6C1",		NULL,		0x0039 },
	{ "P6C2",		NULL,		0x003A },
	{ "P6C3",		NULL,		0x003B },
	{ "CTRL",		NULL,		0x003C },
	{ "P7C1",		NULL,		0x003D },
	{ "P7C2",		NULL,		0x003E },
	{ "P7C3",		NULL,		0x003F },

	{ "SWCHA",		NULL,		0x0280 },
	{ "CTLSWA",		NULL,		0x0281 },
	{ "SWCHB",		NULL,		0x0282 },
	{ "CTLSWB",		NULL,		0x0283 },
	{ "INTIM",		NULL,		0x0284 },
	{ "TIM1T",		NULL,		0x0294 },
	{ "TIM8T",		NULL,		0x0295 },
	{ "TIM64T",		NULL,		0x0296 },
	{ "T1024T",		NULL,		0x0297 },

};
#endif

supersymtable_rec supersymtable[] = {
#ifdef __A8_SYMBOLS__
	{ A8_ZPAGE_LO, symtable_a8zpage_lo, sizeof( symtable_a8zpage_lo ) / sizeof( symtable_rec ), 0, 0, TRUE },
	{ A8_ZPAGE_HI, symtable_a8zpage_hi, sizeof( symtable_a8zpage_hi ) / sizeof( symtable_rec ), 0, 0, TRUE },
	{ A8_OSVARS, symtable_a8osvars, sizeof( symtable_a8osvars ) / sizeof( symtable_rec ), 0, 0, TRUE },
	{ A8_CHIPS, symtable_a8chips, sizeof( symtable_a8chips ) / sizeof( symtable_rec ), 0, 0, TRUE },
	{ A8_MATHROM, symtable_a8mathrom, sizeof( symtable_a8mathrom ) / sizeof( symtable_rec ), 0, 0, TRUE },
	{ A8_OSROM, symtable_a8osrom, sizeof( symtable_a8osrom ) / sizeof( symtable_rec ), 0, 0, TRUE },
#endif

#ifdef __A7_SYMBOLS__
	{ A7_CHIPS, symtable_a7chips, sizeof( symtable_a7chips ) / sizeof( symtable_rec ), 0, 0, TRUE },
#endif
};

#define supersymtable_len ( sizeof( supersymtable ) / sizeof( supersymtable_rec ) )

void CDis::Init( DISS_TYPE dtype, PROC_TYPE ptype, UNDOC_TYPE utype, DWORD dwAllowMask )
{
	m_asm_type = dtype;
	m_proc_type = ptype;
	m_undoc_type = utype;

	for( int i = 0; i < (int)supersymtable_len; i++ )
	{
		if( supersymtable[ i ].m_dwClass & dwAllowMask )
		{
			supersymtable[ i ].m_bAllowed = TRUE;
		}
		else
		{
			supersymtable[ i ].m_bAllowed = FALSE;
		}

		symtable_rec* mysym = supersymtable[ i ].m_symtable;

		supersymtable[ i ].m_wFirst = mysym[ 0 ].m_wAddr;
		supersymtable[ i ].m_wLast = mysym[ supersymtable[ i ].m_wCnt - 1 ].m_wAddr;
	}
}

INSTRUCTION instr_def_unk = { ADDR_IMPLIED, ACCESS_READ, FALSE };

void CDis::OutputBlockDBs( BYTE* pbtBlock, WORD wOffset, WORD wEnd, WORD wSize )
{
	WORD wCnt = wEnd - wOffset + 1;

	WORD wPC = 0;

	BOOL bBeg = TRUE;

	while( wCnt )
	{
		if( bBeg )
		{
			switch ( m_asm_type )
			{
				default:
				case DISS_XA:
					printf( "\t\t.BYT\t" );
					break;

				case DISS_DASM:
					printf( "\t\t.BYTE\t" );
					break;

				case DISS_XASM:
					printf( "\t\tDTA\t" );
					break;
			}

			bBeg = FALSE;

		}
		else
		{
			printf( ", " );
		}

		printf( "$%02X", pbtBlock[ wPC ] );

		if( !(( wPC + 1 ) % wSize ))
		{
			bBeg = TRUE;
			printf( "\n" );
		}

		wPC++;

		wCnt--;
	}
	printf( "\n" );

}

void CDis::OutputBlockDiss( BYTE* pbtBlock, WORD wOffset, WORD wEnd )
{
	BOOL bInsFirst = FALSE;
	char** pInsTable = mnem_table_gen;

	switch( m_asm_type )
	{
		case DISS_XA:
		case DISS_XASM:
			bInsFirst = TRUE;
			break;

		case DISS_DASM:
			pInsTable = mnem_table_dasm;
			bInsFirst = TRUE;
			break;

		default:
			bInsFirst = FALSE;
			break;
	}

	WORD wPC_curr = 0;
	WORD wPC_end = wEnd - wOffset;

	BOOL bInsIncomplete = FALSE;
	int btInsCode;

	while ( wPC_curr <= wPC_end )
	{
		btInsCode = pbtBlock[ wPC_curr ];

		INSTRUCTION* pInst = instable + btInsCode;

		if( ( m_undoc_type == UNDOC_NONE ) && pInst->undoc )
		{
			pInst = &instr_def_unk;
			btInsCode = 256;
		}

		ADDR_TYPES type = pInst->type;

		int iInsLen;
		switch( type )
		{
			default:
				iInsLen = 0;
				break;

			case ADDR_IMMEDIATE:
			case ADDR_INDIRECTX:
			case ADDR_INDIRECTY:
			case ADDR_BRANCH:
			case ADDR_ZPAGE:
			case ADDR_ZPAGEX:
			case ADDR_ZPAGEY:
				iInsLen = 1;
				break;

			case ADDR_INDIRECT:
			case ADDR_ABSOLUTE:
			case ADDR_ABSOLUTEX:
			case ADDR_ABSOLUTEY:
				iInsLen = 2;
				break;
		}

		char szOffBytes[ 50 ];
		sprintf( szOffBytes, "%04X  ", wPC_curr + wOffset );

		if ( wPC_curr + iInsLen > wPC_end )
		{
			iInsLen = wPC_end - wPC_curr + 1;
			bInsIncomplete = TRUE;
		}

		for ( int i = 0; i <= 3; i++ )
		{
			if ( i <= iInsLen )
				sprintf( szOffBytes + 6 + i*3, "%02X ", pbtBlock[ wPC_curr + i ] );
			else
				sprintf( szOffBytes + 6 + i*3, "   " );

		}

		char* szIns = pInsTable[ btInsCode ];

		WORD wAddr = 0xFFFF;
		if ( iInsLen == 1 )
		{
			wAddr = (WORD)pbtBlock[ wPC_curr + 1 ];
		}
		else if ( iInsLen == 2 )
		{
			wAddr = (WORD)pbtBlock[ wPC_curr + 1 ] + ( (WORD)pbtBlock[ wPC_curr + 2 ] << 8 );
		}

		if ( !szIns )
		{
			switch( m_asm_type )
			{
				case DISS_XA:
					szIns = ".BYT";
					type = ADDR_DBDATA;
					iInsLen = 1;
					break;

				case DISS_DASM:
					szIns = ".BYTE";
					type = ADDR_DBDATA;
					iInsLen = 1;
					break;

				case DISS_XASM:
					szIns = "DTA";
					type = ADDR_DBDATA;
					iInsLen = 1;
					break;

				case DISS_PLAIN:
					szIns = "???";
					break;
			}
		}

		if ( bInsIncomplete )
		{
			switch( m_asm_type )
			{
				case DISS_XA:
					szIns = ".BYT";
					type = ADDR_DBDATA;
					break;

				case DISS_DASM:
					szIns = ".BYTE";
					type = ADDR_DBDATA;
					break;

				case DISS_XASM:
					szIns = "DTA";
					type = ADDR_DBDATA;
					break;

				case DISS_PLAIN:
					szIns = "!!!";
					type = ADDR_IMPLIED;
					break;
			}
		}

		if ( bInsFirst )
		{
			printf( "\t\t" );
		}
		else
		{
			printf( szOffBytes );
		}

		char szOp[ 200 ];
		char szIMod[ 20 ];

		GetOperandString(		szIMod,
									szOp, 
									type, 
									iInsLen, 
									pInst->write,
									pbtBlock + wPC_curr, 
									wPC_curr + wOffset 
								);

	  	printf( "%s%s\t%s", szIns, szIMod, szOp );

		if ( bInsFirst )
		{
			printf( "\t;%s", szOffBytes );
		}

		printf( "\n" );

		//one empty line if it's RTS or RTI
		if ( ( btInsCode == 0x60 ) || ( btInsCode == 0x40 ) )
		{
			printf( "\n" );
		}

		if ( type == ADDR_DBDATA )
		{
			iInsLen--;
		}

		wPC_curr += ( iInsLen + 1 );
	}

}

char* CDis::SymbolFind( WORD addr, int iLen, MEMORY_ACCESS access )
{
	static char szBuf[ 5 ];

	if( iLen == 1 )
		sprintf( szBuf, "$%02X", addr );
	else
		sprintf( szBuf, "$%04X", addr );

	//superhack
	//if ( 1 )
	//{
	//	if( ( addr >= 0xd000 ) && ( addr <= 0xd0ff ) )
	//		addr &= 0xFF1F;
	//	if( ( addr >= 0xd200 ) && ( addr <= 0xd2ff ) )
	//		addr &= 0xFF0F;
	//	if( ( addr >= 0xd300 ) && ( addr <= 0xd3ff ) )
	//		addr &= 0xFF03;
	//	if( ( addr >= 0xd400 ) && ( addr <= 0xd4ff ) )
	//		addr &= 0xFF0F;
	//}

	supersymtable_rec* pSuper = supersymtable;
	symtable_rec* pTable	= NULL;
	WORD wSyms = 0;

	for( int i = 0; i < (int)supersymtable_len; i++ )
	{
		if( pSuper->m_bAllowed && ( pSuper->m_wFirst <= addr ) && ( pSuper->m_wLast >= addr ) )
		{
			pTable = pSuper->m_symtable;
			wSyms = pSuper->m_wCnt;
			break;
		}

		pSuper++;
	}

	if( pTable == NULL )
		return szBuf;

	WORD lo = 0;
	WORD mi = 0;
	WORD hi = wSyms;

	while ( lo < hi )
	{
		mi = ( lo + hi ) / 2;

		if ( pTable[ mi ].m_wAddr == addr )
		{
			break;
		}
		else
		{
			if ( pTable[ mi ].m_wAddr > addr )
				hi=mi;
			else
				lo=mi+1;
		}
	}

	if ( pTable[mi].m_wAddr == addr )
	{
		char* szW = pTable[ mi ].m_szNameW;

		if( ( access == ACCESS_WRIT ) && ( szW != NULL ) )
			return szW;

		return pTable[ mi ].m_szNameR;
	}

	return szBuf;
}


void CDis::GetOperandString( char* szIMod, char* szRet, ADDR_TYPES type, int iLen, MEMORY_ACCESS access, BYTE* pbtBuf, WORD wRelPC )
{
	WORD wAddr = 0;
	*szRet = '\0';
	*szIMod = '\0';

	if ( iLen == 1 )
		wAddr = *( pbtBuf + 1 );
	else
		wAddr = (WORD) *( pbtBuf + 1 ) | (WORD)( *( pbtBuf + 2 ) << 8 );

	switch( type )
	{
		case ADDR_ACCUMULATOR:
		{
			switch( m_asm_type )
			{
				case DISS_XASM:
					sprintf( szRet, "@" );
					break;

				default:
					break;
			}
			break;
		}

		case ADDR_BRANCH:
		  	sprintf( szRet, "$%04X", wRelPC + 2 + (int) (signed char)wAddr );
			break;

		case ADDR_IMMEDIATE:
			sprintf( szRet, "#$%02X", wAddr );
			break;

		case ADDR_DBDATA:
			if ( iLen == 1 )
			{
				sprintf( szRet, "$%02X", *pbtBuf );
			}
			else
			{
				sprintf( szRet, "$%02X, $%02X", *pbtBuf, *( pbtBuf + 1 ) );
			}
			break;

		case ADDR_INDIRECT:
			sprintf( szRet, "(%s)", SymbolFind( wAddr, iLen, access ) );
			break;

		case ADDR_ZPAGE:
			sprintf( szRet, "%s", SymbolFind( wAddr, iLen, access ) );
			break;

		case ADDR_ZPAGEX:
			sprintf( szRet, "%s,X", SymbolFind( wAddr, iLen, access ) );
			break;

		case ADDR_ZPAGEY:
			sprintf( szRet, "%s,Y", SymbolFind( wAddr, iLen, access ) );
			break;

		case ADDR_ABSOLUTE:
			if ( wAddr < 0x100 )
			{
				switch( m_asm_type )
				{
					case DISS_XA:
						sprintf( szRet, "!%s", SymbolFind( wAddr, iLen, access ) );
						break;

					case DISS_DASM:
						sprintf( szRet, "%s", SymbolFind( wAddr, iLen, access ) );
						strcpy( szIMod, ".w" );
						break;

					case DISS_XASM:
						sprintf( szRet, "A:%s", SymbolFind( wAddr, iLen, access ) );
						break;

					default:
						sprintf( szRet, "%s", SymbolFind( wAddr, iLen, access ) );
						break;
				}
			}
			else
			{
				sprintf( szRet, "%s", SymbolFind( wAddr, iLen, access ) );
			}

			break;

		case ADDR_ABSOLUTEX:
			if ( wAddr < 0x100 )
			{
				switch( m_asm_type )
				{
					case DISS_XA:
						sprintf( szRet, "!%s,X", SymbolFind( wAddr, iLen, access ) );
						break;

					case DISS_DASM:
						sprintf( szRet, "%s,X", SymbolFind( wAddr, iLen, access ) );
						strcpy( szIMod, ".wx" );
						break;

					case DISS_XASM:
						sprintf( szRet, "A:%s,X", SymbolFind( wAddr, iLen, access ) );
						break;

					default:
						sprintf( szRet, "%s,X", SymbolFind( wAddr, iLen, access ) );
						break;
				}
			}
			else
			{
				sprintf( szRet, "%s,X", SymbolFind( wAddr, iLen, access ) );
			}

			break;

		case ADDR_ABSOLUTEY:
			if ( wAddr < 0x100 )
			{
				switch( m_asm_type )
				{
					case DISS_XA:
						sprintf( szRet, "!%s,Y", SymbolFind( wAddr, iLen, access ) );
						break;

					case DISS_DASM:
						sprintf( szRet, "%s,Y", SymbolFind( wAddr, iLen, access ) );
						strcpy( szIMod, ".wy" );
						break;

					case DISS_XASM:
						sprintf( szRet, "A:%s,Y", SymbolFind( wAddr, iLen, access ) );
						break;

					default:
						sprintf( szRet, "%s,Y", SymbolFind( wAddr, iLen, access ) );
						break;
				}
			}
			else
			{
				sprintf( szRet, "%s,Y", SymbolFind( wAddr, iLen, access ) );
			}

			break;

		case ADDR_INDIRECTX:
			sprintf( szRet, "(%s,X)", SymbolFind( wAddr, iLen, access ) );
			break;

		case ADDR_INDIRECTY:
			sprintf( szRet, "(%s),Y", SymbolFind( wAddr, iLen, access ) );
			break;

		default:
			break;
	}
}

void CDis::DumpSymbols()
{
	char* szFilename;

	switch( m_asm_type )
	{
		case DISS_XASM:
			szFilename = SYMBOLS_NAME_XASM;
			break;

		case DISS_DASM:
			szFilename = SYMBOLS_NAME_DASM;
			break;

		case DISS_XA:
		default:
			szFilename = SYMBOLS_NAME_XA;
			break;
	}
	FILE* f = fopen( szFilename, "wt" );
	if ( !f )
		return;

	supersymtable_rec* pSuper = supersymtable;

	for( int i = 0; i < (int)supersymtable_len; i++ )
	{
		if( pSuper->m_bAllowed )
		{
			symtable_rec* pTable = pSuper->m_symtable;
			WORD wSyms = pSuper->m_wCnt;

			while( wSyms-- )
			{
				PrintSym( f, pTable->m_szNameR, pTable->m_wAddr );

				if( pTable->m_szNameW != NULL )
					PrintSym( f, pTable->m_szNameW, pTable->m_wAddr );

				pTable++;
			}

			fprintf( f, "\n" );
		}

		pSuper++;
	}

	fclose( f );

}

void CDis::PrintSym( FILE* f, char* szName, WORD wAddr )
{
	if ( ! strchr( szName, '+' ) )
	{
		switch( m_asm_type )
		{
			case DISS_XA:
				fprintf( f, "%s\t=\t$%04X\n", szName, wAddr );
				break;

			case DISS_XASM:
			case DISS_DASM:
			default:
				fprintf( f, "%s\tEQU\t$%04X\n", szName, wAddr );
				break;
		}
	}
}

void CDis::PrintRem()
{
	switch( m_asm_type )
	{
		case DISS_XA:
		case DISS_XASM:
		case DISS_DASM:
			printf( ";" );
			break;

		default:
			break;
	}
}

void CDis::PrintExeBlockHeader( WORD wBlkStart, WORD wBlkEnd )
{
	switch( m_asm_type )
	{
		case DISS_XA:
		{
			printf( "\t\t*=\t$%04X\n", wBlkStart - 4 );
			printf( "\t\t.WORD\t$%04X\n", wBlkStart );
			printf( "\t\t.WORD\t$%04X\n", wBlkEnd );
			break;
		}

		case DISS_XASM:
		{
			printf( "\t\tORG\t$%04X\n", wBlkStart );
			break;
		}

		case DISS_DASM:
		{
			printf( "\t\tORG\t$%04X\n", wBlkStart );
			break;
		}

		case DISS_PLAIN:
		default:
		{
			printf( "Block:  %04X-%04X (%04X)\n", wBlkStart, wBlkEnd, wBlkEnd - wBlkStart + 1 );
			break;
		}
	}
}

void CDis::PrintBlockHeader( WORD wBlkStart, WORD wBlkEnd )
{
	switch( m_asm_type )
	{
		case DISS_XA:
		{
			printf( "\t\t*=\t$%04X\n", wBlkStart );
			break;
		}

		case DISS_XASM:
		{
			printf( "\t\tORG\t$%04X\n", wBlkStart );
			break;
		}

		case DISS_DASM:
		{
			printf( "\t\tRORG\t$%04X\n", wBlkStart );
			break;
		}

		case DISS_PLAIN:
		default:
		{
			printf( "Block:  %04X-%04X (%04X)\n", wBlkStart, wBlkEnd, wBlkEnd - wBlkStart + 1 );
			break;
		}
	}
}

void CDis::PrintExeRunAddr( WORD wAddr )
{
	switch( m_asm_type )
	{
		case DISS_XA:
		{
			printf( "\t\t.WORD\t$02E0\n" );
			printf( "\t\t.WORD\t$02E1\n" );
			printf( "\t\t.WORD\t$%04X\n", wAddr );
			break;
		}

		case DISS_XASM:
		{
			printf( "\t\tRUN\t$%04X\n", wAddr );
			break;
		}

		default:
		case DISS_DASM:
			printf( "Run  :  %04X\n", wAddr );
			break;
	}
}

void CDis::PrintExeInitAddr( WORD wAddr )
{
	switch( m_asm_type )
	{
		case DISS_XA:
		{
			printf( "\t\t.WORD\t$02E2\n" );
			printf( "\t\t.WORD\t$02E3\n" );
			printf( "\t\t.WORD\t$%04X\n", wAddr );
			break;
		}

		case DISS_XASM:
		{
			printf( "\t\tINI\t$%04X\n", wAddr );
			break;
		}

		default:
		case DISS_DASM:
			printf( "Init :  %04X\n", wAddr );
			break;
	}
}

void CDis::PrintExeHeader()
{
	switch( m_asm_type )
	{
		case DISS_XA:
		{
			printf( "\t\t.WORD\t$FFFF\n" );
			break;
		}

		default:
			break;
	}
}

void CDis::PrintWord( WORD w )
{
	switch( m_asm_type )
	{
		case DISS_XA:
		case DISS_XASM:
		case DISS_DASM:
		case DISS_PLAIN:
		{
			printf( "\t\t.WORD\t$%04X\n", w );
			break;
		}

		default:
			break;
	}
}

void CDis::PrintDirectives()
{
	switch( m_asm_type )
	{
		case DISS_XA:
		{
			printf( "#include \"" SYMBOLS_NAME_XA "\"\n" );
			break;
		}

		case DISS_DASM:
		{
			printf( "\t\tprocessor 6502\n" );
			printf( "\t\tINCLUDE \"" SYMBOLS_NAME_DASM "\"\n" );
			break;
		}

		case DISS_XASM:
		{
			printf( "\t\tICL\t'" SYMBOLS_NAME_XASM "'\n" );
			break;
		}

		default:
			break;
	}
}
