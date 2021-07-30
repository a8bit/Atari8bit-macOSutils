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

#include "cdsk_scp.h"
#include "autil.h"
#include "cfile.h"

#define SCP_MAGIC 0xFDFD

CScp::CScp() : CDisk()
{
	#ifdef _MEMORY_DUMP_
		printf( "CScp constructed: %08X\n", this );
	#endif
}

CScp::~CScp()
{
	#ifdef _MEMORY_DUMP_
		printf( "CScp destructed: %08X\n", this );
	#endif
}

BOOL CScp::Load( char* szFname, BOOL, BOOL )
{
	WORD	wMagic;
	BYTE	btSectorSize;
	BYTE	btTracks;
	BYTE	btSectorsPerTrack;

	CFile cf;

	if( !cf.Open( szFname ) )
	{
		sprintf( m_szLastError, "SCP: Can't open '%s'", szFname );
		return FALSE;
	}

	strcpy( m_szFname, szFname );

	wMagic = cf.readLEw();

	if ( wMagic != SCP_MAGIC )
	{
		sprintf( m_szLastError, "SCP: File '%s' is not an SCP file!", szFname );
		return FALSE;
	}

	btSectorSize = cf.readb();
	btTracks = cf.readb();
	btSectorsPerTrack = cf.readb();

	int iSectorSize;

	switch( btSectorSize )
	{
		case 0x80:
			iSectorSize = 0x80;
			break;

		case 0x00:
			iSectorSize = 0x100;
			break;

		default:
		{
			sprintf( m_szLastError, "SCP: Invalid sector size: %02X", btSectorSize );
			return FALSE;
		}
	}

	DISK_GEOMETRY dg;
	dg.iSides = 1;
	dg.iTracks = btTracks;
	dg.iSectorsPerTrack = btSectorsPerTrack;
	dg.iBytesPerSector = iSectorSize;

	if ( !Format( &dg ) )
		return FALSE;

	BYTE *pbtTable = new BYTE [ btSectorsPerTrack * btTracks ];

	if ( ! pbtTable )
	{
	 	sprintf( m_szLastError, "SCP: Not enough memory for sector table!" );
	 	return FALSE;
	}

	if ( !cf.Read( pbtTable, btSectorsPerTrack * btTracks ) )
	{
	 	sprintf( m_szLastError, "SCP: Can't read sector table!" );
	 	return FALSE;
	}

	BYTE abtBuff[ 0x100 ];
	memset( abtBuff, 0, 0x100 );

	BYTE* pbtPtr = pbtTable;

	for( int iTrack = 0; iTrack < btTracks; iTrack++ )
	{
		for( int iSector = 0; iSector < btSectorsPerTrack; iSector++ )
		{
			if ( *pbtPtr )
			{
				int iNowRead;

				if ( !iTrack && ( iSector < 3 ) )
					iNowRead = 0x80;
				else
					iNowRead = iSectorSize;

				int iReallyRead;

				if ( !cf.Read( abtBuff, iNowRead, &iReallyRead ) || ( iNowRead != iReallyRead ) )
				{
					delete [] pbtTable;
	 				sprintf( m_szLastError, "SCP: Image broken!" );
	 				return FALSE;
				}

				if ( !WriteSector( *pbtPtr + iTrack* btSectorsPerTrack, abtBuff ) )
				{
					delete [] pbtTable;
					return FALSE;
				}

			}
			pbtPtr++;
		}

	}

	delete [] pbtTable;

	cf.Close();
	return TRUE;

}

#ifdef __CDISK_WRITE__

BOOL CScp::Save( char* szOutFile, BOOL bOverWrite )
{
	if ( !bOverWrite && !access( szOutFile, F_OK ) )
	{
		sprintf( m_szLastError, "SCP: File already exists! '%s'", szOutFile );
		return FALSE;
	}

	BYTE btSize;
	BYTE btTracks;
	BYTE btSpT;

	switch ( m_geometry.iBytesPerSector )
	{
		case 0x80:
			btSize = 0x80;
			break;

		case 0x100:
		default:
			btSize = 0x00;
			break;
	}

	BOOL bGood = FALSE;

	btTracks = m_geometry.iTracks;
	btSpT = m_geometry.iSectorsPerTrack;

	if ( ( m_geometry.iTracks == 40 ) && ( m_geometry.iSectorsPerTrack == 18 ) )
		bGood = TRUE;

	if ( ( m_geometry.iTracks == 40 ) && ( m_geometry.iSectorsPerTrack == 26 ) )
		bGood = TRUE;

	if ( !bGood )
	{
		sprintf( m_szLastError, "SCP: Can't export, because of invalid disk size!" );
		return FALSE;
	}

	int iMapSize = m_geometry.iTracks * m_geometry.iSectorsPerTrack;

	BYTE* pMap = new BYTE [ iMapSize ];

	if ( !pMap )
	{
		sprintf( m_szLastError, "SCP: Can't allocate memory for map!" );
		return FALSE;
	}

	memset( pMap, 0, iMapSize );


	CFile cf;

	if ( !cf.Create( szOutFile ) )
	{
		sprintf( m_szLastError, "SCP: Can't create '%s'", szOutFile );
		delete [] pMap;
		return FALSE;
	}

	WORD wMagic = SCP_MAGIC;

	cf.writeLEw( wMagic );
	cf.writeb( btSize );
	cf.writeb( btTracks );
	cf.writeb( btSpT );

	cf.Seek( iMapSize, SEEK_CUR );

	BYTE abtBuff[ 0x100 ];

	int iSectors = m_geometry.iSectors;

	for( int i = 0; i < iSectors; i++ )
	{
		if ( !ReadSector( abtBuff, i + 1 ) )
		{
			delete [] pMap;
			cf.Close();
			unlink( szOutFile );
			return FALSE;
		}

		int iBytesNow = ( i < 3 ) ? 0x80 : m_geometry.iBytesPerSector;

		if ( !IsBlockEmpty( abtBuff, iBytesNow ) )
		{
			int iWritten;
			if ( !cf.Write( abtBuff, iBytesNow, &iWritten ) || ( iBytesNow != iWritten ) )
			{
				sprintf( m_szLastError, "SCP: Error writing to '%s'", szOutFile );
				delete [] pMap;
				cf.Close( );
				unlink( szOutFile );
				return FALSE;
			
			}

			pMap[ i ] = ( i % btSpT ) + 1;
		}
	}

	cf.Seek( 5, SEEK_SET );

	if ( !cf.Write( pMap, iMapSize ) )
	{
		sprintf( m_szLastError, "SCP: Can't write!" );
		delete [] pMap;
		cf.Close( );
		unlink( szOutFile );
		return FALSE;
	
	}

	delete [] pMap;

	cf.Close();

	return TRUE;
}

#endif //__CDISK_WRITE__
