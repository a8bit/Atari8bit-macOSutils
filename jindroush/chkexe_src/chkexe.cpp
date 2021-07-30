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

#include "at_dis.h"
#include "cfile.h"

#define AT_HEADER		0xFFFF

#define AT_RUNL		0x02E0
#define AT_RUNH		0x02E1
#define AT_RUNPREV	( AT_RUNL - 1 )

#define AT_INITL		0x02E2
#define AT_INITH		0x02E3
#define AT_INITPAST	( AT_INITH + 1 )

#define AT_HWSTART	0xD000
#define AT_HWEND		0xD7FF

typedef struct block
{
	struct block *pNext;
	WORD wBlkStart;
	WORD wBlkEnd;
} FILE_BLOCK;


BOOL BlockRead( CFile&, CFile& );
void BlockInsert( WORD, WORD );
void BlocksWrite( CFile& );
void PrintOffset( CFile& );
void BlockDisassemble( WORD, WORD );

FILE_BLOCK *g_pBlocks = NULL;
BYTE *g_pbtPool;
BOOL g_bHasRunAddress = FALSE;
LONG g_lFileLen;
BOOL g_bDisassemble = FALSE;
BOOL g_bSilent = FALSE;
BOOL g_bIsNotOk = TRUE;
BOOL g_bSplit = FALSE;
BOOL g_bOut = FALSE;

WORD g_iSplitNum = 0;

WORD g_wGapFill = 0;

char* g_szInfile = NULL;
char* g_szOutfile = NULL;

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

	if ( ( argc < 2 ) )
	{
		SWFN_HELP( USAGE );
		return 1;
	}

	fprintf( stderr, SHEADER );

	g_szInfile = argv[ 1 ];

	if ( argc == 3 )
		g_szOutfile = argv[ 2 ];

	CFile cfIn;
	CFile cfOut;

	if ( !cfIn.Open( g_szInfile ) )
	{
		fprintf( stderr, "Unable to open file\n\n%s", g_szInfile );
		return -1;
	}

	if ( AT_HEADER != cfIn.readLEw() )
	{
		printf( "%s: Not an Atari 8-bit binary load format file\n", g_szInfile );
		cfIn.Close();
		return -1;
	}

	if ( g_bSilent )
		printf( "%s: ", g_szInfile );
	else
		printf( "Binary file:  %s\n", g_szInfile );

	if ( argc > 2 )
	{
		if ( !cfOut.Create( g_szOutfile ) )
		{
			fprintf( stderr, "Unable to open file\n\n%s", g_szOutfile );
			return -1;
		}
		g_bOut = TRUE;
	}

	if ( g_bOut )
	{
		cfOut.writeLEw( AT_HEADER );
	}

	g_pbtPool = new BYTE [ 0x10000 ];
	memset( g_pbtPool,0, 0x10000 );

	if ( !g_pbtPool )
	{
		fprintf( stderr, "Unable to allocate memory for address space.\n" );
		return -1;
	}

	g_lFileLen = cfIn.GetLength();

	cfIn.Seek( 2, SEEK_SET );

	while ( cfIn.Tell() < g_lFileLen )
	{
		if ( ( g_bIsNotOk = !BlockRead( cfIn, cfOut ) ) )
			break;
	}

	cfIn.Close();

	if ( g_bHasRunAddress )
		BlockInsert( AT_RUNL, AT_RUNH );

	if ( g_bOut )
	{
		BlocksWrite( cfOut ); // Write all blocks since last init
		cfOut.Close();
	}

	delete [] g_pbtPool;

	if ( !g_bIsNotOk )
		printf( "Ok!\n" );

	return 0;
}


BOOL BlockRead( CFile& cfIn, CFile& cfOut )
{
	WORD wBlkStart;
	WORD wBlkEnd;
	WORD wBlkLen;

	do
	{
		PrintOffset( cfIn );

		if ( cfIn.Tell() >= g_lFileLen )
		{
			printf( "Error:  Unexpected end of file\n" );
			return FALSE;
		}

		wBlkStart = cfIn.readLEw();

		if ( wBlkStart == AT_HEADER )
		{
			if ( !g_bSilent )
				printf( "     :  Unexpected second 0xffff format header\n" );
		}

	} while ( wBlkStart == AT_HEADER );

	if ( cfIn.Tell() >= g_lFileLen )
	{
		printf( "Error:  Unexpected end of file in load range start address\n" );
		return FALSE;
	}

	wBlkEnd = cfIn.readLEw();

	wBlkLen = wBlkEnd - wBlkStart + 1;

	if ( cfIn.Tell() >= g_lFileLen )
	{
		printf( "Error:  Unexpected end of file in load range end address\n" );
		return FALSE;
	}

	if ( ( wBlkStart == wBlkEnd ) && ( ( wBlkStart >> 8 ) == ( wBlkStart & 0xFF ) ) )
	{
		WORD wLen = g_lFileLen - cfIn.Tell() + 4;

		//garbage was limited to be max $500 long, run address must exist!
		if ( ( wLen < 0x500 ) && ( wBlkStart ) )
		{
			printf( "Error:  Apparent garbage fill at end of file (%04X bytes)\n", wLen );
			return FALSE;
		}

	}

	if ( wBlkEnd < wBlkStart )
	{
		if ( !g_bSilent )
			printf( "Block:  %04X-%04X (%04X)\n", wBlkStart, wBlkEnd, wBlkLen );

		printf( "Error:  %ld bytes in file after invalid load range\n", g_lFileLen - cfIn.Tell() );
		return FALSE;
	}

	if ( g_lFileLen < cfIn.Tell() + wBlkLen )
	{
		if ( !g_bSilent )
			printf( "Block:  %04X-%04X (%04X)\n", wBlkStart, wBlkEnd, wBlkLen );

		printf( "Error:  Truncated file:  missing data in load block (%04lX bytes missing)\n", cfIn.Tell() + wBlkLen - g_lFileLen );
		return FALSE;
	}

	// Read in the data for this block
	cfIn.Read( &g_pbtPool[wBlkStart], wBlkLen );

	// Check for run address
	if ( wBlkStart <= AT_RUNH && wBlkStart + wBlkLen >= AT_RUNL )
	{
		if (wBlkStart==AT_RUNH || wBlkStart+wBlkLen==AT_RUNL)
		{
			if ( !g_bSilent )
				printf( "     :  Partial run address\n" );
		}
		if ( g_bHasRunAddress )
			if ( !g_bSilent )
				printf( "     :  Unexpected second run address\n" );

		g_bHasRunAddress = TRUE;

		if ( !g_bSilent )
			printf( "Run  :  %04X\n", g_pbtPool[ AT_RUNL ] + 256 * g_pbtPool[ AT_RUNH ] );

		if ( ( wBlkStart >= AT_RUNL ) && ( wBlkEnd <= AT_RUNH ) )
			return TRUE;

		// Other data in this block
		if ( wBlkStart == AT_RUNL || wBlkStart == AT_RUNH )
		{
			// Run and init in the same block--split
			wBlkStart = AT_INITL;
			wBlkLen = wBlkEnd - wBlkStart + 1;
		}
		else if ( wBlkEnd == AT_RUNL || wBlkEnd==AT_RUNH)
		{
			// other stuff before the run address--split
			wBlkEnd = AT_RUNPREV;
			wBlkLen = wBlkEnd - wBlkStart + 1;
		}
		else
		{
			// Run address in the middle of the block
			if ( !g_bSilent )
				printf( "Block:  %04X-%04X (%04X)\n", wBlkStart, AT_RUNPREV, AT_RUNPREV - wBlkStart + 1 );

			BlockInsert( wBlkStart, AT_RUNPREV );
			wBlkStart = AT_INITL;
			wBlkLen = wBlkEnd - wBlkStart + 1;
		}
	}

	// Check for init address
	// We know there's nothing before the address in the block,
	// as we would have split it off above as a run address.
	if ( wBlkStart <= AT_INITH && wBlkStart + wBlkLen >= AT_INITL )
	{
		if ( wBlkStart == AT_INITH || wBlkStart + wBlkLen == AT_INITL )
		{
			if ( !g_bSilent )
				printf( "     :  Partial init address\n" );
		}
		if ( !g_bSilent )
			printf( "Init :  %04X\n", g_pbtPool[ AT_INITL ] + 256 * g_pbtPool[ AT_INITH ] );

		// Other data in this block?
		if ( wBlkEnd > AT_INITH )
		{
			// More stuff past init--split
			if ( !g_bSilent )
				printf( "Block:  %04X-%04X (%04X)\n", AT_INITPAST, wBlkEnd, wBlkLen );

			BlockInsert( AT_INITPAST, wBlkEnd );
			wBlkEnd = AT_INITH;
			wBlkLen = wBlkEnd - wBlkStart + 1;
		}

		BlockInsert( wBlkStart, wBlkEnd );

		if ( g_bDisassemble )
			BlockDisassemble( wBlkStart, wBlkEnd );

		// Write everything out to avoid cross-init merges
		BlocksWrite( cfOut );
		return TRUE;
	}

	// Print data block load message
	if ( !g_bSilent )
		printf( "Block:  %04X-%04X (%04X)\n", wBlkStart, wBlkEnd, wBlkLen );

	if ( ( !wBlkStart ) && ( !wBlkEnd ) )
		return TRUE;

	BlockInsert( wBlkStart, wBlkEnd );

	if ( g_bDisassemble )
		BlockDisassemble( wBlkStart, wBlkEnd );

	return TRUE;
}


void BlockInsert(WORD wBlkStart,WORD wBlkEnd)
{
	if ( g_bSplit )
	{
		char szPom[ 255 ];
		sprintf( szPom, "blk%04X.blk", g_iSplitNum );

		CFile cf;
		if ( cf.Create( szPom ) )
		{
			cf.writeLEw( AT_HEADER );
			cf.writeLEw( wBlkStart );
			cf.writeLEw( wBlkEnd );
			cf.Write( g_pbtPool + wBlkStart, wBlkEnd - wBlkStart + 1 );
			cf.Close();
		}

		g_iSplitNum++;
	}

	FILE_BLOCK *b,*bp;

	bp = NULL; // previous block
	b = g_pBlocks;

	while( b )
	{
		//this fills the ultra short blks
		if ( ( ( b->wBlkEnd + 1 ) >= 0x400 ) && ( b->wBlkStart < 0xC000 ) )
		{
			if ( ( b->wBlkEnd + 1 ) < wBlkStart )
			{
				if ( ( wBlkStart - ( b->wBlkEnd + 1 ) ) < g_wGapFill )
				{
					if ( !g_bSilent )
						printf( "\t\tBlock merges with a previous block (gap fill)\n" );

					b->wBlkEnd = wBlkEnd;
					return;
				}
			}

		}

		// Check for merge
		if ( b->wBlkEnd + 1 == wBlkStart )
		{
			if ( !g_bSilent )
				printf( "\t\tBlock merges with a previous block\n" );

			b->wBlkEnd = wBlkEnd;
			return;
		}
		if ( b->wBlkStart - 1 == wBlkEnd )
		{
			if ( !g_bSilent )
				printf( "\t\tBlock merges with a previous block (unexpected ordering)\n" );

			b->wBlkStart = wBlkStart;
			return;
		}
		// Check for overlap
		if ( b->wBlkStart <= wBlkStart && wBlkEnd <= b->wBlkEnd )
		{
			if ( !g_bSilent )
				printf( "\t\tWarning:  Completely overlaps a previous block--merged\n" );

			return;
		}
		if (b->wBlkStart <= wBlkEnd && b->wBlkStart >= wBlkStart)
		{
			b->wBlkStart = wBlkStart;
			if (b->wBlkEnd<wBlkEnd) b->wBlkEnd = wBlkEnd;
			if ( !g_bSilent )
				printf( "\t\tWarning:  Partially overlaps a previous block--merged\n" );

			return;
		}
		if ( b->wBlkEnd <= wBlkEnd && b->wBlkEnd >= wBlkStart )
		{
			b->wBlkEnd = wBlkEnd;

			if ( b->wBlkStart > wBlkStart )
				b->wBlkStart = wBlkStart;

			if ( !g_bSilent )
				printf( "\t\tWarning:  Partially overlaps a previous block--merged\n" );

			return;
		}
		bp = b;
		b = b->pNext;
	}

	// Add this block to the end of the list
	b = new FILE_BLOCK;

	if( !b )
	{
		fprintf(stderr,"Unable to alloc memory--aborted\n" );
	}

	b->wBlkStart = wBlkStart;
	b->wBlkEnd = wBlkEnd;
	b->pNext = NULL;

	if (bp)
		bp->pNext = b;
	else
		g_pBlocks = b;
}


void BlocksWrite( CFile& cf )
{
	FILE_BLOCK* b;
	FILE_BLOCK* bp;

	b = g_pBlocks;
	while (b)
	{
		if ( g_bOut)
		{
			cf.writeLEw( b->wBlkStart );
			cf.writeLEw( b->wBlkEnd );

			cf.Write( &g_pbtPool[ b->wBlkStart ], b->wBlkEnd - b->wBlkStart + 1 );
		}
		bp = b; // To allow free call after last use
		b = b->pNext;
		delete bp;
	}
	g_pBlocks = NULL;
}


void BlockDisassemble(WORD wBlkStart,WORD wBlkEnd)
{
	//don't disassemble run & init block
	if ( ( wBlkStart >= AT_RUNL )	&& ( wBlkEnd <= AT_INITH ) )
		goto BDEND;

	//don't disassemble hw addresses
	if ( ( wBlkStart >= AT_HWSTART ) && ( wBlkEnd <= AT_HWEND ) )
		goto BDEND;

	OutputBlockDiss( g_pbtPool + wBlkStart, wBlkStart, wBlkEnd );

BDEND:
	printf( "\n" );
	return;
}

void PrintOffset( CFile& cf )
{
	if ( !g_bSilent )
		printf( "[%04lX] ", cf.Tell() );
}

