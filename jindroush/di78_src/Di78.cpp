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

#include "pub.def"

#include "wintypes.h"

#include "CDis.h"
#include "cfile.h"

BYTE *g_pbtPool;
LONG g_lFileLen;
BOOL g_bDumpSymbols = FALSE;
BOOL g_bDbOnly = FALSE;
BOOL g_bMainOnly = FALSE;
BOOL g_bFullCart = FALSE;

int g_iMemClassAllow = A7_DEFAULT;
int g_iMemClassDisallow = 0;

DISS_TYPE g_diss = DISS_PLAIN;
UNDOC_TYPE g_undoc = UNDOC_NONE;

char* g_szInfile = NULL;

CDis g_dis;

#define SHEADER PRG_NAME " v" PRG_VERSION " (c) " PRG_COPYRIGHT " " PRG_AUTHOR "\n"

#define HEADER SHEADER \
	PRG_DESC "\n" \
	"  Latest version can be found at " PRG_URL "\n" \
	"  Published under GPL. See GPL.TXT.\n\n"

#define USAGE HEADER "Usage:  " PRG_NAME " " PRG_USAGE

#include "switches.cpp"

typedef enum
{
	BANKING_NONE,
	BANKING_ATARI,
	BANKING_ATARIp16,
	BANKING_ABSOLUTE,
	BANKING_ACTIVISION
} BANKING_TYPE;

void DisMain( BYTE*, WORD, BOOL );
void DisBank( BYTE* pbtBlock, int iBank, WORD wMainOffs, WORD wEnd );

void print_byte( BYTE* pbt, int iLen )
{
	printf( "\t\t.BYTE\t" );

	while( iLen-- )
	{
		if( iLen )
			printf( "$%02X, ", *pbt );
		else
			printf( "$%02X\n", *pbt );

		pbt++;
	}
}

int main (int argc,char *argv[])
{
	setbuf( stdout, NULL );
	setbuf( stderr, NULL );

	if ( !SWITCHES_Init( &argc, argv ) )
		return 1;

	if ( ( argc < 2 ) )
	{
		SWFN_HELP( USAGE );
		return 1;
	}

	fprintf( stderr, SHEADER );

	//g_szInfile = "x.a78";

	g_szInfile = argv[ 1 ];

	CFile cfIn;

	if ( !cfIn.Open( g_szInfile ) )
	{
		fprintf( stderr, "Unable to open file\n\n%s", g_szInfile );
		return -1;
	}

	g_iMemClassAllow &= ~g_iMemClassDisallow;

	//todo
	g_dis.Init( g_diss, PROC_6502_PLAIN, g_undoc, g_iMemClassAllow );

	if( g_bFullCart && g_diss != DISS_DASM )
		g_bFullCart = FALSE;

	if( g_bFullCart )
		g_bMainOnly = FALSE;

	if ( g_bDumpSymbols )
		g_dis.DumpSymbols();

	BYTE pbtHeader[ 0x80 ];
	cfIn.Read( &pbtHeader, 0x80 );

	char* szHdr1 = "\1ATARI7800";
	char* szHdr2 = "\2ATARI7800";

	int iVersion = 0;
	BOOL bNTSC = TRUE;
	BOOL bHasPokey = 0;
	BOOL bHasRam = 0;

	BANKING_TYPE BankingType = BANKING_NONE;

	if( ! memcmp( pbtHeader, szHdr1, 10 ) )
	{
		iVersion = 1;
	}
	else if( ! memcmp( pbtHeader, szHdr2, 10 ) )
	{
		iVersion = 2;
	}

	WORD wRawType = ( pbtHeader[ 53 ] ) << 8 | pbtHeader[ 54 ];
	bHasPokey = ( wRawType & 1 ) ? TRUE : FALSE;

	if( !iVersion )
	{
		fprintf( stderr, "Bad or unknown A78 header\n" );
		return -1;
	}

	LONG lLength = cfIn.GetLength() - 128;
	LONG lHLength = pbtHeader[52] |
							( pbtHeader[ 51 ] << 8 ) |
							( pbtHeader[ 50 ] << 16 ) |
							( pbtHeader[ 49 ] << 24 );

	if( lLength != lHLength )
	{
		fprintf( stderr, "Length mismatch (h: %ld != f: %ld)\n",
									lHLength,
									lLength );
		return -1;
	}

	BYTE* pbtBuff = new BYTE [ lLength ];
	cfIn.Read( pbtBuff, lLength );
	cfIn.Close();

	g_dis.PrintRem();
	printf( "This disassembly created by " SHEADER "\n" );

	g_dis.PrintDirectives();

	g_dis.PrintRem();
	printf( "Filename: %s\n", g_szInfile );

	g_dis.PrintRem();
	printf( "Cart name: %.32s\n", pbtHeader + 17 );
 
	if( iVersion == 2 && pbtHeader[ 57 ] )
		bNTSC = FALSE;

	bHasRam = ( wRawType & 0x4 ) ? TRUE : FALSE;

	if( wRawType & 0x02 )
	{
		BankingType = BANKING_ATARI;
	}

	if( ( wRawType & 0x0A ) == 0x0a )
	{
		BankingType = BANKING_ATARIp16;
	}

	if( wRawType == 0x0100 )
		BankingType = BANKING_ACTIVISION;

	if( wRawType == 0x0200 )
		BankingType = BANKING_ABSOLUTE;

	g_dis.PrintRem();
	printf( "Raw type: %04X\n", wRawType );

	g_dis.PrintRem();
	printf( "Has Pokey: %s\n", bHasPokey ? "yes" : "no" );

	g_dis.PrintRem();
	printf( "Has RAM: %s\n", bHasRam ? "yes" : "no" );

	g_dis.PrintRem();
	printf( "TV norm: %s\n", bNTSC ? "NTSC" : "PAL" );
 
	g_dis.PrintRem();
	printf( "Banking type: " );
	switch( BankingType )
	{
		case BANKING_NONE:
			printf( "none" );
			break;

		case BANKING_ATARI:
			printf( "Atari supercart" );
			break;

		case BANKING_ATARIp16:
			printf( "Atari supercart (+ROM at $4000)" );
			break;

		case BANKING_ABSOLUTE:
			printf( "Absolute supercart" );
			break;

		case BANKING_ACTIVISION:
			printf( "Activision supercart" );
			break;

		default:
			printf( "???" );
			break;
	}

	printf( "\n\n" );

	if( g_bFullCart )
	{
		printf( "\t\tORG\t$0000\n" );
	 	printf( "\t\t.BYTE\t$%02X, \"ATARI7800\"\n", pbtHeader[ 0 ] );
	 	printf( "\t\tDS.B\t7, $%02X\n", pbtHeader[ 0x0a ] );

		char szName[ 33 ];
		memcpy( szName, pbtHeader + 17, 32 );
		szName[ 32 ] = '\0';

		printf( "\t\t.BYTE\t\"%s\"\n", szName );

		if( strlen( szName ) < 32 )
		{
			printf( "\t\tDS.B\t%d, $%02X\n", 32 - strlen( szName ), pbtHeader[ 17 + strlen( szName ) ] );
		}

		print_byte( pbtHeader + 0x31, 4 );
		print_byte( pbtHeader + 0x35, 2 );
		print_byte( pbtHeader + 0x37, 1 );
		print_byte( pbtHeader + 0x38, 1 );
		print_byte( pbtHeader + 0x39, 1 );

		printf( "\t\tDS.B\t$2A, 0\n" );

		printf( "\t\t.BYTE\t\"ACTUAL CART DATA STARTS HERE\"\n\n" );
	}
	else if( g_diss == DISS_DASM )
	{
		printf( "\t\tORG\t$0000\n" );
	}

	BYTE* pbtMainBank = NULL;
	WORD wMainOffs = 0;

	switch( BankingType )
	{
		case BANKING_ATARI:
		{
			pbtMainBank = pbtBuff + lLength - 0x4000;
			wMainOffs = 0xc000;


			if( ! g_bMainOnly )
			{
				int iBanks = ( lLength / 0x4000 ) - 1;

				for( int i = 0; i < iBanks; i++ )
					DisBank( pbtBuff + i * 0x4000, i, 0x8000, 0xbFFF );

				if( ! bHasPokey && ! bHasRam )
					DisBank( pbtBuff + 6 * 0x4000, 6, 0x4000, 0x7FFF );
			}

			break;
		}

		case BANKING_ATARIp16:
		{
			pbtMainBank = pbtBuff + lLength - 0x4000;
			wMainOffs = 0xc000;

			if( ! g_bMainOnly )
			{
				int iBanks = ( lLength / 0x4000 ) - 1;

				DisBank( pbtBuff, 0, 0x4000, 0x7FFF );

				for( int i = 1; i < iBanks; i++ )
					DisBank( pbtBuff + i * 0x4000, i, 0x8000, 0xbFFF );
			}

			break;
		}

		case BANKING_NONE:
		{
			pbtMainBank = pbtBuff;
			wMainOffs = 0x10000 - lLength;
			break;
		}

		case BANKING_ABSOLUTE:
		{
			if( lLength != 0x10000 )
			{
				fprintf( stderr, "Incorrect length!\n" );
				return -1;
			}

			pbtMainBank = pbtBuff + 0x8000;
			wMainOffs = 0x8000;

			if( !g_bMainOnly )
			{
				g_dis.PrintRem();
				printf( "Bank 0:\n" );
				g_dis.PrintBlockHeader( 0x4000, 0x7FFF );
				g_dis.OutputBlockDiss( pbtBuff + 0x0000, 0x4000, 0x7FFF );
				printf( "\n" );

				g_dis.PrintRem();
				printf( "Bank 1:\n" );
				g_dis.PrintBlockHeader( 0x4000, 0x7FFF );
				g_dis.OutputBlockDiss( pbtBuff + 0x4000, 0x4000, 0x7FFF );
				printf( "\n" );
			}
			break;
		}

		case BANKING_ACTIVISION:
		{
			pbtMainBank = pbtBuff + 0x1c000;
			wMainOffs = 0xE000;

			BYTE* pb2 = new BYTE[ 0x6000 ];

			memcpy( pb2 + 0x0000, pbtBuff + 0x1a000, 0x2000 );
			memcpy( pb2 + 0x2000, pbtBuff + 0x18000, 0x2000 );
			memcpy( pb2 + 0x4000, pbtBuff + 0x1E000, 0x2000 );

			g_dis.PrintRem();
			printf( "Bank 6+7:\n" );
			g_dis.PrintBlockHeader( 0x4000, 0x9FFF );
			g_dis.OutputBlockDiss( pb2, 0x4000, 0x9FFF );
			printf( "\n" );

			delete[] pb2;

			if( ! g_bMainOnly )
			{
				for( int i = 0; i < 6; i++ )
					DisBank( pbtBuff + i * 0x4000, i, 0xA000, 0xCFFF );
			}
			break;
		}

		default:
		{
			printf( "Not yet supported!\n" );
			delete [] pbtBuff;
			return 1;
		}
	}

	DisMain( pbtMainBank, wMainOffs, bNTSC );

	delete [] pbtBuff;
	return 0;
}

void DisMain( BYTE* pbtBlock, WORD wMainOffs, BOOL bNTSC )
{
	g_dis.PrintRem();
	printf( "Main bank\n" );
	g_dis.PrintBlockHeader( wMainOffs, 0xFFFF );

	if( bNTSC )
	{
		g_dis.OutputBlockDiss( pbtBlock, wMainOffs, 0xFF7F );
		printf( "\n" );
		g_dis.PrintRem();
		printf( "Signature:\n" );
		g_dis.OutputBlockDBs( pbtBlock - wMainOffs + 0xff80, 0xFF80, 0xFFF7, 0x8 );
	}
	else
	{
		g_dis.OutputBlockDiss( pbtBlock, wMainOffs, 0xFFF7 );
	}

	WORD wFlags = pbtBlock[ 0xfff8 - wMainOffs ] + ( pbtBlock[ 0xfff9 - wMainOffs ] << 8 );
	WORD wNMI = pbtBlock[ 0xfffA - wMainOffs ] + ( pbtBlock[ 0xfffb - wMainOffs ] << 8 );
	WORD wMain = pbtBlock[ 0xfffC - wMainOffs ] + ( pbtBlock[ 0xfffd - wMainOffs ] << 8 );
	WORD wIRQ = pbtBlock[ 0xfffE - wMainOffs ] + ( pbtBlock[ 0xffff - wMainOffs ] << 8 );

	g_dis.PrintRem();
	printf( "Flags\n" );
	g_dis.PrintWord( wFlags );

	g_dis.PrintRem();
	printf( "NMI\n" );
	g_dis.PrintWord( wNMI );

	g_dis.PrintRem();
	printf( "Main\n" );
	g_dis.PrintWord( wMain );

	g_dis.PrintRem();
	printf( "IRQ\n" );
	g_dis.PrintWord( wIRQ );

	printf( "\n" );
}

void DisBank( BYTE* pbtBlock, int iBank, WORD wMainOffs, WORD wEnd )
{
	g_dis.PrintRem();
	printf( "Bank %d:\n", iBank );
	g_dis.PrintBlockHeader( wMainOffs, wEnd );

	g_dis.OutputBlockDiss( pbtBlock, wMainOffs, wEnd );

	printf( "\n" );
}
