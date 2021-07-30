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

#include "adsk_di.h"
#include "autil.h"
#include "cfile.h"

BYTE di_crc( BYTE* pbt, int iLen )
{
	WORD crc = 0;

	while( iLen-- )
	{
		crc += *( pbt++ );
		if( crc >= 0x100 )
			crc -= 0xFF;
	}
	
	return crc;
}

CDi::CDi() : ADisk()
{
	#ifdef _MEMORY_DUMP_
		printf( "CDi constructed: %p\n", this );
	#endif
}

CDi::~CDi()
{
	#ifdef _MEMORY_DUMP_
		printf( "CDi destructed: %p\n", this );
	#endif
}

#ifndef __CDISK_NOLOAD__
BOOL CDi::Load( char* szFname, BOOL bRepair, BOOL bRepairAuto )
{
	m_iErrorCode = 0;

	CFile cf;
	if ( !cf.Open( szFname ) )
	{
		sprintf( m_szLastError, "DI: Can't open '%s'", szFname );
		m_iErrorCode = CDISK_ERROR_CANT_OPEN;
		return FALSE;
	}

	strcpy( m_szFname, szFname );

	WORD wMagic = cf.readLEw();

	if ( wMagic != 18756 )	//'DI'
	{
		sprintf( m_szLastError, "DI: File '%s' is not an DI file!", szFname );
		return FALSE;
	}

	BYTE btVerLo = cf.readb();
	BYTE btVerHi = cf.readb();

	if( ( btVerLo != 0x20 ) || ( btVerHi != 0x02 ) )
	{
		sprintf( m_szLastError, "DI: The file '%s' has strange version!\n"
			"This program can't work with it now.\n"
			"Send the file for the analysis.", szFname );
		m_iErrorCode = CDI_FORMAT_VIOLATED;
		return FALSE;
	}

	int iMax = 200;
	while( iMax-- )
	{
		if( ! cf.readb() )
			break;
	}

	if( !iMax )
	{
		sprintf( m_szLastError, "DI: Runaway in header ('%s')", szFname );
		m_iErrorCode = CDI_FORMAT_VIOLATED;
		return FALSE;
	}

	//todo: this need the documentation! Is it number of sides?
	//BYTE btUnk1 = 
	cf.readb();

	BYTE btTracks = cf.readb();
	WORD wSectorsPerTrack = cf.readBEw();

	//todo: are these the flags?
	//WORD wUnk2 = 
	//cf.readBEw();
	BYTE btSides = cf.readb() + 1;
	cf.readb();
	
	WORD wSectorSize = cf.readBEw();

	//todo: and what about this?
	//BYTE btUnk3 = 
	cf.readb();

	switch( wSectorSize )
	{
		case 0x80:
		case 0x100:
			break;

		default:
		{
			sprintf( m_szLastError, "DI: Invalid sector size: %04X", wSectorSize );
			m_iErrorCode = CDI_FORMAT_VIOLATED;
			return FALSE;
		}
	}

	DISK_GEOMETRY dg;

	dg.iSides = btSides;
	dg.iTracks = btTracks;
	dg.iSectorsPerTrack = wSectorsPerTrack;
	dg.iBytesPerSector = wSectorSize;

	if ( !Format( &dg ) )
		return FALSE;

	BYTE abtBuff[ 0x100 ];
	memset( abtBuff, 0, 0x100 );

	int iSectors = m_geometry.iSectors;

	BYTE* pBuf = new BYTE[ iSectors ];
	cf.Read( pBuf, iSectors );

	BYTE* p = pBuf;

	for( int i = 0; i < iSectors; i++ )
	{
		if( *p )
		{
			int iSecSize = ( i < 3 ) ? 0x80 : wSectorSize;

			int iRead;

			cf.Read( abtBuff, iSecSize, &iRead );

			if ( iRead != iSecSize )
			{
				sprintf( m_szLastError, "DI: Read error. File truncated?" );
				delete [] pBuf;
				cf.Close();
				m_iErrorCode = CDI_FORMAT_VIOLATED;
				return FALSE;
			}

			BYTE crc = di_crc( abtBuff, iSecSize );

			if( *p != crc )
			{
				sprintf( m_szLastError, "DI: Sector checksum failed: Sector %d Given: %02X Computed: %02X", i + 1, *p, crc );
				delete [] pBuf;
				cf.Close();
				m_iErrorCode = CDI_FORMAT_VIOLATED;
				return FALSE;
			}

			WriteSector( i + 1, abtBuff );
		}
		p++;
	}

	delete [] pBuf;

	cf.Close();
	return TRUE;

}

#endif

#ifdef __CDISK_SAVE__

char szDIhd[] = "Jindroush's DI class v1.00";
//char szDIhd[] = "XL/ST-link 2.2.0› ";

BOOL CDi::Save( char* szOutFile, BOOL bOverWrite )
{
	if( ( m_geometry.iTracks > 0xFF ) ||
		( m_geometry.iSectorsPerTrack > 0xFFFF ) ||
		( m_geometry.iSides > 2 ) )
	{
		sprintf( m_szLastError, "DI: Can't create such file! Is it possible? :-)" );
		return FALSE;
	}

	CFile cf;

	if ( !bOverWrite && !access( szOutFile, F_OK ) )
	{
		sprintf( m_szLastError, "DI: File already exists! '%s'", szOutFile );
		return FALSE;
	}

	if ( !cf.Create( szOutFile ) )
	{
		sprintf( m_szLastError, "DI: Can't create '%s'", szOutFile );
		return FALSE;
	}

	cf.writeb( 'D' );
	cf.writeb( 'I' );
	cf.writeb( 0x20 );
	cf.writeb( 0x02 );

	cf.Write( szDIhd, strlen( szDIhd ) + 1 );

	cf.writeb( 1 );
	cf.writeb( m_geometry.iTracks );
	cf.writeBEw( m_geometry.iSectorsPerTrack );
	cf.writeb( m_geometry.iSides - 1 );
	cf.writeb( 0 );
	cf.writeBEw( m_geometry.iBytesPerSector );
	cf.writeb( 0 );

	BYTE abtBuff[ 0x100 ];
	memset( abtBuff, 0, 0x100 );

	BYTE* pBuf = new BYTE[ m_geometry.iSectors ];

	for( int i = 0; i < m_geometry.iSectors; i++ )
	{
		int iSecSize = ( i < 3 ) ? 0x80 : m_geometry.iBytesPerSector;

		ReadSector( abtBuff, i + 1 );
		pBuf[ i ] = di_crc( abtBuff, iSecSize );
	}

	cf.Write( pBuf, m_geometry.iSectors );

	for( int i = 0; i < m_geometry.iSectors; i++ )
	{
		int iSecSize = ( i < 3 ) ? 0x80 : m_geometry.iBytesPerSector;

		ReadSector( abtBuff, i + 1 );
		if( pBuf[ i ] || !IsBlockEmpty( abtBuff, iSecSize ) )
		{
			cf.Write( abtBuff, iSecSize );
		}
	}

	delete [] pBuf;

	cf.Close();

	return TRUE;
}

#endif //__CDISK_WRITE__
