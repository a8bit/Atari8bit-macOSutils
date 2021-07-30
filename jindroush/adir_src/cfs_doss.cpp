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

#include "cfs_doss.h"
#include "autil.h"

#define SP3TOLONG(dire) ( ( dire.btSizeHi << 16 ) + dire.wSizeLo )

CDosS::CDosS() : CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CDosS constructed: %08X\n", this );
	#endif
}

CDosS::~CDosS()
{
	Dismount();
	#ifdef _MEMORY_DUMP_
		printf( "CDosS destructed: %08X\n", this );
	#endif
}

BOOL CDosS::Mount( ADisk* pDisk )
{
	m_iFilesValid = 0;
	m_iFilesInvalid = 0;

	m_pDisk = pDisk;
	m_pRoot = NULL;

	if ( m_pDisk->GetBootSectorCount() != 3 )
	{
 		sprintf( m_szLastError, "DOSS: Strange boot sector!" );
		return FALSE;
	}

	int iSize = m_pDisk->GetBootSectorSize();

	BYTE* pBoot = new BYTE [ iSize ];

	if ( !pBoot )
	{
 		sprintf( m_szLastError, "DOSS: Not enough memory to allocate boot sector!" );
		return FALSE;
	}

	if ( !m_pDisk->GetBootSector( pBoot ) )
	{
		sprintf( m_szLastError, "DOSS: Can't read boot sector because\n%s", m_pDisk->GetLastError() );
		delete [] pBoot;
		return FALSE;
	}

	BYTE btSpDosIdent = pBoot[ 7 ];

	if ( btSpDosIdent != 0x80 )
	{
		sprintf( m_szLastError, "DOSS: Mismatched boot." );
		delete [] pBoot;
		return FALSE;
	}

	m_wMainDirStart = pBoot[ 9 ] + ( pBoot[ 10 ] << 8 );
	//WORD wSectors = pBoot[ 11 ] + ( pBoot[ 12 ] << 8 );
	//WORD wFreeSectors = pBoot[ 13 ] + ( pBoot[ 14 ] << 8 );
	//BYTE btBitmapSectors = pBoot[ 15 ];
	//WORD wFirstBitmapSector = pBoot[ 16 ] + ( pBoot[ 17 ] << 8 );
	//WORD wFreeDataSector = pBoot[ 18 ] + ( pBoot[ 19 ] << 8 );
	//WORD wFreeDirSector = pBoot[ 20 ] + ( pBoot[ 21 ] << 8 );
	//22-29 volume name

	switch( pBoot[ 31 ] )
	{
		case 0:
			m_wSectorSize = 0x100;
			break;

		case 0x80:
			m_wSectorSize = 0x80;
			break;

		default:
		{
 			sprintf( m_szLastError, "DOSS: Unknown sector size %02X!", pBoot[ 31 ] );
			delete [] pBoot;
			return FALSE;
		}

	}

	//BYTE btVersion = pBoot[ 32 ];
	//BYTE btSeqNumber = pBoot[ 38 ];
	//BYTE btRndNumber = pBoot[ 39 ];
	//WORD wDosBootStart = pBoot[ 40 ] + ( pBoot[ 41 ] << 8 );

	delete [] pBoot;

	return ReadDir( m_wMainDirStart, (CDosSDirEntry**)&m_pRoot );
}

int CDosS::GetSectorLinkEntry( int iLink, int iSec )
{
	WORD awSector[ 0x80 ];

	int iCurrLink = iLink;

	int iDivisor = 0;
	switch( m_wSectorSize )
	{
		case 0x80:
			iDivisor = 0x3E;
			break;

		case 0x100:
			iDivisor = 0x7E;
			break;

		default:
			return 0;
			
	}

	int iSecNeeded = ( iSec / iDivisor ) + 1;

	do
	{
		if ( !m_pDisk->ReadSector( awSector, iCurrLink ) )
		{
			sprintf( m_szLastError, "DOSS: Can't read link sector because\n%s", m_pDisk->GetLastError() );
			return 0;
		}

		iSecNeeded--;
		//not endian-safe
		//iCurrLink = awSector[ 0 ];
		iCurrLink = MREAD_LEW( (BYTE*)awSector );
		if ( iSec >= iDivisor )
			iSec -= iDivisor;

	} while( iSecNeeded );
	 
	//not endian-safe
	//return awSector[ iSec + 2 ];
	return MREAD_LEW( (BYTE*)(awSector + iSec + 2 ));
}

BOOL CDosS::ReadDir( int iSectorLink, CDosSDirEntry** ppRoot )
{
	DOSS_DIRENT dire;

	BYTE abtSector[ 0x100 ];

	int iSector = GetSectorLinkEntry( iSectorLink, 0 );

	if ( !m_pDisk->ReadSector( abtSector, iSector ) )
	{
		sprintf( m_szLastError, "DOSS: Can't read dir because\n%s", m_pDisk->GetLastError() );
		return FALSE;
	}

	//not endian safe
	//memcpy( &dire, abtSector, sizeof( DOSS_DIRENT ) );
	BYTE* pTmp = abtSector;
	dire.btFlags = MGET_B( pTmp );
	dire.wSector = MGET_LEW( pTmp );
	dire.wSizeLo = MGET_LEW( pTmp );
	dire.btSizeHi = MGET_B( pTmp );
	memcpy( dire.acAtariName, pTmp, 11 ); pTmp+= 11;
	memcpy( &dire.btDay, pTmp, 6 ); //dirty hack, copying last 6 byte values

	int iDirLen = SP3TOLONG( dire );
	BYTE* pDir = MapFile( iSectorLink, iDirLen );
	if ( !pDir )
	 	return FALSE;

	int iEntries = iDirLen / sizeof( DOSS_DIRENT );

	//if ( iDirLen != iEntries * (int)sizeof( DOSS_DIRENT ) )
	//{
	//	UnMapFile( pDir );
	//	sprintf( m_szLastError, "DOSS: Dir size mismatch! (%04X<>%04X)\n",
	//		iDirLen,
	//		iEntries * (int)sizeof( DOSS_DIRENT ) 
	//	);
	//	return FALSE;
	//}

	CDosSDirEntry* pPrev = NULL;

	for( int i = 1; i < iEntries; i++ )
	{
		
		DOSS_DIRENT dire2;
		BYTE* pTmp = pDir + i * sizeof( DOSS_DIRENT );
		dire2.btFlags = MGET_B( pTmp );
		dire2.wSector = MGET_LEW( pTmp );
		dire2.wSizeLo = MGET_LEW( pTmp );
		dire2.btSizeHi = MGET_B( pTmp );
		memcpy( dire2.acAtariName, pTmp, 11 ); pTmp+= 11;
		memcpy( &dire2.btDay, pTmp, 6 ); //dirty hack, copying last 6 byte values

		CDosSDirEntry* pE = CreateEntry( &dire2 );

		if ( pE && ! ( pE->m_dwFlags & DIRE_DELETED ) )
		{
			if ( *ppRoot )
			{
				pPrev->m_pNext = pE;
				pE->m_pPrev = pPrev;
				pPrev = pE;
			}
			else
			{
				*ppRoot = pE;
				pPrev = pE;
			}

		}

	}

	UnMapFile( pDir );

	return TRUE;
}

void CDosS::Dismount()
{
	DeleteList( m_pRoot );
}

CDosSDirEntry* CDosS::CreateEntry( DOSS_DIRENT* pDire )
{
	CDosSDirEntry* pE = new CDosSDirEntry();

	if ( !pE )
	{
		sprintf( m_szLastError, "DOSS: Can't allocate memory for directory!" );
		return NULL;
	}

	ADos2MsDos( pE->m_szFname, pDire->acAtariName );

	sprintf( pE->m_szAscData, "%02X %04X %02X%04X %02d-%02d-19%02d %2d:%02d.%02d", 
		pDire->btFlags, 
		pDire->wSector, 
		pDire->btSizeHi, 
		pDire->wSizeLo,
		pDire->btMonth,
		pDire->btDay,
		pDire->btYear,
		pDire->btHour,
		pDire->btMinute,
		pDire->btSecond
	);

	pE->m_iLength = ( pDire->btSizeHi << 16 ) + pDire->wSizeLo;
	pE->m_iLinkSector = pDire->wSector;

	if ( pDire->btFlags == 0x10 )
	{
		pE->m_dwFlags |= DIRE_DELETED;
	}

	if ( ! ( pDire->btFlags && 0x08 ) )
	{
		pE->m_dwFlags |= DIRE_DELETED;
	}

	if ( pDire->btFlags & 0x20 )
	{
		pE->m_dwFlags |= DIRE_SUBDIR;

		BOOL bRes = ReadDir( pE->m_iLinkSector, (CDosSDirEntry**)&pE->m_pSubdir );

		if ( !bRes )
		{
			delete pE;
			return NULL;
		}
	}
	else
	{
		if ( pE->m_dwFlags & DIRE_DELETED )
		{
		}
		else if ( ExportFile( NULL, pE ) )
			m_iFilesValid++;
		else
			m_iFilesInvalid++;
	}

	return pE;
}

BOOL CDosS::ExportFile( char* szOutFile, CDirEntry* pDirE )
{
	int hOutfile = -1;

	if ( szOutFile )
	{
		hOutfile = open( szOutFile, O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE );

		if ( -1 == hOutfile )
		{
			sprintf( m_szLastError, "DOSS: Unable to create file '%s'!", szOutFile );
			return FALSE;
		}
	}

	int iLinkSector = ((CDosSDirEntry*)pDirE ) -> m_iLinkSector;
	int iLength = ((CDosSDirEntry*)pDirE ) -> m_iLength;

	BYTE* pDir = MapFile( iLinkSector, iLength );

	if ( !pDir )
	{
	 	return FALSE;
	}

	if ( -1 != hOutfile )
		write( hOutfile, pDir, iLength );

	UnMapFile( pDir );

	if ( -1 != hOutfile )
		close( hOutfile );

	return TRUE;
}

BYTE* CDosS::MapFile( int iLinkSector, int iLength )
{
	BYTE* pBuff = new BYTE [ iLength ];

	if ( !pBuff )
		return NULL;

	int iLogSector = 0;
	
	BYTE abtBuff[ 0x100 ];
	BYTE * p = pBuff;

	while( iLength )
	{
		int iToCopy = ( iLength > m_wSectorSize ) ? m_wSectorSize : iLength;

		int iSector = GetSectorLinkEntry( iLinkSector, iLogSector );

		if ( !m_pDisk->ReadSector( abtBuff, iSector ) )
		{
				sprintf( m_szLastError, "DOSS: Can't read file sector because\n%s", m_pDisk->GetLastError() );
				delete [] pBuff;
				return 0;
		}

		memcpy( p, abtBuff, iToCopy );
		iLength -= iToCopy;
		p += iToCopy;
		iLogSector++;
	}

	return pBuff;
}

void CDosS::UnMapFile( BYTE * pBuff )
{
	if ( pBuff )
		delete [] pBuff;
}
