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

#include "autil.h"

void Peecize( char* szPC, char* szAtari );

//converts 8+3 Atari fname to it's PC equivalent
void ADos2MsDos( char* szMsDos, char* szADos )
{
	char szName[ 9 ];
	strncpy( szName, szADos, 8 );
	szName[ 8 ] = '\0';

	char szExt[ 4 ];
	strncpy( szExt, szADos + 8, 3 );
	szExt[ 3 ] = '\0';

	Peecize( szName, szName );
	Peecize( szExt, szExt );

	if ( !*szName )
		strcpy( szName, "out" );

	strcpy( szMsDos, szName );

	if ( *szExt )
	{
		strcat( szMsDos, "." );
		strcat( szMsDos, szExt );
	}
}

//kind of 'good-for-dos' filter
void Peecize( char* szPC, char* szAtari )
{
	int iLen = strlen( szAtari );

	while( iLen-- )
	{
		//filter off the inverse chars
		char c = *( szAtari++ ) & 0x7F;

		//filter spaces
		if ( c == ' ' )
			continue;

		//filter unprintables
		if ( !isprint( c ) )
			c = '_';

		//filter other 'ugly' characters :)
		switch( c )
		{
			case '*':
			case ':':
			case '\"':
			case ',':
			case '.':
			case '|':
			case '?':
			case '/':
			case '\\':
				c = '_';
				break;
		}

		*( szPC++ ) = c;
	}

	*szPC = '\0';
}


//converts any Atari name to it's PC equivalent
void GuessBestFnameFromAtari( char* szDest, char* szSrc, char* szExt )
{
	char szTemp[ 255 ];
	Peecize( szTemp, szSrc );
	GuessBestFnameFromPC( szDest, szTemp, szExt );
}

//changes file extension
void GuessBestFnameFromPC( char* szDest, char* szSrc, char* szExt, char* szAdd )
{
	char szTemp[ 255 ];

	char szGoodSrc[ 255 ];

	_fixpath( szSrc, szGoodSrc );

	char* szSl = strrchr( szGoodSrc, '/' );

	if ( szSl )
		strcpy( szTemp, szSl + 1 );
	else
		strcpy( szTemp, szGoodSrc );

	char* szDot = strrchr( szTemp, '.' );

	if ( szDot )
		*szDot = '\0';

	if ( !strlen( szTemp ) )
		strcpy( szTemp, "out" );

	strcpy( szDest, szTemp );

	if ( szAdd )
		strcat( szDest, szAdd );

	strcat( szDest, "." );
	strcat( szDest, szExt );
}

//is memory block empty?
BOOL IsBlockEmpty( BYTE* p, int iLen )
{
	while( iLen-- )
	{
		if ( *( p++ ) )
			return FALSE;
	}

	return TRUE;
}

