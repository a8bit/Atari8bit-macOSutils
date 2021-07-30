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

#include "adsk_xfd.h"
#include "autil.h"
#include "cfile.h"

CXfd::CXfd() : ADisk()
{
	#ifdef _MEMORY_DUMP_
		printf( "CXfd constructed: %p\n", this );
	#endif
}

CXfd::~CXfd()
{
	#ifdef _MEMORY_DUMP_
		printf( "CXfd destructed: %p\n", this );
	#endif
}

typedef enum
{
	LOAD_OK,
	LOAD_BAD_DD_1,
	LOAD_BAD_DD_2,
	LOAD_BAD_DD_3
} LOAD_VARIANT;

#ifndef __CDISK_NOLOAD__
BOOL CXfd::Load( char* szFname, BOOL bRepair, BOOL bRepairAuto )
{
	LOAD_VARIANT load_method = LOAD_OK;	

	CFile cf;

	if ( !cf.Open( szFname ) )
	{
		sprintf( m_szLastError, "XFD: Can't open '%s'", szFname );
		return FALSE;
	}

	strcpy( m_szFname, szFname );

	LONG lFileLen = cf.GetLength();

	int iSecs;
	int iSecSize;

	if ( lFileLen % 0x80 )
	{
		sprintf( m_szLastError, "XFD: Strange length!" );
		cf.Close();
		return FALSE;
	}

	if ( ( lFileLen / 0x80 ) > 1040 )
	{
		iSecSize = 0x100;

		iSecs = ( ( lFileLen - 0x180 ) / 0x100 ) + 3;
	}
	else
	{
		iSecSize = 0x80;
		iSecs = lFileLen / 0x80;
	}

	if ( ( ( ( iSecs - 3 ) * iSecSize ) + 0x180 ) != lFileLen )
	{
		sprintf( m_szLastError, "XFD: Format violated: (%08lX != %08X)", lFileLen, iSecs * iSecSize );
		m_iErrorCode = CXFD_FORMAT_VIOLATED;

		#ifdef __CDISK_NOREPAIR__
		cf.Close();
		return FALSE;

		#else
		if ( !bRepair )
		{
			cf.Close();
			return FALSE;
		}
		else
		{
			iSecs = lFileLen / iSecSize;
			BYTE abtBuff[ 0x100 ];

			memset( abtBuff, 0, 0x100 );

			int iM1zeroes = 3;
			int iM2zeroes = 3;
			int iM3zeroes = 3;

			cf.Seek( ( 0x02 - 1 ) * 0x80, SEEK_SET );
			cf.Read( abtBuff, 0x80 );

			if ( IsBlockEmpty( abtBuff, 0x80 ) )
				iM1zeroes--;

			cf.Seek( ( 0x04 - 1 ) * 0x80, SEEK_SET );
			cf.Read( abtBuff, 0x80 );

			if ( IsBlockEmpty( abtBuff, 0x80 ) )
			{
				iM1zeroes--;
				iM2zeroes--;
			}

			cf.Seek( ( 0x05 - 1 ) * 0x80, SEEK_SET );
			cf.Read( abtBuff, 0x80 );

			if ( IsBlockEmpty( abtBuff, 0x80 ) )
				iM2zeroes--;

			cf.Seek( ( 0x06 - 1 ) * 0x80, SEEK_SET );
			cf.Read( abtBuff, 0x80 );

			if ( IsBlockEmpty( abtBuff, 0x80 ) )
			{
				iM1zeroes--;
				iM2zeroes--;
			}

			cf.Seek( -0x180, SEEK_END );
			cf.Read( abtBuff, 0x80 );
			if ( IsBlockEmpty( abtBuff, 0x80 ) )
				iM3zeroes--;

			cf.Read( abtBuff, 0x80 );
			if ( IsBlockEmpty( abtBuff, 0x80 ) )
				iM3zeroes--;

			cf.Read( abtBuff, 0x80 );
			if ( IsBlockEmpty( abtBuff, 0x80 ) )
				iM3zeroes--;

			if ( !iM1zeroes )
			{
				load_method = LOAD_BAD_DD_1;
			}
			else if ( !iM2zeroes )
			{
				load_method = LOAD_BAD_DD_2;
			}
			else if ( !iM3zeroes )
			{
				load_method = LOAD_BAD_DD_3;
			}

			if ( !bRepairAuto )
			{
				printf( "Invalid DD ATR file encountered.\n" );
				printf( "Choose repair method:\n" );
				printf( "1) Sector, gap, sector, gap, sector, gap, data\n" );
				printf( "2) Three sectors, three empty sectors, data\n" );
				printf( "3) Data, three empty sectors\n" );
				printf( "4) Don't repair\n" );

				switch( load_method )
				{
					case LOAD_BAD_DD_1:
						printf( "(Method 1 looks best)\n" );
						break;

					case LOAD_BAD_DD_2:
						printf( "(Method 2 looks best)\n" );
						break;

					case LOAD_BAD_DD_3:
						printf( "(Method 3 looks best)\n" );
						break;

					default:
						break;
				}

				int iMethod;

				printf( "\n" );
				do
				{
					iMethod = getch() - '0';
				} while( ( iMethod < 1 ) || ( iMethod > 4 ) );

				if ( iMethod == 4 )
				{
					cf.Close();
					return FALSE;
				}
			}
			else
			{
				if ( load_method == LOAD_OK )
					load_method = LOAD_BAD_DD_1;
			}

			cf.Seek( 0, SEEK_SET );
		}
		#endif
	}

	DISK_GEOMETRY dg;

	GuessClassicSizes( iSecs, iSecSize, &dg );

	if ( !Format( &dg ) )
	{
		cf.Close();
		return FALSE;
	}

	BYTE abtBuff[ 0x100 ];
	memset( abtBuff, 0, 0x100 );

	for( int i = 0; i < iSecs; i++ )
	{
		switch( load_method )
		{
			default:
			case LOAD_OK:
				cf.Read( abtBuff, ( i < 3 ) ? 0x80 : iSecSize );
				break;

			case LOAD_BAD_DD_1:
				if ( i < 3 )
				{
					cf.Read( abtBuff, 0x80 );
					cf.Seek( 0x80, SEEK_CUR );
				}
				else
					cf.Read( abtBuff, 0x100 );
				break;

			case LOAD_BAD_DD_2:
				if ( i < 3 )
				{
					cf.Read( abtBuff, 0x80 );

					if ( i == 2 )
						cf.Seek( 0x180, SEEK_CUR );
				}
				else
					cf.Read( abtBuff, 0x100 );

				break;

			case LOAD_BAD_DD_3:
				if ( i < 3 )
					cf.Read( abtBuff, 0x80 );
				else
					cf.Read( abtBuff, 0x100 );

				break;
		}

		if ( !WriteSector( i + 1, abtBuff ) )
		{
			cf.Close();
			return FALSE;
		}
	}

	cf.Close();
	return TRUE;

}
#endif


#ifdef __CDISK_SAVE__

BOOL CXfd::Save( char* szOutFile, BOOL bOverWrite )
{
	CFile cf;

	if ( !bOverWrite && !access( szOutFile, F_OK ) )
	{
		sprintf( m_szLastError, "XFD: File already exists! '%s'", szOutFile );
		return FALSE;
	}

	if ( !cf.Create( szOutFile ) )
	{
		sprintf( m_szLastError, "XFD: Can't create '%s'", szOutFile );
		return FALSE;
	}

	BYTE abtBuff[ 0x100 ];

	for( WORD i = 1; i <= m_geometry.iSectors; i++ )
	{
		if ( !ReadSector( abtBuff, i ) )
			return FALSE;

		int iToWrite = ( i <= 3 ) ? 0x80: m_geometry.iBytesPerSector;

		int iWritten;

		if ( !cf.Write( abtBuff, iToWrite, &iWritten ) || ( iWritten != iToWrite ) )
		{
			sprintf( m_szLastError, "XFD: Can't write!" );
			cf.Close();
			unlink( szOutFile );
			return FALSE;
		}

	}

	cf.Close();

	return TRUE;
}

#endif //__CDISK_SAVE__
