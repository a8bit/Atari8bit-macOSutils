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
#include "autil.h"
#include "adsk.h"

DISK_TYPE g_dtypeOut = DISK_ATR;
BOOL g_bClassic = FALSE;
#define MAX_STRING 256
char g_szMessage[ MAX_STRING ] = "";

typedef struct
{
	WORD w1;
	WORD w2;
	WORD w3;
	WORD w4;
	WORD w5;
	WORD w6;
	WORD w7;
} BASHEAD;

#define BAS_HEAD_SIZE 0x0E

#define SHEADER PRG_NAME " v" PRG_VERSION " (c) " PRG_COPYRIGHT " " PRG_AUTHOR "\n"

#define HEADER SHEADER \
   PRG_DESC "\n" \
	"  Latest version can be found at " PRG_URL "\n" \
	"  Published under GPL. See GPL.TXT.\n\n"

#define USAGE HEADER "Usage:  " PRG_NAME " " PRG_USAGE

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

	fprintf( stderr, SHEADER );

	char* szInFile = argv[ 1 ];
	char szOutFile[ MAX_STRING ];

	//setting the message
	char szMessage[ MAX_STRING ];
	strcpy( szMessage, g_szMessage );

	//if not present, set to 'Loading prgname'
	if ( ! *szMessage )
	{
		strcpy( szMessage, "Loading " );
		strcat( szMessage, szInFile );
	}

	//and pad it with spaces
	memset( szMessage + strlen( szMessage), 0x20, MAX_STRING - strlen( szMessage ) );
	szMessage[ MAX_STRING - 1 ] = '\0';

	//get output filename
	if ( argc >= 3 )
	{
		strcpy( szOutFile, argv[ 2 ] );
	}
	else
	{
		GuessBestFnameFromPC( szOutFile, szInFile, GetDiskTypeExt( g_dtypeOut ) );
	}

	ADisk* pDisk = NULL;
	switch( g_dtypeOut )
	{
		case DISK_ATR:
			pDisk = new CAtr();
			break;

		case DISK_XFD:
			pDisk = new CXfd();
			break;

		case DISK_SCP:
			pDisk = new CScp();
			break;

		case DISK_DCM:
			pDisk = new CDcm();
			break;

		case DISK_DI:
			pDisk = new CDi();
			break;

		default:
			break;
	}

	if ( !pDisk )
	{
		printf( "Can't create such image!\n" );
		return FALSE;
	}

	//reads boot stub
	char szBootName[ MAX_STRING ];
	strcpy( szBootName, argv[ 0 ] );
	char* szWhere = strrchr( szBootName, '/' );

	if ( szWhere )
		szWhere++;
	else
		szWhere = szBootName;
		
	strcpy( szWhere, "bas2boot.boo" );

	CFile cf;

	if ( !cf.Open( szBootName ) )
	{
		printf( "Can't open boot image: %s\n", szBootName );
		return 1;
	}

	long lBootLen = cf.GetLength();

	if ( lBootLen % 0x80 )
	{
		printf( "Invalid boot image: %s\n", szBootName );
		cf.Close();
		return 1;
	}

	BYTE* pbtBoot = new BYTE [ lBootLen ];

	cf.Read( pbtBoot, lBootLen );

	cf.Close();

	int iBootSectors = lBootLen / 0x80;

	if ( pbtBoot[ 1 ] != iBootSectors )
	{
		printf( "Invalid boot image: %s\nSize mismatch.\n", szBootName );
		cf.Close();
		delete [] pbtBoot;
		return 1;
	}

	//open basic file
	if ( !cf.Open( szInFile ) )
	{
		printf( "Can't open source file '%s'!\n", szInFile );
		return 1;
	}

	long lBasicLen = cf.GetLength();

	//set real length in header
	pbtBoot[ 8 ] = lBasicLen & 0xFF;
	pbtBoot[ 9 ] = lBasicLen >> 8;

	lBasicLen -= BAS_HEAD_SIZE;

	int iBasicSectors = ( lBasicLen + 0x7F ) / 0x80;

	//load and relocate basic header
	BASHEAD head;
	head.w1 = cf.readLEw();
	head.w2 = cf.readLEw() - head.w1;
	head.w3 = cf.readLEw() - head.w1;
	head.w4 = cf.readLEw() - head.w1;
	head.w5 = cf.readLEw() - head.w1;
	head.w6 = cf.readLEw() - head.w1;
	head.w7 = cf.readLEw() - head.w1;

	BYTE* pbtHd = pbtBoot + lBootLen - BAS_HEAD_SIZE - 2;

	*( pbtHd++ ) = ( iBasicSectors - 1 ) & 0xFF;
	*( pbtHd++ ) = ( iBasicSectors - 1 ) >>8;

	*( pbtHd++ ) = head.w1 & 0xFF;
	*( pbtHd++ ) = head.w1 >> 8;

	*( pbtHd++ ) = head.w2 & 0xFF;
	*( pbtHd++ ) = head.w2 >> 8;

	*( pbtHd++ ) = head.w3 & 0xFF;
	*( pbtHd++ ) = head.w3 >> 8;

	*( pbtHd++ ) = head.w4 & 0xFF;
	*( pbtHd++ ) = head.w4 >> 8;

	*( pbtHd++ ) = head.w5 & 0xFF;
	*( pbtHd++ ) = head.w5 >> 8;

	*( pbtHd++ ) = head.w6 & 0xFF;
	*( pbtHd++ ) = head.w6 >> 8;

	*( pbtHd++ ) = head.w7 & 0xFF;
	*( pbtHd++ ) = head.w7 >> 8;

	//find empty space at the end of boot
	BYTE* pbtString = pbtBoot + lBootLen - BAS_HEAD_SIZE - 2 - 2;
	BYTE* pbtStringEnd = pbtString;

	while( *pbtString == 0x20 )
		pbtString--;

	pbtString++;

	int iStringLen = pbtStringEnd - pbtString;

	//put the message there
	memcpy( pbtString, szMessage, iStringLen + 1 );

	//read basic program
	BYTE* pbtBas = new BYTE [ iBasicSectors * 0x80 ];
	memset( pbtBas, 0, iBasicSectors * 0x80 );

	cf.Read( pbtBas, lBasicLen );

	cf.Close();

	//format disk
	DISK_GEOMETRY dg;
	
	if ( g_bClassic )
		ForceClassicSize( iBasicSectors + iBootSectors, 0x80, &dg );
	else
		GuessClassicSizes( iBasicSectors + iBootSectors, 0x80, &dg );

	if ( !pDisk->Format( &dg ) )
	{
		delete pDisk;
		delete[] pbtBoot;
		delete [] pbtBas;
		return 1;
	}

	//write boot & body
	pDisk->WriteSectors( 1, pbtBoot, iBootSectors );
	pDisk->WriteSectors( 1 + iBootSectors, pbtBas, iBasicSectors );

	//save it to disk
	if ( !pDisk->Save( szOutFile, FALSE ) )
	{
		printf( "Error! %s\n", pDisk->GetLastError() );
		delete pDisk;
		delete [] pbtBoot;
		delete [] pbtBas;
		return 1;
	}

	delete pDisk;

	delete [] pbtBoot;
	delete [] pbtBas;

	printf( "Written to %s (%s)\n", szOutFile, GetDiskTypeName( g_dtypeOut ) );

	return 0;
}

