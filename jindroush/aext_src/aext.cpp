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

int g_iBlkEnd = 0;
int g_iBlkStart = 0;
int g_iBlkLength = 0;
int g_iBlkBase = 0;

char szStart[ MAX_PATH ];
char szEnd[ MAX_PATH ];
char szLength[ MAX_PATH ];

BOOL g_bStartUsed = FALSE;
BOOL g_bEndUsed = FALSE;
BOOL g_bLenUsed = FALSE;

int g_bOutputIsAtari = FALSE;
int g_bUseBase = FALSE;

#define SHEADER PRG_NAME " v" PRG_VERSION " (c) " PRG_COPYRIGHT " " PRG_AUTHOR "\n"

#define HEADER SHEADER \
   PRG_DESC "\n" \
	"  Latest version can be found at " PRG_URL "\n" \
	"  Published under GPL. See GPL.TXT.\n\n"

#define USAGE HEADER "Usage:  " PRG_NAME " " PRG_USAGE

#include "switches.h"

int ToNum( char*, int );

int main( int argc, char *argv[] )
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

	char* szInfile = argv[ 1 ];

	char szOutfile[ MAX_PATH ];

	if ( argc >= 3 )
	{
		strcpy( szOutfile, argv[ 2 ] );
	}
	else
	{
		strcpy( szOutfile, "prg.axe" );
	}

	int hIn = open( szInfile, O_RDONLY | O_BINARY );

	if ( hIn == -1 )
	{
		fprintf( stderr, "Can't open infile '%s'!\n", szInfile );
		return -1;
	}

	if( g_bStartUsed )
	{
		g_iBlkStart = ToNum( szStart, 1 );
	}

	if( g_bEndUsed )
	{
		g_iBlkEnd = ToNum( szEnd, 1 );
	}

	if( g_bLenUsed )
	{
		g_iBlkLength = ToNum( szLength, 0 );
	}

	LONG lLen = lseek( hIn, 0, SEEK_END );
	lseek( hIn, 0, SEEK_SET );

	if( g_bEndUsed && g_iBlkEnd < 1 )
	{
		g_iBlkEnd = lLen - 1 + g_iBlkEnd;
	}

	if( g_bStartUsed )
	{
		if ( g_bEndUsed )
		{
			g_iBlkLength = g_iBlkEnd - g_iBlkStart + 1;
		}
		else
		{
			//START & ?
			if ( g_bLenUsed )
			{
				//start & length
				g_iBlkEnd = g_iBlkStart + g_iBlkLength - 1;		
			}
			else
			{
				//start & nothing
				g_iBlkEnd = lLen - 1;
				g_iBlkLength = g_iBlkEnd - g_iBlkStart + 1;
			}
		}

	}
	else if( g_bEndUsed )
	{
		if( g_bLenUsed )
		{
			//end & length
			g_iBlkStart = g_iBlkEnd - g_iBlkLength + 1;
		}
		else
		{
			//end only
			g_iBlkStart = 0;
			g_iBlkLength = g_iBlkEnd - g_iBlkStart + 1;
		}
	}
	else
	{
		if( g_bLenUsed )
		{
			//length only
			g_iBlkStart = 0;
			g_iBlkEnd = g_iBlkStart + g_iBlkLength - 1;		
		}
		else
		{
			//nothing
			g_iBlkStart = 0;
			g_iBlkLength = lLen;
			g_iBlkEnd = g_iBlkStart + g_iBlkLength - 1;		
		}
	}

	if( ( g_iBlkStart < 0 ) || ( g_iBlkStart >= lLen ) )
	{
		fprintf( stderr, "Invalid start ptr!\n" );
		close( hIn );
		return -1;
	}

	if( ( g_iBlkEnd < 0 ) || ( g_iBlkEnd >= lLen ) || ( g_iBlkEnd < g_iBlkStart ) )
	{
		fprintf( stderr, "Invalid end ptr!\n" );
		close( hIn );
		return -1;
	}

	int iBlkStartM = g_iBlkStart;
	int iBlkEndM = g_iBlkEnd;

	if( g_bUseBase )
	{
		iBlkStartM = g_iBlkBase;
		iBlkEndM = g_iBlkBase + g_iBlkLength - 1;
	}

	if ( ( ( iBlkEndM >= 0x10000 ) || ( iBlkStartM >= 0x10000 ) ) && g_bOutputIsAtari )
	{
		fprintf( stderr, "End goes past end of RAM in real Atari!\n" );
		close( hIn );
		return -1;
	}

	int hOut = open( szOutfile, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE );

	if ( hOut == -1 )
	{
		fprintf( stderr, "Can't open outfile '%s'!\n", szOutfile );
		close( hIn );
		return -1;
	}

	#define BUFLEN 0x10000

	BYTE* pbtPtr = new BYTE [ BUFLEN ];
	lseek( hIn, g_iBlkStart, SEEK_SET );

	if ( g_bOutputIsAtari )
	{
		WORD wStart = iBlkStartM;
		WORD wEnd = iBlkEndM;

		BYTE btPom = 0xFF;
		write( hOut, &btPom, 1 );
		write( hOut, &btPom, 1 );

		btPom = wStart & 0xFF;
		write( hOut, &btPom, 1 );

		btPom = wStart >> 8;
		write( hOut, &btPom, 1 );

		btPom = wEnd & 0xFF;
		write( hOut, &btPom, 1 );

		btPom = wEnd >> 8;
		write( hOut, &btPom, 1 );
	}

	int iLeft = g_iBlkLength;

	while( iLeft )
	{
		int iReadNow = iLeft > BUFLEN ? BUFLEN : iLeft;
		read( hIn, pbtPtr, iReadNow );
		write( hOut, pbtPtr, iReadNow );
		iLeft -= iReadNow;
	}

	close( hOut );
	close( hIn );

	printf( "Done!\n" );

	return 0;
}

int ToNum( char* szString, int iOffs )
{
	BOOL bSec = FALSE;

	if( *szString == 's' )
	{
		bSec = TRUE;
		szString++;
	}

	int iNum;
	sscanf( szString, "%X", &iNum );

	if( bSec )
	{
		if( iNum < iOffs )
		{
			printf( "Incorrect sector number %s\n", szString );
			return -1;
		}

		iNum -= iOffs;
		iNum *= 0x80;

		if( iOffs )
		{
			//begin & end
			iNum += 0x10;
		}

		//whole code assumes SD ATR file!!!
	}

	return iNum;
}
