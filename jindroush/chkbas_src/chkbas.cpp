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
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include "carray.h"
#include "cfile.h"

#include "pub.def"

#include "basic.h"
#include "basic_tb.h"
#include "basic_xl.h"
#include "basic_xe.h"

typedef enum
{
	BASIC = 0,			//standard basic
	BASIC_TB = 1,		//Turbo Basic
	BASIC_XL = 2,		//OSS Basic XL
	BASIC_XE = 3,		//OSS Basic XE
	BASIC_XEX = 4		//OSS Basic XE in extended mode
} BASIC_TYPE;

typedef struct
{
	char** ppCommands;	//array of commands
	char** ppOperands;	//array of operands

	int iCmdNum;			//number of commands
	int iOpNum;				//number of operands
	int iOpFirst;			//first operand
} LANGDESC;

LANGDESC langtbl[] =
{
	{ aCmdsBasic, aOpsBasic, BASIC_CMD_NUM, BASIC_OPS_NUM, 0x0E },
	{ aCmdsTBasic, aOpsTBasic, TBASIC_CMD_NUM, TBASIC_OPS_NUM, 0x0D },
	{ aCmdsBasicXl, aOpsBasicXl, BASICXL_CMD_NUM, BASICXL_OPS_NUM, 0x0D },
	{ aCmdsBasicXe, aOpsBasicXe, BASICXE_CMD_NUM, BASICXE_OPS_NUM, 0x0D },
	{ aCmdsBasicXe, aOpsBasicXe, BASICXE_CMD_NUM, BASICXE_OPS_NUM, 0x0D },
};

#define OP_NHCONST 		0x0D
#define OP_NCONST			0x0E
#define OP_SCONST			0x0F
#define OP_EOL				0x16

#define VART_SCALAR		0x00

#define VART_ARRAYU		0x40
#define VART_ARRAY		0x41

#define VART_STRINGU		0x80
#define VART_STRING		0x81

#define VART_STRINGAU	0x90
#define VART_STRINGA		0x91

#define VART_PROC			0xC1
#define VART_LABEL		0xC2

#define SHEADER PRG_NAME " v" PRG_VERSION " (c) " PRG_COPYRIGHT " " PRG_AUTHOR "\n"

#define HEADER SHEADER \
   PRG_DESC "\n" \
	"  Latest version can be found at " PRG_URL "\n" \
	"  Published under GPL. See GPL.TXT.\n" \
	"  Thanks to Russ Gilbert for his SALVAGE programs.\n\n"

#define USAGE HEADER "Usage:  " PRG_NAME " " PRG_USAGE

BOOL GenCode( CFile* pcf, DWORD dwStart, DWORD dwEnd );
BOOL GenLineCode( CFile* pcf, DWORD dwEnd );
BOOL GenCmdCode( CFile* pcf, DWORD dwLineStart, DWORD dwLineEnd );
BOOL GenOpsCode( CFile* pcf, DWORD dwTokEnd );
double ReadAtariBCD( CFile* pcf );

BOOL Vars_Load( CFile* pcf, WORD wVNT, WORD wVNTL, WORD wVVT, WORD NV );
void Var_Get( char* szRet, int iNum );
void Vars_Destroy();

FILE* g_fout = NULL;
CArray g_aVariables;			//array of variables

BOOL g_bVerbose = FALSE;	//verbose output
BOOL g_bAtariOut = FALSE;	//atari output
BOOL g_bExtOut = TRUE;		//extended output
BOOL g_bHasErrors = FALSE;	//decode error flag
BOOL g_bNoInverse = FALSE;	//don't emit inverse
BASIC_TYPE g_Type = BASIC;	//basic type. Default is Atari Basic

char** g_aBasicCmds;			//array of commands
char** g_aBasicOps;			//array of operands
int g_iCmdNum;					//number of commands
int g_iOpsNum;					//number of operands
int g_iOpFirst;				//first operand

#include "switches.cpp"

int main( int argc, char* argv[] )
{
	setbuf( stdout, NULL );
	setbuf( stderr, NULL );

	if ( !SWITCHES_Init( &argc, argv ) )
		return 1;

	if ( argc < 2 )
	{
		SWFN_HELP( USAGE );
		return 1;
	}

	if ( g_bExtOut )
		fprintf( stderr, SHEADER "\n" );
	else
		fprintf( stderr, HEADER );

	char* szInfile = argv[ 1 ];
	char* szOutfile = NULL;

	if ( argc > 2 )
		szOutfile = argv[ 2 ];

	CFile cf;

	if ( !cf.Open( szInfile ) )
	{
		fprintf( stderr, "Can't open input file '%s'\n", szInfile );
		return 1;
	}

	if ( szOutfile )
	{
		if( g_bAtariOut )
			g_fout = fopen( szOutfile, "wb" );	//opening as binary because of
															//atascii
		else
			g_fout = fopen( szOutfile, "wt" );
	}

	if ( !g_fout )
		g_fout = stdout;

	//Don't emit additional info if you are in Atari mode
	if ( g_bAtariOut )
		g_bExtOut = FALSE;

	if ( g_bExtOut )
	{
		fprintf( g_fout, HEADER );
		fprintf( g_fout, "Input file: %s\n\n", szInfile );
	}

	//initialize globals depending on language set
	g_aBasicCmds = langtbl[ g_Type ].ppCommands;
	g_aBasicOps = langtbl[ g_Type ].ppOperands;
	g_iCmdNum = langtbl[ g_Type ].iCmdNum;
	g_iOpsNum = langtbl[ g_Type ].iOpNum;
	g_iOpFirst = langtbl[ g_Type ].iOpFirst;

	DWORD dwFileLen = cf.GetLength();

	//read head
	WORD wLOMEM = cf.readLEw();
	WORD wVNT = cf.readLEw();
	WORD wVNTE = cf.readLEw();
	WORD wVVT = cf.readLEw();
	WORD wSTMTAB = cf.readLEw();
	WORD wSTMCUR = cf.readLEw();
	WORD wSTARP = cf.readLEw();

	if( ( wLOMEM == 0x00DD ) && ( g_Type == BASIC_XE ) )
	{
		//extended mode of Basic XE
		wLOMEM = 0;
		g_Type = BASIC_XEX;
	}

	WORD wCOR = wVNT - wLOMEM - 0x0E;

	wVNT -= wCOR;
	wVNTE -= wCOR;
	wVVT -= wCOR;
	wSTMTAB -= wCOR;
	wSTMCUR -= wCOR;
	wSTARP -= wCOR;

	WORD wVNTL = wVNTE - wVNT + 1;
	WORD wVVTE = wSTMTAB - 1;
	WORD wVVTL = wVVTE - wVVT + 1;
	WORD NV = wVVTL / 8;

	WORD wCodeLen = wSTMCUR - wSTMTAB;
	WORD wCodeLenCur = wSTARP - wSTMCUR;

	long lLenDiff = (long)dwFileLen - (long)wSTARP;

	if ( g_bExtOut )
	{
		fprintf( g_fout, "Constants & pointers:\n" );
		fprintf( g_fout, "Start of Name Table      (VNT)   : %04X\n", wVNT );
		fprintf( g_fout, "End of Name Table        (VNTE)  : %04X\n", wVNTE );
		fprintf( g_fout, "Lenght of Name Table     (VNTL)  : %04X\n", wVNTL );

		fprintf( g_fout, "Start of Variable Table  (VVT)   : %04X\n", wVVT );
		fprintf( g_fout, "End of Variable Table    (VVTE)  : %04X\n", wVVTE );
		fprintf( g_fout, "Length of Variable Table (VVTL)  : %04X\n", wVVTL );

		fprintf( g_fout, "Number of Variables      (NV)    : %04X\n", NV );

		fprintf( g_fout, "Start of Code            (STMTAB): %04X\n", wSTMTAB );
		fprintf( g_fout, "Length of Code                   : %04X\n", wCodeLen );
		fprintf( g_fout, "Current command          (STMCUR): %04X\n", wSTMCUR );
		fprintf( g_fout, "Length of current command        : %04X\n", wCodeLenCur );
		fprintf( g_fout, "First byte after program (STARP) : %04X\n", wSTARP );

		if( g_Type != BASIC_XEX )
		{
			fprintf( g_fout, "Length of file                   : %04lX\n", dwFileLen );
			fprintf( g_fout, "File len difference              : %08lX\n", (DWORD)lLenDiff );
		}
		fputc( '\n', g_fout );
	}

	//records must be 8 bytes long
	if ( NV * 8 != wVVTL )
	{
		fprintf( stderr, "Variable Table Length Mismatch!\n" );
		return 1;
	}

	if ( lLenDiff <0 )
	{
		fprintf( stderr, "File Length Incorrect!\n" );
		return 1;
	}

	if ( !Vars_Load( &cf, wVNT, wVNTL, wVVT, NV ) )
		return 1;

	if ( g_bExtOut )
	{
		fprintf( g_fout, "Main code starts here:\n" );
	}

	GenCode( &cf, wSTMTAB, wSTMCUR );

	if ( g_bExtOut )
	{
		fputc( '\n', g_fout );

		fprintf( g_fout, "Immediate code starts here:\n" );
		GenCode( &cf, wSTMCUR, wSTARP );
		fputc( '\n', g_fout );
	}
	else
	{
		cf.Seek( wSTARP, SEEK_SET );
	}

	//if extended mode of Basic XE
	if( g_Type == BASIC_XEX )
	{
		//code is stored in 4 extended memory banks
		for( int iBank = 0; iBank < 4; iBank++ )
		{
			//read the end of code in bank
			WORD wLen = cf.readLEw();

			//because bank starts at 0x4000, the value must be GE
			if( wLen < 0x4000 )
			{
				fprintf( stderr, "Bank end too low! %04X\n", wLen );
				g_bHasErrors = TRUE;
				break;
			}

			wLen -= 0x4000;

			if ( g_bExtOut )
			{
				fprintf( g_fout, "\nBank %d code:\n", iBank );
			}

			if ( wLen )
			{
				//if we have anything to process, we do so
				long lNow = cf.Tell();
				GenCode( &cf, lNow, lNow + wLen );
			}
			else
			{
				//else we skip one byte (error in BXE?) in empty bank
				cf.skip( 1 );
			}

			if ( g_bExtOut )
			{
				fputc( '\n', g_fout );
			}
		}
	}

	cf.Close();

	if ( g_bHasErrors )
		fprintf( stderr, "File may contain some errors!\n" );

	if ( g_fout != stdout )
		fclose( g_fout );

	fprintf( stderr, "Done!\n" );

	Vars_Destroy();

	return 0;
}

BOOL GenCode( CFile* pcf, DWORD dwStart, DWORD dwEnd )
{
	pcf->Seek( dwStart, SEEK_SET );

	//going thru the bank
	while( pcf->Tell() < (long)dwEnd )
	{
		//generating line
		if ( !GenLineCode( pcf, dwEnd ) )
		{
			g_bHasErrors = TRUE;
			return FALSE;
		}

		//output the correct line terminator
		if ( g_bAtariOut )
			fputc( 0x9B, g_fout );
		else
			fputc( '\n', g_fout );
	}

	return TRUE;
}

BOOL GenLineCode( CFile* pcf, DWORD dwEnd )
{
	DWORD dwLineStart = pcf->Tell();

	//some debug info
	if ( g_bVerbose )
		fprintf( g_fout, "[O:%08lX]", dwLineStart );

	//line number
	WORD wLineNum = pcf->readLEw();

	//end of line
	BYTE btLEnd = pcf->readb();
	DWORD dwLineEnd = dwLineStart + btLEnd;

	//don't print immediate line in BASIC XE extended (already filtered in
	//  other versions

	if( g_Type == BASIC_XEX )
	{
		if( wLineNum == 0x8000 )
		{
			pcf->Seek( dwLineEnd, SEEK_SET );
			return TRUE;
		}
	}

	fprintf( g_fout, "%d ", wLineNum );

	if ( g_bVerbose )
	{
		fprintf( g_fout, "[S:%04lX E:%04lX]", dwLineStart, dwLineEnd );
	}

	//if the line goes the end of buffer, croak
	if ( ( dwLineStart > dwEnd ) || ( dwLineEnd > dwEnd ) )
	{
		fprintf( g_fout, "***Line size mismatch. (%04lX-%04lX)\n", dwLineEnd, dwEnd );
		return FALSE;
	}

	//and generate the output for commands
	while( pcf->Tell() < (long)dwLineEnd )
	{
		if ( !GenCmdCode( pcf, dwLineStart, dwLineEnd ) )
			return FALSE;
	}

	return TRUE;
}

BOOL GenCmdCode( CFile* pcf, DWORD dwLineStart, DWORD dwLineEnd )
{
	//end of token
	BYTE btTEnd = pcf->readb();

	DWORD dwTokStart = pcf->Tell();
	DWORD dwTokEnd = dwLineStart + btTEnd;

	if ( g_bVerbose )
	{
		fprintf( g_fout, "[C:%04lX E:%04lX]", dwTokStart, dwTokEnd );
	}

	//could this ever happen?
	if ( dwTokStart >= dwTokEnd )
	{
		fprintf( g_fout, "***Command size problem!\n" );
		return FALSE;
	}

	//if token goes behind the eol
	if ( dwTokEnd > dwLineEnd )
	{
		fprintf( g_fout, "***Command size mismatch. (%04lX-%04lX)\n", dwTokEnd, dwLineEnd );
		return FALSE;
	}

	//which token
	BYTE btTok = pcf->readb();

	//token buffer
	char szTok[ 50 ];

	//if known token
	if ( btTok < g_iCmdNum ) 
	{
		//sub table for BXL extended commands
		if( ( btTok == BXL_EXTEND ) && ( g_Type == BASIC_XL ) )
		{
			BYTE btTok = pcf->readb();

			switch( btTok )
			{
				case 0x10:
					strcpy( szTok, "LOCAL" );
					break;

				case 0x11:
					strcpy( szTok, "EXIT" );
					break;

				case 0x12:
					strcpy( szTok, "PROCEDURE" );
					break;

				case 0x13:
					strcpy( szTok, "CALL" );
					break;

				case 0x14:
					strcpy( szTok, "SORTUP" );
					break;

				case 0x15:
					strcpy( szTok, "SORTDOWN" );
					break;

				default:
					sprintf( szTok, "EXTEND?%02X", btTok );
					g_bHasErrors = TRUE;
					break;

			}
		}
		else
			strcpy( szTok, g_aBasicCmds[ btTok ] );
	}
	else
	{
		sprintf( szTok, "COM%02X", btTok );
		fprintf( stderr, "Unknown command %02X!!!\n", btTok );
		g_bHasErrors = TRUE;
	}

	if ( *szTok )
	{
		//lower case for some dialects
		if( ( g_Type == BASIC_XL ) || ( g_Type == BASIC_XE ) || ( g_Type == BASIC_XEX ) )
		{
			strlwr( szTok + 1 );
		}

		fprintf( g_fout, "%s ", szTok );
	}

	//special handling for REM & DATA (first two statements)
	if ( btTok < 2 )
	{
		BYTE c;

		int iSecure = 0x100;

		for( ;; )
		{
			c = pcf->readb();

			if ( c == 0x9B )
				break;

			if ( g_bNoInverse )
				c &= 0x7F;

			fputc( c, g_fout );

			iSecure--;

			if( !iSecure )
			{
				fprintf( stderr, "Runaway in Rem/Data code!\n" );
				return FALSE;
			}
		} 

		return TRUE;

	}

	//generate operands for remain of command
	while( pcf->Tell() < (long)dwTokEnd )
	{
		if ( !GenOpsCode( pcf, dwTokEnd ) )
			return FALSE;
	}

	return TRUE;
}

BOOL GenOpsCode( CFile* pcf, DWORD dwTokEnd )
{
	//get operand
	BYTE btTok = pcf->readb();

	if ( btTok == OP_EOL )
	{
		//new line
		return TRUE;
	}

	if ( btTok == OP_NCONST )
	{
		//bcd num

		fprintf( g_fout, "%.10g", ReadAtariBCD( pcf ) );

		return TRUE;
	}

	if ( btTok == OP_NHCONST )
	{
		if ( g_Type == BASIC_TB )
		{
			//hex num TBASIC
			fprintf( g_fout, "$%X", (unsigned short)ReadAtariBCD( pcf ) );
			return TRUE;
		}

		if( ( g_Type == BASIC_XL ) || ( g_Type == BASIC_XE ) || ( g_Type == BASIC_XEX ) )
		{
			//hex num BASIC XL
			WORD wNum = (unsigned short)ReadAtariBCD( pcf );

			if( wNum > 0xFF )
				fprintf( g_fout, "$%04X", wNum );
			else
				fprintf( g_fout, "$%02X", wNum );

			return TRUE;
		}
	}

	if ( btTok == OP_SCONST )
	{
		//string
		BYTE c;

		BYTE btLen = pcf->readb();

		fprintf( g_fout, "\"" );
		while( btLen-- )
		{
			c = pcf->readb();

			if ( c == 0x9B )
			{
				break;
			}

			//is this for TBASIC only?
			if ( c == '\"' )
			{
				fprintf( g_fout, "\"\"" );
			}
			else
				fputc( c, g_fout );

		} 
		fprintf( g_fout, "\"" );
		return TRUE;
	}

	//and now for variables
	if ( btTok & 0x80 )
	{
		char szPom[ 255 ];

		Var_Get( szPom, btTok & 0x7F );

		if( ( g_Type == BASIC_XL ) || ( g_Type == BASIC_XE ) || ( g_Type == BASIC_XEX ) )
		{
			strlwr( szPom + 1 );
		}

		fprintf( g_fout, szPom );

		return TRUE;
	}

	//func or op
	if ( ( btTok < g_iOpFirst ) || ( btTok - g_iOpFirst + 1 > (BYTE)g_iOpsNum ) )
	{
		fprintf( g_fout, "UNKOP%02X", btTok );
		fprintf( stderr, "Unknown operand %02X!!!\n", btTok );
		g_bHasErrors = TRUE;
	}
	else
	{
		char szPom[ 100 ];
		strcpy( szPom, g_aBasicOps[ btTok - g_iOpFirst ] );

		if( ( g_Type == BASIC_XL ) || ( g_Type == BASIC_XE ) || ( g_Type == BASIC_XEX ) )
		{
			strlwr( szPom + 1 );
		}

		fprintf( g_fout, "%s", szPom );
	}

	return TRUE;

}

//code for reading Atari BCD format
double ReadAtariBCD( CFile* pcf )
{
	double dRes = 0;

	//reads exponent
	BYTE btExp = pcf->readb();

	int iExp;

	if ( !btExp )
	{
		//if Exp==0, we're skipping it! (silently!)
		iExp = 0;
		pcf->skip( 5 );
	}
	else
	{
		//compute exponent
		iExp = ( btExp - 68 ) * 2;

		int i = 5;

		//read 5 pairs of numbers and combine 'em
		while ( i-- )
		{
			BYTE btPom = pcf->readb();

			BYTE btNum = ( btPom >> 4 ) * 10 + ( btPom &0xf );

			dRes *= 100;
			dRes += btNum;
		}

	}

	dRes *= pow( 10, iExp );

	return dRes;
}

BOOL Vars_Load( CFile* pcf, WORD wVNT, WORD wVNTL, WORD wVVT, WORD NV )
{
	char szVarTemp[ 256 ];
	char szVarGen[ 256 ];

	char* pStr = NULL;

	BOOL bTog = FALSE;

	pcf->Seek( wVNT, SEEK_SET );

	for( int i = 0; i < wVNTL; i++ )
	{
		if ( !pStr )
			pStr = szVarTemp;

		char c = pcf->readb();

		if ( c & 0x80 )
		{
			c &= 0x7F;
			bTog = TRUE;
		}

		*( pStr++ ) = c;
		*pStr = '\0';

		//this is for correcting the runaway
		if ( pStr - szVarTemp > 254 )
		{
			*szVarTemp = '\0';
			bTog = TRUE;
		}

		if ( bTog )
		{
			int iVar = g_aVariables.GetSize();

			sprintf( szVarGen, "VAR%d", iVar );

			pStr = szVarTemp;

			int iLength = strlen( szVarTemp );

			if ( iLength )
			{
				for( int i = 0; i < iLength; i++ )
				{
					if ( !isprint( *( pStr++ ) ) )
					{
						strcpy( szVarTemp, szVarGen );
						break;
					}
				}

				if ( iVar )
				{
					for( int i = 0; i < iVar; i++ )
					{
						if( !strcmp( (char*)g_aVariables.GetAt( i ), szVarTemp ) )
						{
							fprintf( g_fout, "Dupped varname: %s\n", szVarTemp );
							strcpy( szVarTemp, szVarGen );
							break;
						}
					}
				}

			}
			else
			{
				strcpy( szVarTemp, szVarGen );
			}

			pStr = new char [ strlen( szVarTemp ) + 1 ];

			if ( pStr )
			{
				strcpy( pStr, szVarTemp );
				g_aVariables.Add( pStr );
			}
			else
			{
				printf( "Not enough memory!\n" );
				return FALSE;
			}

			bTog = FALSE;
			pStr = NULL;
		}
	}

	if ( g_bExtOut )
	{
		fprintf( g_fout, "Variable table:\n" );
	}

	pcf->Seek( wVVT, SEEK_SET );

	for( int i = 0 ; i < NV; i++ )
	{
		BYTE btType = pcf->readb();
		BYTE btNumber = pcf->readb();

		char* szType;

		char szText[ 50 ];

		switch( btType )
		{
			case VART_SCALAR:
			{
				szType = "SCALAR  ";
				sprintf( szText, "%.10g", ReadAtariBCD( pcf ) );
				break;
			}

			case VART_ARRAYU:
			{
				szType = "ARRAYu  ";
			  	char* szP = szText;
				szP += sprintf( szP, "SPoff: %04X ", pcf->readLEw() );
				szP += sprintf( szP, "Dim1: %d ", pcf->readLEw() );
				szP += sprintf( szP, "Dim2: %d ", pcf->readLEw() );
				break;
			}

			case VART_ARRAY:
			{
				szType = "ARRAY   ";
			  	char* szP = szText;
				szP += sprintf( szP, "SPoff: %04X ", pcf->readLEw() );
				szP += sprintf( szP, "Dim1: %d ", pcf->readLEw() );
				szP += sprintf( szP, "Dim2: %d ", pcf->readLEw() );
				break;
			}

			case VART_STRINGAU:
			{
				//BASIC_XL / XE
				szType = "STRINGAu";
			  	char* szP = szText;
				szP += sprintf( szP, "SPoff: %04X ", pcf->readLEw() );
				szP += sprintf( szP, "Dim1: %d ", pcf->readLEw() );
				szP += sprintf( szP, "Dim2: %d ", pcf->readLEw() );
				break;
			}

			case VART_STRINGA:
			{
				//BASIC_XL / XE
				szType = "STRINGA ";
			  	char* szP = szText;
				szP += sprintf( szP, "SPoff: %04X ", pcf->readLEw() );
				szP += sprintf( szP, "Dim1: %d ", pcf->readLEw() );
				szP += sprintf( szP, "Dim2: %d ", pcf->readLEw() );
				break;
			}

			case VART_STRINGU:
			{
				szType = "STRINGu ";
			  	char* szP = szText;
				szP += sprintf( szP, "SPoff: %04X ", pcf->readLEw() );
				szP += sprintf( szP, "Len: %d ", pcf->readLEw() );
				szP += sprintf( szP, "Dim: %d ", pcf->readLEw() );
				break;
			}

			case VART_STRING:
			{
				szType = "STRING  ";
			  	char* szP = szText;
				szP += sprintf( szP, "SPoff: %04X ", pcf->readLEw() );
				szP += sprintf( szP, "Len: %d ", pcf->readLEw() );
				szP += sprintf( szP, "Dim: %d ", pcf->readLEw() );
				break;
			}

			case VART_PROC:
			{
				//TBASIC
				szType = "PROC    ";
			  	char* szP = szText;
				for( int i = 0 ; i < 6; i++ )
				{
					szP += sprintf( szP, "%02X ", pcf->readb() );
				}
				break;
			}

			case VART_LABEL:
			{
				//TBASIC
				szType = "LABEL   ";
			  	char* szP = szText;
				for( int i = 0 ; i < 6; i++ )
				{
					szP += sprintf( szP, "%02X ", pcf->readb() );
				}
				break;
			}

			default:
			{
				szType = "UNKNOWN ";
			  	char* szP = szText;
				for( int i = 0 ; i < 6; i++ )
				{
					szP += sprintf( szP, "%02X ", pcf->readb() );
				}
				g_bHasErrors = TRUE;
			}
		}

		char szName[ 255 ];
		Var_Get( szName, btNumber );

		char cLastChar = szName[ strlen( szName ) - 1 ];

		switch( btType )
		{
			case VART_STRING:
			case VART_STRINGU:
			{
				if ( cLastChar != '$' )
				{
					strcat( szName, "$" );
					char* szStr = new char [ strlen( szName ) + 1 ];
					strcpy( szStr, szName );
					char* szOld = (char*) g_aVariables.GetAt( btNumber );

					if ( szOld )
						delete [] szOld;

					g_aVariables.SetAt( btNumber, szStr );

				}

				break;
			}

			case VART_ARRAY:
			case VART_ARRAYU:
			{
				if ( cLastChar == '(' )
				{
					szName[ strlen( szName ) - 1 ] = '\0';
					char* szStr = new char [ strlen( szName ) + 1 ];
					strcpy( szStr, szName );
					char* szOld = (char*) g_aVariables.GetAt( btNumber );

					if ( szOld )
						delete [] szOld;

					g_aVariables.SetAt( btNumber, szStr );

				}

				break;
			}

		}

		if ( g_bExtOut )
		{
			fprintf( g_fout, "%04X %s (%02X) %02X: %s %s\n",
					i+1, 
					szType, 
					btType, 
					btNumber, 
					szText,
					szName );
		}

	}

	if ( g_bExtOut )
		fputc( '\n', g_fout );

	return TRUE;
}

void Var_Get( char* szRet, int iNum )
{
	char* szVar = (char*) g_aVariables.GetAt( iNum );

	if ( szVar )
		strcpy( szRet, szVar );
	else
		sprintf( szRet, "VAR%d", iNum );
}

void Vars_Destroy()
{
	for( int i = 0; i < g_aVariables.GetSize(); i++ )
	{
		if ( g_aVariables.GetAt( i ) )
			delete [] ( char* ) g_aVariables.GetAt( i );
	}
}

