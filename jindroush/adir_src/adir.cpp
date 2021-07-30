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
#include <fnmatch.h>

#include "pub.def"

#include "adsk.h"

#include "cfs.h"
#include "cfs_dos2.h"
#include "cfs_dos3.h"
#include "cfs_dosm.h"
#include "cfs_doss.h"
#include "cfs_kboo.h"
#include "cfs_b2b.h"
#include "cfs_jonw.h"
#include "cfs_howf.h"
#include "cfs_robc.h"
#include "cfs_boot.h"

typedef enum
{
	DOS_AUTO,
	DOS_2,
	DOS_3,
	DOS_MY,
	DOS_SP,
	DOS_KBOOT,
	DOS_B2B,
	DOS_JONW,
	DOS_HOWF,
	DOS_ROBC,
	DOS_BOOT,
} DOS_TYPE;

void ReadDir( char* szDirName, CDirEntry* pEntry );
BOOL CheckMask( char* szFname, char* szMask );
void MakePath( char* szDest, char* szDir, char* szFileName );
BOOL InitializeFs( DOS_TYPE dostype, BOOL bVerbose );
BOOL SWFN_DOST( void* );
char* GetFSName( DOS_TYPE dostype );

BOOL g_bIsMyDos = FALSE;
BOOL g_bToLower = FALSE;
BOOL g_bVerbose = FALSE;
BOOL g_bListOnly = TRUE;
BOOL g_bRepairAuto = TRUE;

DOS_TYPE g_dostype = DOS_AUTO;
DISK_TYPE g_dtypeIn = DISK_AUTO;

char g_szMask[ 256 ] = "*";
char g_szPath[ 256 ] = ".";

ADisk* g_pDisk;
CFs* g_pFs;

#define SHEADER PRG_NAME " v" PRG_VERSION " (c) " PRG_COPYRIGHT " " PRG_AUTHOR "\n"

#define HEADER SHEADER \
   PRG_DESC "\n" \
	"  Latest version can be found at " PRG_URL "\n" \
	"  Published under GPL. See GPL.TXT.\n\n"

#define USAGE HEADER "Usage:  " PRG_NAME " " PRG_USAGE

#include "switches.cpp"

//#include <crt0.h>
//int _crt0_startup_flags = _CRT0_FLAG_PRESERVE_FILENAME_CASE;

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

	char* szInfile = argv[ 1 ];

	if ( argc >= 3 )
	{
		strcpy( g_szMask, argv[ 2 ] );
	}

	DISKINIT_RETCODE ret;
	g_dtypeIn = DISK_ATR;
	if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &g_pDisk, DISK_ATR, szInfile, FALSE, TRUE, g_bRepairAuto ) ) )
	{
		g_dtypeIn = DISK_DI;
		if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &g_pDisk, DISK_DI, szInfile, FALSE, FALSE, FALSE ) ) )
		{
			g_dtypeIn = DISK_DCM;
			if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &g_pDisk, DISK_DCM, szInfile, FALSE, FALSE, FALSE ) ) )
			{
				g_dtypeIn = DISK_SCP;
				if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &g_pDisk, DISK_SCP, szInfile, FALSE, FALSE, FALSE ) ) )
				{
					g_dtypeIn = DISK_XFD;
					if ( DI_RET_CONTINUE == ( ret = InitializeDisk( &g_pDisk, DISK_XFD, szInfile, FALSE, TRUE, g_bRepairAuto ) ) )
					{
						printf( "Unable to determine format of disk image '%s'!\n", szInfile );
						return FALSE;
					}
				}
			}
		}
	}

	if ( ret == DI_RET_CANT_CONTINUE )
		return 1;

	printf( "Input file '%s' (%s)\n", szInfile, GetDiskTypeName( g_dtypeIn ) );


	if ( g_dostype == DOS_AUTO )
	{
		DOS_TYPE bestfs = DOS_AUTO;
		int iBest = 100;

		printf( "Checking '%s' for possible filesystems:\n", argv[1] );

		if ( InitializeFs( DOS_SP, FALSE ) )
		{
			if ( iBest > g_pFs->GetInvalidPercent() )
			{
				bestfs = DOS_SP;
				iBest = g_pFs->GetInvalidPercent();
			}
			delete g_pFs;
		}

		if ( InitializeFs( DOS_ROBC, FALSE ) )
		{
			if ( iBest > g_pFs->GetInvalidPercent() )
			{
				bestfs = DOS_ROBC;
				iBest = g_pFs->GetInvalidPercent();
			}
			delete g_pFs;
		}

		if ( InitializeFs( DOS_MY, FALSE ) )
		{
			if ( iBest > g_pFs->GetInvalidPercent() )
			{
				bestfs = DOS_MY;
				iBest = g_pFs->GetInvalidPercent();
			}
			delete g_pFs;
		}

		if ( InitializeFs( DOS_2, FALSE ) )
		{
			if ( iBest > g_pFs->GetInvalidPercent() )
			{
				bestfs = DOS_2;
				iBest = g_pFs->GetInvalidPercent();
			}
			delete g_pFs;

		}

		if ( InitializeFs( DOS_KBOOT, FALSE ) )
		{
			if ( iBest > g_pFs->GetInvalidPercent() )
			{
				bestfs = DOS_KBOOT;
				iBest = g_pFs->GetInvalidPercent();
			}
			delete g_pFs;
		}

		if ( InitializeFs( DOS_B2B, FALSE ) )
		{
			if ( iBest > g_pFs->GetInvalidPercent() )
			{
				bestfs = DOS_B2B;
				iBest = g_pFs->GetInvalidPercent();
			}
			delete g_pFs;
		}

		if ( InitializeFs( DOS_JONW, FALSE ) )
		{
			if ( iBest > g_pFs->GetInvalidPercent() )
			{
				bestfs = DOS_JONW;
				iBest = g_pFs->GetInvalidPercent();
			}
			delete g_pFs;
		}

		if ( InitializeFs( DOS_HOWF, FALSE ) )
		{
			if ( iBest > g_pFs->GetInvalidPercent() )
			{
				bestfs = DOS_HOWF;
				iBest = g_pFs->GetInvalidPercent();
			}
			delete g_pFs;
		}

		if ( InitializeFs( DOS_3, FALSE ) )
		{
			if ( iBest > g_pFs->GetInvalidPercent() )
			{
				bestfs = DOS_3;
				iBest = g_pFs->GetInvalidPercent();
			}
			delete g_pFs;
		}

		if ( ( bestfs == DOS_AUTO ) || iBest )
		{
			fprintf( stderr, "\nUnable to guess best filesystem (Best was %s: %d%%). Exiting.\n", GetFSName( bestfs ), iBest );
			delete g_pDisk;
			return 1;
		}
		else
		{
			printf( "\nBest guess:\n" );

			if ( !InitializeFs( bestfs, TRUE ) )
			{
				delete g_pDisk;
				return 1;
			}
		}

	}
	else
	{
		if ( !InitializeFs( g_dostype, TRUE ) )
		{
			delete g_pDisk;
			return 1;
		}
	}

	CDirEntry* pEntry = g_pFs->GetRoot();

	if ( !pEntry )
	{
		fprintf( stderr, "No files found!\n" );
		delete g_pFs;
		delete g_pDisk;
		return 1;
	}

	ReadDir( NULL, pEntry );

	delete g_pFs;
	delete g_pDisk;

	return 0;
}

BOOL InitializeFs( DOS_TYPE dostype, BOOL bVerbose )
{
	switch( dostype )
	{
		case DOS_2:
			g_pFs = new CDos2();
			break;

		case DOS_3:
			g_pFs = new CDos3();
			break;

		case DOS_MY:
			g_pFs = new CDosM();
			break;

		case DOS_SP:
			g_pFs = new CDosS();
			break;

		case DOS_KBOOT:
			g_pFs = new CKBoot();
			break;

		case DOS_B2B:
			g_pFs = new CBas2Boot();
			break;

		case DOS_JONW:
			g_pFs = new CJonw();
			break;

		case DOS_HOWF:
			g_pFs = new CHowf();
			break;

		case DOS_ROBC:
			g_pFs = new CRobc();
			break;

		case DOS_BOOT:
			g_pFs = new CBoot();
			break;

		default:
			if ( bVerbose )
				fprintf( stderr, "Invalid filesystem specified!\n" );
			return FALSE;
	}

	if ( !g_pFs )
	{
		if ( bVerbose )
		 	fprintf( stderr, "Can't initialize filesystem driver!\n" );
	 	return FALSE;
	}

	if ( !g_pFs->Mount( g_pDisk ) )
	{
		if ( bVerbose )
			fprintf( stderr, "Mount failed because:\n%s\n", g_pFs->GetLastError() );
		delete g_pFs;
		return FALSE;
	}

	printf( "Mounting as %s: Invalid %d%%\n", GetFSName( dostype ), g_pFs->GetInvalidPercent() );

	return TRUE;
}

void ReadDir( char* szDirName, CDirEntry* pEntry )
{
	char szCurrEntryName[ 256 ];

	do
	{
		strcpy( szCurrEntryName, pEntry->m_szFname );

		char szFullAtariPath[ 256 ];

		if ( szDirName )
			MakePath( szFullAtariPath, szDirName, szCurrEntryName );
		else
			strcpy( szFullAtariPath, szCurrEntryName );

		char szFullOsPath[ 256 ];
		MakePath( szFullOsPath, g_szPath, szFullAtariPath );

		if ( CheckMask( szCurrEntryName, g_szMask ) )
		{

			if ( pEntry->m_dwFlags & DIRE_SUBDIR )
			{
				if ( !g_bListOnly )
				{
					printf( "Creating dir %s\n", szCurrEntryName );
					mkdir( szFullOsPath, 0777 );
				}

				if ( pEntry->m_pSubdir )
					ReadDir( szFullAtariPath, pEntry->m_pSubdir );

			}
			else
			{
				printf( "%s %s ", pEntry->m_szAscData, szFullAtariPath );

				if ( !g_bListOnly )
				{
					BOOL bRes = g_pFs->ExportFile( szFullOsPath, pEntry );

					if ( bRes )
					{
						printf( "exported\n" );
					}
					else
					{
						printf( "invalid because:\n%s\n", g_pFs->GetLastError() );
					}
				}
				else
				{
					printf( "\n" );
				}
			}
		}

		pEntry = pEntry->m_pNext;

	} while( pEntry );

	return;
}

BOOL CheckMask( char* szFname, char* szMask )
{
	return fnmatch( szMask, szFname, FNM_NOESCAPE ) ? FALSE : TRUE;
}

void MakePath( char* szDest, char* szDir, char* szFileName )
{
	int iLength;

	if ( szDest != szDir )
		strcpy ( szDest, szDir );

	iLength = strlen( szDest );

	if ( iLength )
	{
		if ( szDest[ iLength - 1 ] != '/' )
		{
			szDest[ iLength + 1 ] = '\0';
			szDest[ iLength ] = '/';
		}
	}
	strcat( szDest, szFileName );
}

BOOL SWFN_DOST( void* p )
{
	g_dostype = (DOS_TYPE)(long)p;

	return TRUE;
}

//returns ptr to filesystem name
char* GetFSName( DOS_TYPE dostype )
{
	switch( dostype )
	{
		case DOS_2:
			return "Dos2";

		case DOS_3:
			return "Dos3";

		case DOS_MY:
			return "MyDos";

		case DOS_SP:
			return "SpartaDos";

		case DOS_KBOOT:
			return "KBoot";

		case DOS_B2B:
			return "Bas2Boot";

		case DOS_HOWF:
			return "HowFen";

		case DOS_ROBC:
			return "RobC";

		case DOS_BOOT:
			return "Boot";

		case DOS_JONW:
			return "JonW";

		default:
			return "None";
	}
}

//returns ptr to disk type name
