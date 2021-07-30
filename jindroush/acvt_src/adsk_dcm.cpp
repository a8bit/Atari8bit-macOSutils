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

#include "adsk_dcm.h"
#include "cfile.h"
#include "cprefile.h"
#include "autil.h"

//#define _DCM_DUMP_

#define DCM_CHANGE_BEGIN    0x41		//Change only start of sector  
#define DCM_DOS_SECTOR      0x42		//128 byte compressed sector   
#define DCM_COMPRESSED      0x43		//Uncompressed/compressed pairs
#define DCM_CHANGE_END      0x44		//Change only end of sector    
#define DCM_PASS_END   		 0x45		//End of pass
#define DCM_SAME_AS_BEFORE  0x46		//Same as previous non-zero    
#define DCM_UNCOMPRESSED    0x47		//Uncompressed sector          

#define DCM_HEADER_SINGLE	0xFA
#define DCM_HEADER_MULTI	0xF9

#define DCM_DENSITY_SD		0			//Single density, 90K          
#define DCM_DENSITY_DD		1			//Double density, 180K         
#define DCM_DENSITY_ED		2			//Enhanced density, 130K       

CDcm::CDcm() : ADisk()
{
	#ifdef _MEMORY_DUMP_
		printf( "CDcm constructed: %p\n", this );
	#endif
}

CDcm::~CDcm()
{
	#ifdef _MEMORY_DUMP_
		printf( "CDcm destructed: %p\n", this );
	#endif
}

#ifndef __CDISK_NOLOAD__
BOOL CDcm::Load( char* szFname, BOOL, BOOL )
{
	BYTE	btArcType = 0;		//Block type for first block
	BYTE	btBlkType;		//Current block type

	m_bAlreadyFormatted = FALSE;
	m_bLastPass = FALSE;
	m_wCurrentSector = 0;

	CFile cfo;
	CPreFile cf;

	if ( !cfo.Open( szFname ) )
	{
		sprintf( m_szLastError, "DCM: Can't open '%s'", szFname );
		m_iErrorCode = CDISK_ERROR_CANT_OPEN;
		return FALSE;
	}

	strcpy( m_szFname, szFname );

	cf.Open( &cfo, 20 );

	m_lFileLength = cf.GetLength();

	for(;;) //outpass
	{
		if ( cf.Tell() >= m_lFileLength )
		{
			if ( ( !m_bLastPass ) && ( btArcType == DCM_HEADER_MULTI ) )
			{
				sprintf( m_szLastError,"DCM: Multi-part archive error.\n" \
				"To process these files, you must first combine the files into a single file." );
				m_iErrorCode = CDCM_FORMAT_VIOLATED;
				return FALSE;
			}
		}

		btArcType = cf.readb();

		switch( btArcType )
		{
			case DCM_HEADER_MULTI:
			case DCM_HEADER_SINGLE:
				if ( !DecodeRecFA( cf ) )
					return FALSE;
				break;

			default:
				sprintf( m_szLastError, "DCM: %02X is an unknown header block.\n", btArcType );
				return FALSE;
		}

		for(;;) //inpass
		{
			btBlkType = cf.readb();

			if ( btBlkType == DCM_PASS_END )
				break;

			if ( cf.Tell() >= m_lFileLength )
			{
				sprintf( m_szLastError, "DCM: EOF before end block." );
				m_iErrorCode = CDCM_FORMAT_VIOLATED;
				return FALSE;
			}

			BOOL bRes = TRUE;

			*m_szLastError = '\0';
			switch( btBlkType & 0x7F )
			{
				case DCM_CHANGE_BEGIN:
					bRes = DecodeRec41( cf );
					break;

				case DCM_DOS_SECTOR:
					bRes = DecodeRec42( cf );
					break;

				case DCM_COMPRESSED:
					bRes = DecodeRec43( cf );
					break;

				case DCM_CHANGE_END:
					bRes = DecodeRec44( cf );
					break;

				case DCM_SAME_AS_BEFORE:
					//not needed
					//bRes = DecodeRec46( cf );
					break;

				case DCM_UNCOMPRESSED:
					bRes = DecodeRec47( cf );
					break;

				default:
				{
					switch( btBlkType )
					{
						case DCM_HEADER_MULTI:
						case DCM_HEADER_SINGLE:
							sprintf( m_szLastError, "DCM: Trying to start section but last section never had "
							"an end section block.");
							break;

						default:
							sprintf( m_szLastError, "DCM: %02X is an unknown block type. File may be "
							"corrupt.",btBlkType);
							break;
					}

					m_iErrorCode = CDCM_FORMAT_VIOLATED;
					return FALSE;
				}
			}

			if ( !bRes )
			{
				sprintf( m_szLastError, "DCM: Block %02X decode error!", btBlkType );
				m_iErrorCode = CDCM_FORMAT_VIOLATED;
				return FALSE;
			}

			if ( !WriteSector( m_wCurrentSector, m_abtCurrBuff ) )
				return FALSE;

			if ( btBlkType & 0x80 )
				m_wCurrentSector++;
			else
				m_wCurrentSector = cf.readLEw();

		} //infinite for (inpass)

		//End block
		if ( m_bLastPass )
			break;

	} //infinite for (outpass)

	cf.Close();
	cfo.Close();
	return TRUE;

}

BOOL CDcm::DecodeRec41( CGenFile& cf )
{
	#ifdef _DCM_DUMP_
	printf( "dec41: %08lX\n", cf.Tell() - 1 );
	#endif

	int iOffset = cf.readb();
	BYTE* pbt = m_abtCurrBuff + iOffset;

	do
	{
		*( pbt-- ) = cf.readb();
	} while( iOffset-- );

	return TRUE;
}

BOOL CDcm::DecodeRec42( CGenFile& cf )
{
	#ifdef _DCM_DUMP_
	printf( "dec42: %08lX\n", cf.Tell() - 1 );
	#endif

	sprintf( m_szLastError, "DCM: Record type 0x42 untested. Uncomment?" );
	return FALSE;

	//TODO: uncomment later!
	//cf.Read( m_abtCurrBuff + 123, 5 );
	//memset( m_abtCurrBuff, m_abtCurrBuff[ 123 ], 123 );
	//return TRUE;
}

BOOL CDcm::DecodeRec43( CGenFile& cf )
{
	#ifdef _DCM_DUMP_
	printf( "dec43: %08lX\n", cf.Tell() - 1 );
	#endif

	BYTE* pbtP = m_abtCurrBuff;
	BYTE* pbtE;

	BYTE* pbtEnd = m_abtCurrBuff + m_iSectorSize;

	do
	{
		//uncompressed string
		if ( pbtP != m_abtCurrBuff )
			pbtE = m_abtCurrBuff + ReadOffset( cf );
		else
			pbtE = m_abtCurrBuff + cf.readb();

		if ( pbtE < pbtP )
			return FALSE;

		#ifdef _DCM_DUMP_
		printf( "dec43: uncst: %p %p %ld\n", pbtP, pbtE, pbtE - pbtP );
		#endif

		if ( pbtE != pbtP )
		{
			cf.Read( pbtP, pbtE - pbtP );
			pbtP = pbtE;
		}

		if ( pbtP >= pbtEnd )
			break;

		//rle compressed string
		pbtE = m_abtCurrBuff + ReadOffset( cf );
		BYTE c = cf.readb();

		#ifdef _DCM_DUMP_
		printf( "dec43:   cst: %p %p %ld\n", pbtP, pbtE, pbtE - pbtP );
		#endif

		if ( pbtE < pbtP )
			return FALSE;

		memset( pbtP, c, pbtE - pbtP );
		pbtP = pbtE;

	} while( pbtP < pbtEnd );

	return TRUE;
}

BOOL CDcm::DecodeRec44( CGenFile& cf )
{
	#ifdef _DCM_DUMP_
	printf( "dec44: %08lX\n", cf.Tell() - 1 );
	#endif

	int iOffset = ReadOffset( cf );

	cf.Read( m_abtCurrBuff + iOffset, m_iSectorSize - iOffset );

	return TRUE;
}

BOOL CDcm::DecodeRec46( CGenFile& cf )
{
	#ifdef _DCM_DUMP_
	printf( "dec46: %08lX\n", cf.Tell() - 1 );
	#endif

	return TRUE;
}

BOOL CDcm::DecodeRec47( CGenFile& cf )
{
	#ifdef _DCM_DUMP_
	printf( "dec47: %08lX\n", cf.Tell() - 1 );
	#endif

	//TODO: Is this TRUE or NOT???
	//cf.Read( m_abtCurrBuff, ( m_wCurrentSector < 4 ? 128 : m_iSectorSize ) );

	cf.Read( m_abtCurrBuff, m_iSectorSize );
	return TRUE;
}

BOOL CDcm::DecodeRecFA( CGenFile& cf )
{
	#ifdef _DCM_DUMP_
	printf( "decFA: %08lX\n", cf.Tell() - 1 );
	#endif

	BYTE btPom = cf.readb();

	BYTE btDensity = ( btPom >> 5 ) & 0x03;
	//BYTE btPass = btPom & 0x1F;
	m_bLastPass = ( btPom & 0x80 ) ? TRUE : FALSE;

	int iSpT;
	int iTracks;

	switch( btDensity )
	{
		case DCM_DENSITY_SD:
			iTracks = 40;
			iSpT = 18;
			m_iSectorSize = 128;
			break;

		case DCM_DENSITY_DD:
			iTracks = 40;
			iSpT = 18;
			m_iSectorSize = 256;
			break;

		case DCM_DENSITY_ED:
			iTracks = 40;
			iSpT = 26;
			m_iSectorSize = 128;
			break;

		default:
			sprintf( m_szLastError,"DCM: Density type unknown (%02X)\n", btDensity );
			return FALSE;
	}

	if ( !m_bAlreadyFormatted )
	{
		DISK_GEOMETRY dg;
		dg.iSides = 1;
		dg.iTracks = iTracks;
		dg.iSectorsPerTrack = iSpT;
		dg.iBytesPerSector = m_iSectorSize;

		if ( !Format( &dg ) )
			return FALSE;

		m_bAlreadyFormatted = TRUE;
	}
	
	m_wCurrentSector = cf.readLEw();

	return TRUE;
}

WORD CDcm::ReadOffset( CGenFile& cf )
{
	BYTE bt = cf.readb();

	return( bt ? bt : 256 );
}
#endif

#ifdef __CDISK_SAVE__

BOOL CDcm::Save( char* szOutFile, BOOL bOverWrite )
{
	if ( !bOverWrite && !access( szOutFile, F_OK ) )
	{
		sprintf( m_szLastError, "DCM: File already exists! '%s'", szOutFile );
		return FALSE;
	}

	int iDensity = -1;
	m_iSectorSize = m_geometry.iBytesPerSector;

	if ( m_iSectorSize == 0x80 )
	{
		if ( m_geometry.iTracks == 40 )
		{
			if ( m_geometry.iSectorsPerTrack == 18 )
				iDensity = DCM_DENSITY_SD;

			if ( m_geometry.iSectorsPerTrack == 26 )
				iDensity = DCM_DENSITY_ED;
		}
	}
	
	if ( m_iSectorSize == 0x100 )
	{
		if ( ( m_geometry.iSectorsPerTrack == 18 ) &&
			( m_geometry.iTracks == 40 ) )
			iDensity = DCM_DENSITY_DD;
	}
	
	if ( iDensity == - 1 )
	{
		sprintf( m_szLastError, "DCM: Can't work with such density!" );
		return FALSE;
	}

	int iPass = 1;

	m_pbtPass = new BYTE [ 0x6500 ];

	CFile cf;

	if ( !cf.Create( szOutFile ) )
	{
		sprintf( m_szLastError, "DCM: Can't create '%s'", szOutFile );
		delete [] m_pbtPass;
		return FALSE;
	}

	int iFirstSector = 0;
	int iPrevSector = 0;
	int iCurrentSector = 1;

	memset( m_abtPrevBuff, 0, m_iSectorSize );

	EncodeRecFA( FALSE, iPass, iDensity, iFirstSector );

	//here should be other compression

	while( iCurrentSector <= m_geometry.iSectors )
	{
		iFirstSector = 0;

		while( ( m_pbtCurr - m_pbtPass ) < 0x5EFD )
		{
			if ( iCurrentSector > m_geometry.iSectors )
				break;

			ReadSector( m_abtCurrBuff, iCurrentSector );

			BOOL bSkip = IsBlockEmpty( m_abtCurrBuff, m_iSectorSize );

			//first non empty sector is marked as first, what a surprise! :)
			if ( !bSkip && !iFirstSector )
			{
				iFirstSector = iCurrentSector;
				iPrevSector = iCurrentSector;
			}

			//if just skipped, increment sector
			if ( bSkip )
			{
				iCurrentSector++;
			}
			else
			{
				//if there is a gap, write sector number
				if ( ( iCurrentSector - iPrevSector ) > 1 )
				{
					*( m_pbtCurr++ ) = iCurrentSector;
					*( m_pbtCurr++ ) = iCurrentSector >> 8;
				}
				else
				{
					//else mark previous record
					*m_pbtLastRec |= 0x80;
				}

				//first sector could be encoded with only some data
				if ( iCurrentSector == iFirstSector )
					EncodeRec( TRUE );
				else
				{
					//if are same, encode as record 46
					if ( !memcmp( m_abtPrevBuff, m_abtCurrBuff, m_iSectorSize ) )
						EncodeRec46();
					else
						EncodeRec( FALSE );
				}

				//store this sector as previous
				memcpy( m_abtPrevBuff, m_abtCurrBuff, m_iSectorSize );

				//and move pointers
				iPrevSector = iCurrentSector;
				iCurrentSector++;
			}

		}

		//mark previous sector
		*m_pbtLastRec |= 0x80;

		//encode end
		EncodeRec45();

		BYTE* pEnd = m_pbtCurr;

		//change beginning block
		if ( iCurrentSector > m_geometry.iSectors )
			EncodeRecFA( TRUE, iPass, iDensity, iFirstSector );
		else
			EncodeRecFA( FALSE, iPass, iDensity, iFirstSector );

		//and write whole pass

		if ( ( pEnd - m_pbtPass ) > 0x6000 )
		{
			sprintf( m_szLastError, "DCM: Internal error! Pass too long!" );
			delete [] m_pbtPass;
			cf.Close();
			unlink( szOutFile );
			return FALSE;
		}

		if ( !cf.Write( m_pbtPass, pEnd - m_pbtPass ) )
		{
			sprintf( m_szLastError, "DCM: Can't write!" );
			delete [] m_pbtPass;
			cf.Close();
			unlink( szOutFile );
			return FALSE;
		}

		iPass++;
	}

	cf.Close();

	delete [] m_pbtPass;

	return TRUE;
}

void CDcm::EncodeRecFA( BOOL bLast, int iPass, int iDensity, int iFirstSec )
{
	m_pbtCurr = m_pbtPass;
	
	#ifdef _DCM_DUMP_
	printf( "ERFA: %08lX\n", m_pbtCurr - m_pbtPass );
	#endif

	m_pbtLastRec = m_pbtCurr;

	BYTE btType = bLast ? 0x80 : 0;

	btType |= ( iDensity & 3 ) << 5;

	btType |= ( iPass & 0x1F );

	*( m_pbtCurr++ ) = DCM_HEADER_SINGLE;
	*( m_pbtCurr++ ) = btType;
	*( m_pbtCurr++ ) = iFirstSec;
	*( m_pbtCurr++ ) = iFirstSec >> 8;

}

void CDcm::EncodeRec45()
{
	#ifdef _DCM_DUMP_
	printf( "ER45: %08lX\n", m_pbtCurr - m_pbtPass );
	#endif

	m_pbtLastRec = m_pbtCurr;
	*( m_pbtCurr++ ) = DCM_PASS_END;
}

void CDcm::EncodeRec46()
{
	#ifdef _DCM_DUMP_
	printf( "ER46: %08lX\n", m_pbtCurr - m_pbtPass );
	#endif
	
	m_pbtLastRec = m_pbtCurr;
	*( m_pbtCurr++ ) = DCM_SAME_AS_BEFORE;
}

void CDcm::EncodeRec( BOOL bIsFirstSector )
{
	#ifdef _DCM_DUMP_
	printf( "ER: %08lX\n", m_pbtCurr - m_pbtPass );
	#endif
	
	m_pbtLastRec = m_pbtCurr;

	BYTE abtBuff41[ 0x300 ];
	BYTE abtBuff43[ 0x300 ];
	BYTE abtBuff44[ 0x300 ];
	BYTE* abtBuff47 = m_abtCurrBuff;

	int iEnd41 = 0x300;
	int iEnd43 = 0x300;
	int iEnd44 = 0x300;

	int iBestMethod = DCM_UNCOMPRESSED;
	int iBestEnd = m_iSectorSize;
	BYTE* pbtBest = abtBuff47;

	EncodeRec43( abtBuff43, &iEnd43, m_abtCurrBuff, m_iSectorSize );

	if ( !bIsFirstSector )
	{
		EncodeRec41( abtBuff41, &iEnd41, m_abtCurrBuff, m_abtPrevBuff, m_iSectorSize );
		EncodeRec44( abtBuff44, &iEnd44, m_abtCurrBuff, m_abtPrevBuff, m_iSectorSize );
	}

	if ( iEnd41 < iBestEnd )
	{
		iBestMethod = DCM_CHANGE_BEGIN;
		iBestEnd = iEnd41;
		pbtBest = abtBuff41;
	}

	if ( iEnd43 < iBestEnd )
	{
		iBestMethod = DCM_COMPRESSED;
		iBestEnd = iEnd43;
		pbtBest = abtBuff43;
	}

	if ( iEnd44 < iBestEnd )
	{
		iBestMethod = DCM_CHANGE_END;
		iBestEnd = iEnd44;
		pbtBest = abtBuff44;
	}

	*( m_pbtCurr++ ) = iBestMethod;
	memcpy( m_pbtCurr, pbtBest, iBestEnd );
	m_pbtCurr += iBestEnd;
}

void CDcm::EncodeRec41( BYTE* pbtDest, int* piDestLen, BYTE* pbtSrc, BYTE* pbtSrcOld, int iSrcLen )
{
	BYTE* pbtS = pbtSrc + iSrcLen - 1;
	pbtSrcOld += iSrcLen - 1;

	BYTE* pbtD = pbtDest;

	for( int i = 0; i < iSrcLen; i++ )
	{
		if ( *( pbtS-- ) != * ( pbtSrcOld-- ) )
			break;
	}

	pbtS++;

	*( pbtD++ ) = pbtS - pbtSrc;

	int iBytes = pbtS - pbtSrc + 1;

	while( iBytes-- )
	{
		*( pbtD++ ) = *( pbtS-- );
	}

	*piDestLen = pbtD - pbtDest;
}

void CDcm::EncodeRec43( BYTE* pbtDest, int* piDestLen, BYTE* pbtSrc, int iSrcLen )
{
	BYTE* pbtEnd = pbtSrc + iSrcLen;
	BYTE* pbtCur = pbtSrc;

	BYTE* pbtD = pbtDest;

	while( pbtCur < pbtEnd )
	{
		BOOL bFound = FALSE;

		for( BYTE* pbtNow = pbtCur; pbtNow < ( pbtEnd - 2 ); pbtNow++ )
		{

			if ( ( *pbtNow == *(pbtNow+1) ) && ( *pbtNow == *(pbtNow+2) ) )
			{
				int iUnc = pbtNow - pbtCur;
				
				*( pbtD ++ ) = pbtNow - pbtSrc;
				if ( iUnc )
				{
					memcpy( pbtD, pbtCur, iUnc );
					pbtD += iUnc;
				}

				BYTE bt = *pbtNow;
				BYTE*p;
				for( p = pbtNow + 1; p < pbtEnd; p++ )
				{
					if ( *p != bt )
						break;
				}

				if ( p > pbtEnd )
					p = pbtEnd;

				*( pbtD++ ) = p - pbtSrc;
				*( pbtD++ ) = bt;

				pbtCur = p;
				bFound = TRUE;
				break;
			}
		}

		if ( ( pbtCur >= pbtEnd - 2 ) || !bFound ) 
		{
			if ( pbtCur < pbtEnd )
			{
				*( pbtD++ ) = iSrcLen;
				memcpy( pbtD, pbtCur, pbtEnd - pbtCur );
				pbtD += pbtEnd - pbtCur;
			}

			break;
		}

	}

	*piDestLen = pbtD - pbtDest;
}

void CDcm::EncodeRec44( BYTE* pbtDest, int* piDestLen, BYTE* pbtSrc, BYTE* pbtSrcOld, int iSrcLen )
{
	BYTE* pbtS = pbtSrc;
	BYTE* pbtEnd = pbtSrc + iSrcLen;

	BYTE* pbtD = pbtDest;

	for( int i = 0; i < iSrcLen; i++ )
	{
		if ( *( pbtS++ ) != * ( pbtSrcOld++ ) )
			break;
	}

	pbtS--;

	*( pbtD++ ) = pbtS - pbtSrc;
	memcpy( pbtD, pbtS, pbtEnd - pbtS );
	pbtD += pbtEnd - pbtS;

	*piDestLen = pbtD - pbtDest;
}

#endif // __CDISK_SAVE__

