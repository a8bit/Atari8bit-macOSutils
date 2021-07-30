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

#include "cfile.h"

CFile::CFile()
{
	m_bOpened = FALSE;
	m_hFile = 0;
	m_bIsDirty = FALSE;
}

CFile::~CFile()
{
	Close();
}

//opens file for reading, binary
BOOL CFile::Open( char* szName )
{
	if ( m_bOpened )
		return FALSE;

	m_hFile = open( szName, O_BINARY | O_RDONLY );

	if ( m_hFile == -1 )
	{
		m_hFile = 0;

		//printf( "CFile::Open %s Err:%d\n", szName, errno );

		return FALSE;
	}

	m_bOpened = TRUE;

	m_lLength = lseek( m_hFile, 0, SEEK_END );
	lseek( m_hFile, 0, SEEK_SET );

	return TRUE;

}

//creates new file, opened  for rdwr, binary, deletes existing
BOOL CFile::Create( char* szName )
{
	if ( m_bOpened )
		return FALSE;

	m_bIsDirty = FALSE;
	m_hFile = open( szName, O_BINARY | O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE );

	if ( m_hFile == -1 )
	{
		m_hFile = 0;
		return FALSE;
	}

	m_bOpened = TRUE;

	m_lLength = lseek( m_hFile, 0, SEEK_END );
	lseek( m_hFile, 0, SEEK_SET );

	return TRUE;

}

//reads iBytesToRead bytes to buffer
BOOL CFile::Read( void* pBuff, int iBytesToRead, int* piBytesRead )
{
	if ( !m_bOpened )
		return FALSE;

	m_bIsDirty = FALSE;
	int iBytesRead = read( m_hFile, pBuff, iBytesToRead );

	if ( piBytesRead )
		*piBytesRead = iBytesRead;

	if ( -1 == iBytesRead )
		return FALSE;
	
	m_lCurrPtr = tell( m_hFile );

	return TRUE;
}

//writes iBytesToWrite from buffer
//TODO:check for file being opened for writing
BOOL CFile::Write( void* pBuff, int iBytesToWrite, int* piBytesWritten )
{
	if ( !m_bOpened )
		return FALSE;

	int iBytesWritten = write( m_hFile, pBuff, iBytesToWrite );

	if ( piBytesWritten )
		*piBytesWritten = iBytesWritten;

	if ( -1 == iBytesWritten )
		return FALSE;

	m_lCurrPtr = tell( m_hFile );
	m_bIsDirty = TRUE;

	return TRUE;
}

//seek
BOOL CFile::Seek( long lPos, int iType )
{
	m_lCurrPtr = lseek( m_hFile, lPos, iType );

	return TRUE;
}

//closes the file
void CFile::Close()
{
	if ( m_hFile )
		close( m_hFile );

	m_bIsDirty = FALSE;
	m_hFile = 0;
	m_bOpened = FALSE;
}

