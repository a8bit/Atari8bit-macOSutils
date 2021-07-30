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

#include "cgenfile.h"

//writes little-endian dword
void CGenFile::writeLEdw( DWORD dwT )
{
	writeb( dwT & 0xFF );
	writeb( ( dwT >> 8 ) & 0xFF );
	writeb( ( dwT >>16 ) & 0xFF );
	writeb( ( dwT >>24 ) & 0xFF );
}

//writes little-endian word
void CGenFile::writeLEw( WORD wT )
{
	writeb( wT & 0xFF );
	writeb( ( wT >> 8 ) & 0xFF );
}

void CGenFile::writeBEw( WORD wT )
{
	writeb( ( wT >> 8 ) & 0xFF );
	writeb( wT & 0xFF );
}

//reads little-endian word
WORD CGenFile::readLEw()
{
	WORD wT;
	wT = readb();
	wT |= readb() << 8;
	return wT;
}

WORD CGenFile::readBEw()
{
	WORD wT;
	wT = readb() << 8;
	wT |= readb();
	return wT;
}


//reads little-endian dword
DWORD CGenFile::readLEdw()
{
	DWORD dwT;
	dwT = readb();
	dwT |= readb() << 8;
	dwT |= readb() << 16;
	dwT |= readb() << 24;

	return dwT;
}

//writes byte
void CGenFile::writeb( BYTE btT )
{
	Write( &btT, 1, NULL );
}

//reads byte
BYTE CGenFile::readb()
{
	BYTE btPom;
	Read( &btPom, 1, NULL );
	return btPom;
}

//skips part of file
BOOL CGenFile::skip( long lSkip )
{
	return Seek( lSkip, SEEK_CUR );
}

//gets current value of pointer
long CGenFile::Tell()
{
	return m_lCurrPtr;
}

