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

#include <iostream>
#include "pub.def"
#include "wintypes.h"
#include "autil.h"
#include "adsk.h"

BOOL Convert( char* szInFile, char* szOutFile );

DISK_TYPE g_dtypeIn = DISK_AUTO;
DISK_TYPE g_dtypeOut = DISK_ATR;

BOOL g_bVerbose = FALSE;
BOOL g_bBatchMode = FALSE;
BOOL g_bRepairAuto = FALSE;
BOOL g_bRepair = TRUE;
BOOL g_bFirstErrStop = FALSE;
BOOL g_bOverWrite = FALSE;
BOOL g_bTestOnly = FALSE;
BOOL g_bForceClassic = FALSE;

#define SHEADER PRG_NAME " v" PRG_VERSION " (c) " PRG_COPYRIGHT " " PRG_AUTHOR "\n"

#define HEADER SHEADER \
   PRG_DESC "\n" \
	"  Latest version can be found at " PRG_URL "\n" \
	"  Published under GPL. See GPL.TXT.\n\n"

#define USAGE HEADER "Usage:  " PRG_NAME " " PRG_USAGE

//THIS IS A GENERATED FILE. DO NOT EDIT!!!
//EDIT switches.def INSTEAD!


int m_iArgsCurr;
int m_iArgsOrig;
char** m_ppcArgvOrig;
char** m_ppcArgvCurr;
char* m_szSwitch;

//gets one token from cmdline
BOOL SWITCHES_GetSwitch()
{
  if ( !m_iArgsCurr )
    return FALSE;
  
  m_szSwitch = *( m_ppcArgvCurr++ );
  
  m_iArgsCurr--;
  
  return TRUE;
}

//gets one token, returns TRUE if value
BOOL SWITCHES_GetValue()
{
  if ( !SWITCHES_GetSwitch() )
    return FALSE;
  
  switch( *m_szSwitch )
  {
    case '-':
    case '/':
      return FALSE;
    default:
      return TRUE;
  }
}


BOOL SWFN_HELP( char* szUsage )
{
  if ( szUsage )
    printf( "%s\n", szUsage );
  printf( "-help         -this help\n" );
  printf( "-v            -verbose output\n" );
  printf( "-atr          -output is ATR\n" );
  printf( "-di           -output is DI\n" );
  printf( "-xfd          -output is XFD\n" );
  printf( "-scp          -output is SCP\n" );
  printf( "-dcm          -output is DCM\n" );
  printf( "-batch        -batch mode\n" );
  printf( "-errstop      -in batchmode stop on first error\n" );
  printf( "-over         -turns on file overwriting\n" );
  printf( "-autorepair   -auto repair mode\n" );
  printf( "-norepair     -don't repair\n" );
  printf( "-test         -only test input files\n" );
  printf( "-classic      -force to create 'classic' disk formats\n" );
  return FALSE;
}


BOOL SWITCHES_Init( int* pargc, char** argv )
{
  BOOL bRet = TRUE;
  
  m_iArgsCurr = *pargc - 1;
  m_iArgsOrig = 1;
  m_ppcArgvOrig = argv + 1;
  m_ppcArgvCurr = argv + 1;
  
  while( m_iArgsCurr )
  {
    if ( !bRet )
      break;
    
    if ( SWITCHES_GetValue() )
    {
      bRet = TRUE;
      *( m_ppcArgvOrig++ ) = * ( m_ppcArgvCurr - 1 );
      m_iArgsOrig++;
    }
    else
    {
      m_szSwitch++;
      
      bRet = TRUE;
      
      if (!strcmp( m_szSwitch, "help" ) || !strcmp( m_szSwitch, "?" ))
      {
        bRet = SWFN_HELP( USAGE );
        continue;
      }
      if (!strcmp( m_szSwitch, "v" ))
      {
        g_bVerbose = TRUE;
        continue;
      }
      if (!strcmp( m_szSwitch, "atr" ) || !strcmp( m_szSwitch, "a" ))
      {
        g_dtypeOut = DISK_ATR;
        continue;
      }
      if (!strcmp( m_szSwitch, "di" ))
      {
        g_dtypeOut = DISK_DI;
        continue;
      }
      if (!strcmp( m_szSwitch, "xfd" ) || !strcmp( m_szSwitch, "x" ))
      {
        g_dtypeOut = DISK_XFD;
        continue;
      }
      if (!strcmp( m_szSwitch, "scp" ) || !strcmp( m_szSwitch, "s" ))
      {
        g_dtypeOut = DISK_SCP;
        continue;
      }
      if (!strcmp( m_szSwitch, "dcm" ) || !strcmp( m_szSwitch, "d" ))
      {
        g_dtypeOut = DISK_DCM;
        continue;
      }
      if (!strcmp( m_szSwitch, "batch" ) || !strcmp( m_szSwitch, "batchmode" ) || !strcmp( m_szSwitch, "b" ))
      {
        g_bBatchMode = TRUE;
        continue;
      }
      if (!strcmp( m_szSwitch, "errstop" ))
      {
        g_bFirstErrStop = TRUE;
        continue;
      }
      if (!strcmp( m_szSwitch, "over" ) || !strcmp( m_szSwitch, "overwrite" ))
      {
        g_bOverWrite = TRUE;
        continue;
      }
      if (!strcmp( m_szSwitch, "autorepair" ))
      {
        g_bRepairAuto = TRUE;
        continue;
      }
      if (!strcmp( m_szSwitch, "norepair" ))
      {
        g_bRepair = FALSE;
        continue;
      }
      if (!strcmp( m_szSwitch, "test" ))
      {
        g_bTestOnly = TRUE;
        continue;
      }
      if (!strcmp( m_szSwitch, "classic" ))
      {
        g_bForceClassic = TRUE;
        continue;
      }
      
      printf( "Invalid switch: %s\n", *( m_ppcArgvCurr - 1 ) );
      return FALSE;
    } //endif
  } //while
  
  *pargc = m_iArgsOrig;
  return bRet;
}

int main(int argc,char *argv[])
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

	BOOL bRet = FALSE;

	char szOutFile[ 255 ];

	if ( g_bBatchMode )
	{
		argc--;
		argv++;

		while( argc-- )
		{
			char* szInFile = *(argv++);
			GuessBestFnameFromPC( szOutFile, szInFile, GetDiskTypeExt( g_dtypeOut ) );
			bRet = Convert( szInFile, szOutFile );
			printf( "\n" );

			if ( !bRet && g_bFirstErrStop )
				break;
		}

	}
	else
	{
		char* szInFile = argv[ 1 ];

		if ( argc >= 3 )
		{
			strcpy( szOutFile, argv[ 2 ] );
		}
		else
		{
			GuessBestFnameFromPC( szOutFile, szInFile, GetDiskTypeExt( g_dtypeOut ) );
		}

		bRet = Convert( szInFile, szOutFile );
	}

	if ( bRet && !g_bTestOnly )
		printf ( "Done.\n" );

	return bRet ? 0 : 1;
}

BOOL Convert( char* szInFile, char* szOutFile )
{
	ADisk* pDiskIn = NULL;
	ADisk* pDiskOut = NULL;

	DISKINIT_RETCODE ret;

	g_dtypeIn = DISK_ATR;
	if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &pDiskIn, DISK_ATR, szInFile, FALSE, g_bRepair, g_bRepairAuto ) ) )
	{
		g_dtypeIn = DISK_DI;
		if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &pDiskIn, DISK_DI, szInFile, FALSE, g_bRepair, g_bRepairAuto ) ) )
		{
			g_dtypeIn = DISK_DCM;
			if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &pDiskIn, DISK_DCM, szInFile, FALSE, g_bRepair, g_bRepairAuto ) ) )
			{
				g_dtypeIn = DISK_SCP;
				if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &pDiskIn, DISK_SCP, szInFile, FALSE, g_bRepair, g_bRepairAuto ) ) )
				{
					g_dtypeIn = DISK_XFD;
					if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &pDiskIn, DISK_XFD, szInFile, FALSE, g_bRepair, g_bRepairAuto ) ) )
					{
						printf( "Unable to determine format of disk image '%s'!\n", szInFile );
						return FALSE;
					}
				}
			}
		}
	}

	if ( ret == DI_RET_CANT_CONTINUE )
		return FALSE;

	printf( "Input file '%s' (%s)\n", szInFile, GetDiskTypeName( g_dtypeIn ) );

	if ( g_bVerbose )
	{
		DISK_GEOMETRY* pgeo = pDiskIn->GetGeometry();
		printf( "Sides: %d\n", pgeo->iSides );
		printf( "Tracks: %d\n", pgeo->iTracks );
		printf( "SecPerTrack: %d\n", pgeo->iSectorsPerTrack );
		printf( "BytesPerSec: %d\n", pgeo->iBytesPerSector );
		printf( "Sectors: %d\n", pgeo->iSectors );
	}

	if ( g_bTestOnly )
	{
		printf( "File loaded OK.\n" );
		delete pDiskIn;
		return TRUE;
	}

	switch( g_dtypeOut )
	{
		case DISK_ATR:
			pDiskOut = new CAtr();
			break;

		case DISK_XFD:
			pDiskOut = new CXfd();
			break;

		case DISK_DI:
			pDiskOut = new CDi();
			break;

		case DISK_SCP:
			pDiskOut = new CScp();
			break;

		case DISK_DCM:
			pDiskOut = new CDcm();
			break;

		default:
			break;
	}

	if ( !pDiskOut )
	{
		fprintf( stderr, "Can't create such image!\n" );
		return FALSE;
	}

	DISK_GEOMETRY* pgeo = pDiskIn->GetGeometry();
	DISK_GEOMETRY forced;

	memcpy( &forced, pgeo, sizeof( DISK_GEOMETRY ) );

	if ( g_bForceClassic )
		ForceClassicSize( &forced );

	//duplicating the image
	pDiskOut->Duplicate( pDiskIn, &forced );

	printf( "Output file '%s' (%s)\n", szOutFile, GetDiskTypeName( g_dtypeOut ) );

	//and save it in new format
	if ( !pDiskOut->Save( szOutFile, g_bOverWrite ) )
	{
		printf( "Error! %s\n", pDiskOut->GetLastError() );
		delete pDiskIn;
		delete pDiskOut;
		return FALSE;
	}

	delete pDiskIn;
	delete pDiskOut;

	return TRUE;
}

