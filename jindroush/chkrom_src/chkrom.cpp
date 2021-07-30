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

//#include <unistd.h>
#include "pub.def"
#include "wintypes.h"
#include "at_dis.h"

BOOL g_bDisassemble = FALSE;

int hIn = -1;

char* g_szInfile = NULL;

#define SHEADER PRG_NAME " v" PRG_VERSION " (c) " PRG_COPYRIGHT " " PRG_AUTHOR "\n"

#define HEADER SHEADER \
   PRG_DESC "\n" \
	"  Latest version can be found at " PRG_URL "\n" \
	"  Published under GPL. See GPL.TXT.\n\n"

#define USAGE HEADER "Usage:  " PRG_NAME " " PRG_USAGE

#include "switches.cpp"

int main (int argc,char *argv[])
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

	fprintf( stderr, SHEADER );

	g_szInfile = argv[ 1 ];

	hIn = open( g_szInfile, O_RDONLY | O_BINARY );

	if ( hIn == -1 )
	{
		fprintf(stderr, "Unable to open file\n\n%s", g_szInfile );
		exit( 1 );
	}

	LONG lFileLen = lseek( hIn, 0, SEEK_END );

	lseek( hIn, 0, SEEK_SET );

	if ( ( lFileLen != 0x2000 ) && ( lFileLen != 0x4000 ) )
	{
		printf( "%s: Unknown ROM size\n", g_szInfile );
		close( hIn );
		exit( 1 );
	}

	printf( "ROM file:  %s\n", g_szInfile );

	BYTE pbtMem[ 0x4000 ];

	read( hIn, pbtMem, lFileLen );

	WORD wInit = pbtMem[ lFileLen - 2 ] + ( pbtMem[ lFileLen - 1 ] << 8 );
	WORD wFlags = pbtMem[ lFileLen - 4 ] + ( pbtMem[ lFileLen - 3 ] << 8 );
	WORD wRun = pbtMem[ lFileLen - 6 ] + ( pbtMem[ lFileLen - 5 ] << 8 );

	WORD wStart = ( lFileLen == 0x2000 ) ? 0xA000 : 0x8000;

	printf( "Init Addr :%04X\n", wInit );
	printf( "Run Addr  :%04X\n", wRun );
	printf( "Flags     :%04X ( ", wFlags );

	if ( wFlags & 0x0400 )
	{
		printf( "INIT&RUN " );
	}
	else
		printf( "INIT " );

	if ( wFlags & 0x0100 )
		printf( "BOOT " );

	if ( wFlags & 0x8000 )
		printf( "TESTMOD " );

	printf( ")\n" );

	if ( g_bDisassemble )
	{
		printf( "\n" );
		OutputBlockDiss( pbtMem, wStart, wStart + lFileLen - 1 - 6 );
		printf( "\n" );
	}

	printf( "Ok!\n" );

	close( hIn );

	return 0;
}

