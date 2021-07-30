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

char* aCmdsTBasic[] =
{
	"REM",			//00
	"DATA",			//01
	"INPUT",			//02
	"COLOR",			//03
	"LIST",			//04
	"ENTER",			//05
	"LET",			//06
	"IF",				//07
	"FOR",			//08
	"NEXT",			//09
	"GOTO",			//0a
	"GO TO",			//0b
	"GOSUB",			//0c
	"TRAP",			//0d
	"BYE",			//0e
	"CONT",			//0f
	"COM",			//10
	"CLOSE",			//11
	"CLR",			//12
	"DEG",			//13
	"DIM",			//14
	"END",			//15
	"NEW",			//16
	"OPEN",			//17
	"LOAD",			//18
	"SAVE",			//19
	"STATUS",		//1a
	"NOTE",			//1b
	"POINT",			//1c
	"XIO",			//1d
	"ON",				//1e
	"POKE",			//1f
	"PRINT",			//20
	"RAD",			//21
	"READ",			//22
	"RESTORE",		//23
	"RETURN",		//24
	"RUN",			//25
	"STOP",			//26
	"POP",			//27
	"?",				//28
	"GET",			//29
	"PUT",			//2a
	"GRAPHICS",		//2b
	"PLOT",			//2c
	"POSITION",		//2d
	"DOS",			//2e
	"DRAWTO",		//2f
	"SETCOLOR",		//30
	"LOCATE",		//31
	"SOUND",			//32
	"LPRINT",		//33
	"CSAVE",			//34
	"CLOAD",			//35
	"",				//36 silent let
	"ERROR -",		//37 last Atari Basic command
	"DPOKE",			//38
	"MOVE",			//39
	"-MOVE",			//3A
	"*F",				//3B
	"REPEAT",		//3C
	"UNTIL",			//3D
	"WHILE",			//3E
	"WEND",			//3F
	"ELSE",			//40
	"ENDIF",			//41
	"BPUT",			//42
	"BGET",			//43
	"FILLTO",		//44
	"DO",				//45
	"LOOP",			//46
	"EXIT",			//47
	"DIR",			//48
	"LOCK",			//49
	"UNLOCK",		//4A
	"RENAME",		//4B
	"DELETE",		//4C
	"PAUSE",			//4D
	"TIME$=",		//4E
	"PROC",			//4F
	"EXEC",			//50
	"ENDPROC",		//51
	"FCOLOR",		//52
	"*L",				//53
	"------------------------------",				//54
	"RENUM",			//55
	"DEL",			//56
	"DUMP",			//57
	"TRACE",			//58
	"TEXT",			//59
	"BLOAD",			//5A
	"BRUN",			//5B
	"GO#",			//5C
	"#",				//5D
	"*B",				//5E
	"PAINT",			//5F
	"CLS",			//60
	"DSOUND",		//61
	"CIRCLE",		//62
	"\%PUT",			//63
	"\%GET",			//64
};						  

char* aOpsTBasic[] = {	  
   "NHCONST",       //0D
	"NCONST",		  //0E
	"SCONST",		  //0F
	"NOUSE",			  //10
	"NOUSE",			  //11
	",",				  //12
	"$",				  //13
	":",				  //14
	";",				  //15
	"EOL",			  //16
	" GOTO ",		  //17
	" GOSUB ",		  //18
	" TO ",			  //19
	" STEP ",		  //1A
	" THEN ",		  //1B
	"#",				  //1C
	"<=",				  //1D
	"<>",				  //1E
	">=",				  //1F
	"<",				  //20
	">",				  //21
	"=",				  //22
	"^",				  //23
	"*",				  //24
	"+",				  //25
	"-",				  //26
	"/",				  //27
	" NOT ",			  //28
	" OR ",			  //29
	" AND ",			  //2A
	"(",				  //2B
	")",				  //2C
	"=",				  //2D
	"=",				  //2E
	"<=",				  //2F
	"<>",				  //30
	">=",				  //31
	"<",				  //32
	">",				  //33
	"=",				  //34
	"+",				  //35
	"-",				  //36
	"(",				  //37
	"(",				  //38
	"(",				  //39
	"(",				  //3A
	"(",				  //3B
	",",				  //3C
	"STR$",			  //3D
	"CHR$",			  //3E
	"USR",			  //3F
	"ASC",			  //40
	"VAL",			  //41
	"LEN",			  //42
	"ADR",			  //43
	"ATN",			  //44
	"COS",			  //45
	"PEEK",			  //46
	"SIN",			  //47
	"RND",			  //48
	"FRE",			  //49
	"EXP",			  //4A
	"LOG",			  //4B
	"CLOG",			  //4C
	"SQR",			  //4D
	"SGN",			  //4E
	"ABS",			  //4F
	"INT",			  //50
	"PADDLE",		  //51
	"STICK",			  //52
	"PTRIG",			  //53
	"STRIG",			  //54	last Atari Basic
	"DPEEK",			  //55
	"&",					//56
	"!",					//57
	"INSTR",				//58
	"INKEY$",			//59
	" EXOR ",			//5A
	"HEX$",				//5B
	"DEC",				//5C
	" DIV ",				//5D
	"FRAC",				//5E
	"TIME$",				//5F
	"TIME",				//60
	" MOD ",				//61
	" EXEC ",			//62
	"RND",				//63
	"RAND",				//64
	"TRUNC",				//65
	"%0",					//66
	"%1",					//67
	"%2",					//68
	"%3",					//69
	" GO# ",				//6A
	"UINSTR",			//6B
	"ERR",				//6C
	"ERL",				//6D
};

#define TBASIC_CMD_NUM ( sizeof( aCmdsTBasic ) / sizeof( aCmdsTBasic[ 0 ] ) )
#define TBASIC_OPS_NUM ( sizeof( aOpsTBasic ) / sizeof( aOpsTBasic[ 0 ] ) )

