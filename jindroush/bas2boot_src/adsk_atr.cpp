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

#include "adsk_atr.h"
#include "autil.h"
#include "cfile.h"

#define ATR_MAGIC 0x0296

typedef struct
{
	WORD	wMagic;
	WORD	wPars;
	WORD	wSecSize;
	BYTE	btParHigh;
	DWORD dwCRC;
	DWORD dwUnused;
	BYTE	btFlags;

} ATRhead;

#define ATR_HEAD_LEN 0x10

CAtr::CAtr() : ADisk()
{
	#ifdef _MEMORY_DUMP_
		printf( "CAtr constructed: %p\n", this );
	#endif
}

CAtr::~CAtr()
{
	#ifdef _MEMORY_DUMP_
		printf( "CAtr destructed: %p\n", this );
	#endif
}

typedef enum
{
	LOAD_OK,
	LOAD_BAD_DD_1,
	LOAD_BAD_DD_2,
	LOAD_BAD_DD_3,
	LOAD_PAD
} LOAD_VARIANT;

#ifndef __CDISK_NOLOAD__
BOOL CAtr::Load( char* szFname, BOOL bRepair, BOOL bRepairAuto )
{
	LOAD_VARIANT load_method = LOAD_OK;	

	m_iErrorCode = 0;

	int iFirstSectorsSize = 0x80;

	CFile cf;

	if ( !cf.Open( szFname ) )
	{
		sprintf( m_szLastError, "ATR: Can't open '%s'", szFname );
		m_iErrorCode = CDISK_ERROR_CANT_OPEN;
		return FALSE;
	}

	strcpy( m_szFname, szFname );

	ATRhead head;

	head.wMagic			=	cf.readLEw();	
	head.wPars			=	cf.readLEw();	
	head.wSecSize		=	cf.readLEw();	
	head.btParHigh		=	cf.readb();	
	head.dwCRC			=	cf.readLEdw(); 
	head.dwUnused		=	cf.readLEdw(); 
	head.btFlags		=	cf.readb();	

	if ( head.wMagic != ATR_MAGIC )
	{
		sprintf( m_szLastError, "ATR: File '%s' is not an ATR file!", szFname );
		return FALSE;
	}

	LONG lFileLen = cf.GetLength();
	cf.Seek( ATR_HEAD_LEN, SEEK_SET );

	switch( head.wSecSize )
	{
		case 0x80:
		case 0x100:
			break;

		default:
		{
			sprintf( m_szLastError, "ATR: Invalid sector size: %04X", head.wSecSize );
			return FALSE;
		}
	}

	DWORD dwPars = head.wPars | ( head.btParHigh << 16 );

	int iSectors = ( dwPars * 0x10 ) / head.wSecSize;

	//BOOL bReadOnly = (head.btFlags & 1) ? TRUE : FALSE;

	if ( head.wSecSize == 0x100 )
	{
		//if ( dwPars % head.wSecSize )
		if ( ( dwPars * 0x10 ) % head.wSecSize )
		{
			iSectors = ( ( dwPars * 0x10 - 0x180 ) / head.wSecSize ) + 3;
		}
		else
		{
			sprintf( m_szLastError, "ATR: Format violated. First three sectors are not $80 long!" );
			m_iErrorCode = CATR_FORMAT_VIOLATED;

			#ifdef __CDISK_NOREPAIR__
				return FALSE;
			#else
			if ( !bRepair )
			{
				return FALSE;
			}
			else
			{
				BYTE abtBuff[ 0x100 ];

				memset( abtBuff, 0, 0x100 );

				int iM1zeroes = 3;
				int iM2zeroes = 3;
				int iM3zeroes = 3;

				cf.Seek( ATR_HEAD_LEN + ( 0x02 - 1 ) * 0x80, SEEK_SET );
				cf.Read( abtBuff, 0x80 );

				if ( IsBlockEmpty( abtBuff, 0x80 ) )
					iM1zeroes--;

				cf.Seek( ATR_HEAD_LEN + ( 0x04 - 1 ) * 0x80, SEEK_SET );
				cf.Read( abtBuff, 0x80 );

				if ( IsBlockEmpty( abtBuff, 0x80 ) )
				{
					iM1zeroes--;
					iM2zeroes--;
				}

				cf.Seek( ATR_HEAD_LEN + ( 0x05 - 1 ) * 0x80, SEEK_SET );
				cf.Read( abtBuff, 0x80 );

				if ( IsBlockEmpty( abtBuff, 0x80 ) )
					iM2zeroes--;

				cf.Seek( ATR_HEAD_LEN + ( 0x06 - 1 ) * 0x80, SEEK_SET );
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

					switch( iMethod )
					{
						case 1:
							load_method = LOAD_BAD_DD_1;
							break;
						case 2:
							load_method = LOAD_BAD_DD_2;
							break;
						case 3:
							load_method = LOAD_BAD_DD_3;
							break;
						default:
						case 4:
							return FALSE;
							break;
					}
				}
				else
				{
					if ( load_method == LOAD_OK )
						load_method = LOAD_BAD_DD_1;
				}


				cf.Seek( ATR_HEAD_LEN, SEEK_SET );

				switch( load_method )
				{
					case LOAD_BAD_DD_1:
					case LOAD_BAD_DD_2:
					case LOAD_BAD_DD_3:
						iFirstSectorsSize = 0x100;
						break;

					default:
						break;
						
				}
					
			} //end of repair

			#endif
		}
	}

	LONG lTotalComputedLen = (LONG) ( ( iSectors - 3 ) * head.wSecSize + ATR_HEAD_LEN + 3 * iFirstSectorsSize );

	if ( lTotalComputedLen != lFileLen )
	{
		sprintf( m_szLastError, "ATR: Invalid length! %08lX <> %08lX (%08X)", lTotalComputedLen, lFileLen, iSectors );
		m_iErrorCode = CATR_FORMAT_VIOLATED;

		#ifdef __CDISK_NOREPAIR__
			return FALSE;
		#else
		if ( !bRepair || ( load_method != LOAD_OK ) )
			return FALSE;
		else
		{
			if ( !bRepairAuto )
			{
				printf( "ATR with invalid length encountered.\n" );
				printf( "Should be: $%08lX. Is: $%08lX.\n", lTotalComputedLen, lFileLen );
				printf( "Choose:\n" );
				printf( "1) Change file length (shorten/pad)\n" );
				printf( "2) Change header info\n" );
				printf( "3) Don't repair\n" );

				int iMethod;

				do
				{
					iMethod = getch() - '0';
				} while( ( iMethod < 1 ) || ( iMethod > 3 ) );

				switch( iMethod )
				{
					case 1:
						load_method = LOAD_PAD;
						break;

					case 2:
						load_method = LOAD_OK;
						if ( lFileLen > 0x180 )
						{
							iSectors = ( ( lFileLen - 0x180 ) / head.wSecSize ) + 3;
						}
						else
						{
							iSectors = lFileLen / 0x80;
						}

						break;

					default:
					case 3:
						return FALSE;
						break;
				}
			}
			else
			{
				if ( load_method == LOAD_OK )
					load_method = LOAD_PAD;
			}

		}
		#endif

	}

	DISK_GEOMETRY dg;

	GuessClassicSizes( iSectors, head.wSecSize, &dg );

	if ( !Format( &dg ) )
		return FALSE;

	BYTE abtBuff[ 0x100 ];
	memset( abtBuff, 0, 0x100 );

	for( int i = 0; i < iSectors; i++ )
	{
		switch( load_method )
		{
			default:
			case LOAD_OK:
				cf.Read( abtBuff, ( i < 3 ) ? 0x80 : head.wSecSize );
				break;

			case LOAD_PAD:
				memset( abtBuff, 0, 0x100 );
				cf.Read( abtBuff, ( i < 3 ) ? 0x80 : head.wSecSize );
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

		WriteSector( i + 1, abtBuff );
	}

	cf.Close();
	return TRUE;

}

#endif

#ifdef __CDISK_SAVE__

BOOL CAtr::Save( char* szOutFile, BOOL bOverWrite )
{
	CFile cf;

	if ( !bOverWrite && !access( szOutFile, F_OK ) )
	{
		sprintf( m_szLastError, "ATR: File already exists! '%s'", szOutFile );
		return FALSE;
	}

	if ( !cf.Create( szOutFile ) )
	{
		sprintf( m_szLastError, "ATR: Can't create '%s'", szOutFile );
		return FALSE;
	}

	ATRhead head;
	memset( &head, 0, sizeof( ATRhead ) );

	head.wMagic = ATR_MAGIC;
	head.wSecSize = m_geometry.iBytesPerSector;

	BOOL bReadOnly = TRUE;
	head.btFlags |= ( bReadOnly ) ? 0x01 : 0x00;

	DWORD dwLength = 0;

	dwLength = 0x180 + ( m_geometry.iSectors - 3 ) * m_geometry.iBytesPerSector;

	dwLength >>= 4;

	head.wPars = dwLength & 0xFFFF;
	head.btParHigh = dwLength >> 0x10;

	cf.writeLEw( head.wMagic );	
	cf.writeLEw( head.wPars );	
	cf.writeLEw( head.wSecSize );	
	cf.writeb( head.btParHigh );	
	cf.writeLEdw( head.dwCRC ); 
	cf.writeLEdw( head.dwUnused	); 
	cf.writeb( head.btFlags );	

	BYTE abtBuff[ 0x100 ];

	for( WORD i = 1; i <= m_geometry.iSectors; i++ )
	{
		ReadSector( abtBuff, i );

		int iToWrite = ( i <= 3 ) ? 0x80: m_geometry.iBytesPerSector;

		int iWritten;

		if ( !cf.Write( abtBuff, iToWrite, &iWritten ) || ( iToWrite != iWritten ) )
		{
			sprintf( m_szLastError, "ATR: Can't write!" );
			cf.Close();
			unlink( szOutFile );
			return FALSE;
			
		}
	}

	cf.Close();

	return TRUE;
}

#endif //__CDISK_WRITE__
