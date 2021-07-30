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

#include "cprefile.h"

CPreFile::CPreFile()
{
	m_lCacheStartOffset = -1;
	m_lCacheEndOffset = -1;
	m_bOpened = FALSE;
	m_pbtCache = NULL;
}

CPreFile::~CPreFile()
{
	Close();
}

BOOL CPreFile::Open( CGenFile* pcf, int iCacheSizeKB )
{
	m_lCacheLen = 1024 * iCacheSizeKB;

	if ( m_bOpened )
		return FALSE;

	m_pcf = pcf;

	m_pcf->Seek( 0, SEEK_END );

	m_lLength = m_pcf->Tell();

	if ( m_lLength < m_lCacheLen )
	{
		m_lCacheLen = m_lLength;
	}

	if ( ! ( m_pbtCache = new BYTE [ m_lCacheLen ] ) )
		return FALSE;

	m_pcf->Seek( 0, SEEK_SET );

	m_lCurrPtr = 0;

	m_bOpened = TRUE;

	PreRead();

	return TRUE;

}

BOOL CPreFile::PreRead()
{
	m_pcf->Seek( m_lCurrPtr, SEEK_SET );
	m_lCacheStartOffset = m_lCurrPtr;
	m_lCacheEndOffset = m_lCacheStartOffset + m_lCacheLen - 1;

	m_pcf->Read( m_pbtCache, m_lCacheLen );

	return TRUE;
}

BOOL CPreFile::Read( void* pBuff, int iLen, int* piLen )
{
	if ( !m_bOpened )
		return FALSE;

	if ( m_lCurrPtr > m_lLength )
		return FALSE;

	if ( iLen >= m_lCacheLen )
	{
		m_pcf->Seek( m_lCurrPtr, SEEK_SET );
		if ( !m_pcf->Read( pBuff, iLen, piLen ) )
			return FALSE;

		m_lCurrPtr = m_pcf->Tell();
		m_lCacheEndOffset = -1;
		return TRUE;
	}
	
	if ( ( m_lCurrPtr < m_lCacheStartOffset ) || ( m_lCurrPtr > m_lCacheEndOffset ) )
		PreRead();

	long lReadEnd = m_lCurrPtr + iLen - 1;

	if ( lReadEnd >= m_lCacheEndOffset )
	{
		m_pcf->Seek( m_lCurrPtr, SEEK_SET );
		if ( !m_pcf->Read( pBuff, iLen, piLen ) )
			return FALSE;

		m_lCurrPtr = m_pcf->Tell();
		m_lCacheEndOffset = -1;

		return TRUE;
	}

	memcpy( pBuff, m_pbtCache + m_lCurrPtr - m_lCacheStartOffset, iLen );
	m_lCurrPtr += iLen;

	return TRUE;
}

BOOL CPreFile::Write( void* pBuff, int iLen, int* piLen )
{
	return FALSE;
}

BOOL CPreFile::Seek( long lPos, int iType )
{

	switch( iType )
	{
		case SEEK_SET:
			m_lCurrPtr = lPos;
			break;

		case SEEK_CUR:
			m_lCurrPtr += lPos;
			break;

		case SEEK_END:
			m_lCurrPtr = m_lLength - lPos;
			break;

	}
	return TRUE;
}

void CPreFile::Close()
{
	m_bOpened = FALSE;

	if ( m_pbtCache )
		delete [] m_pbtCache;

	m_pbtCache = NULL;
	m_lCurrPtr = 0;
}

