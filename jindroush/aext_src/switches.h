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

//THIS IS A GENERATED FILE. DO NOT EDIT!!!
//EDIT switches.def INSTEAD!


int m_iArgsCurr;
int m_iArgsOrig;
char** m_ppcArgvOrig;
char** m_ppcArgvCurr;
char* m_szSwitch;

//gets one token from cmdline
BOOL SWITCHES_GetSwitch()
{
	if ( !m_iArgsCurr )
		return FALSE;
	
	m_szSwitch = *( m_ppcArgvCurr++ );
	
	m_iArgsCurr--;
	
	return TRUE;
}

//gets one token, returns TRUE if value
BOOL SWITCHES_GetValue()
{
	if ( !SWITCHES_GetSwitch() )
		return FALSE;
	
	switch( *m_szSwitch )
	{
		case '-':
		case '/':
			return FALSE;
		default:
			return TRUE;
	}
}


BOOL SWFN_HELP( char* szUsage )
{
	if ( szUsage )
		printf( "%s\n", szUsage );
	printf( "-help         -this help\n" );
	printf( "-e hexnum     -end\n" );
	printf( "-s hexnum     -start\n" );
	printf( "-n hexnum     -length\n" );
	printf( "-oa           -output is Atari block\n" );
	printf( "-base hexnum  -base\n" );
	return FALSE;
}


BOOL SWFN_NUMH( int* piNum )
{
	char* szOrigSw = m_szSwitch;
	
	if ( !SWITCHES_GetSwitch() )
	{
		printf( "Missing value for switch '%s'\n", szOrigSw );
		return FALSE;
	}
	
	sscanf( m_szSwitch, "%X", piNum );
	
	return TRUE;
}

BOOL SWFN_GETPATH( char* szPath )
{
	char* szOrigSw = m_szSwitch;
	
	if ( !SWITCHES_GetSwitch() )
	{
		printf( "Missing value for switch '%s'\n", szOrigSw );
		return FALSE;
	}
	
	strcpy( szPath, m_szSwitch );
	return TRUE;
}


BOOL SWITCHES_Init( int* pargc, char** argv )
{
	BOOL bRet = TRUE;
	
	m_iArgsCurr = *pargc - 1;
	m_iArgsOrig = 1;
	m_ppcArgvOrig = argv + 1;
	m_ppcArgvCurr = argv + 1;
	
	while( m_iArgsCurr )
	{
		if ( !bRet )
			break;
		
		if ( SWITCHES_GetValue() )
		{
			bRet = TRUE;
			*( m_ppcArgvOrig++ ) = * ( m_ppcArgvCurr - 1 );
			m_iArgsOrig++;
		}
		else
		{
			m_szSwitch++;
			
			bRet = TRUE;
			
			if (!strcmp( m_szSwitch, "help" ) || !strcmp( m_szSwitch, "?" ))
			{
				bRet = SWFN_HELP( USAGE );
				continue;
			}
			if (!strcmp( m_szSwitch, "e" ) || !strcmp( m_szSwitch, "end" ))
			{
				bRet = SWFN_GETPATH( szEnd );g_bEndUsed = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "s" ) || !strcmp( m_szSwitch, "start" ))
			{
				bRet = SWFN_GETPATH( szStart );g_bStartUsed = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "n" ) || !strcmp( m_szSwitch, "length" ))
			{
				bRet = SWFN_GETPATH( szLength );g_bLenUsed = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "oa" ) || !strcmp( m_szSwitch, "atari" ))
			{
				g_bOutputIsAtari = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "base" ))
			{
				bRet = SWFN_NUMH( &g_iBlkBase );
				g_bUseBase = TRUE;
				continue;
			}
			
			printf( "Invalid switch: %s\n", *( m_ppcArgvCurr - 1 ) );
			return FALSE;
		} //endif
	} //while
	
	*pargc = m_iArgsOrig;
	return bRet;
}
