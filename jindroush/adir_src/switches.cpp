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
	printf( "-dos2         -mount as Dos2\n" );
	printf( "-dos3         -mount as Dos3\n" );
	printf( "-mydos        -mount as MyDos\n" );
	printf( "-spdos        -mount as SpartaDos\n" );
	printf( "-kboot        -mount as Kboot\n" );
	printf( "-bas2boot     -mount as Bas2Boot\n" );
	printf( "-jonw         -mount as JonW\n" );
	printf( "-robc         -mount as RobC\n" );
	printf( "-howf         -mount as HowFen Dos\n" );
	printf( "-boot         -mount as boot\n" );
	printf( "-e            -export files\n" );
	printf( "-dir path     -export to directory\n" );
	printf( "-dontrepair   -don't repair invalid files automatically\n" );
	return FALSE;
}


BOOL SWFN_GETPATH( char* szPath )
{
	char* szOrigSw = m_szSwitch;
	
	if ( !SWITCHES_GetValue() )
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
			if (!strcmp( m_szSwitch, "dos2" ))
			{
				g_dostype = DOS_2;
				continue;
			}
			if (!strcmp( m_szSwitch, "dos3" ))
			{
				g_dostype = DOS_3;
				continue;
			}
			if (!strcmp( m_szSwitch, "mydos" ) || !strcmp( m_szSwitch, "dosm" ))
			{
				g_dostype = DOS_MY;
				continue;
			}
			if (!strcmp( m_szSwitch, "spdos" ) || !strcmp( m_szSwitch, "sparta" ) || !strcmp( m_szSwitch, "doss" ))
			{
				g_dostype = DOS_SP;
				continue;
			}
			if (!strcmp( m_szSwitch, "kboot" ))
			{
				g_dostype = DOS_KBOOT;
				continue;
			}
			if (!strcmp( m_szSwitch, "bas2boot" ) || !strcmp( m_szSwitch, "b2b" ))
			{
				g_dostype = DOS_B2B;
				continue;
			}
			if (!strcmp( m_szSwitch, "jonw" ))
			{
				g_dostype = DOS_JONW;
				continue;
			}
			if (!strcmp( m_szSwitch, "robc" ))
			{
				g_dostype = DOS_ROBC;
				continue;
			}
			if (!strcmp( m_szSwitch, "howf" ) || !strcmp( m_szSwitch, "howfen" ))
			{
				g_dostype = DOS_HOWF;
				continue;
			}
			if (!strcmp( m_szSwitch, "boot" ))
			{
				g_dostype = DOS_BOOT;
				continue;
			}
			if (!strcmp( m_szSwitch, "e" ))
			{
				g_bListOnly = FALSE;
				continue;
			}
			if (!strcmp( m_szSwitch, "dir" ))
			{
				bRet = SWFN_GETPATH( g_szPath );
				continue;
			}
			if (!strcmp( m_szSwitch, "dontrepair" ))
			{
				g_bRepairAuto = FALSE;
				continue;
			}
			
			printf( "Invalid switch: %s\n", *( m_ppcArgvCurr - 1 ) );
			return FALSE;
		} //endif
	} //while
	
	*pargc = m_iArgsOrig;
	return bRet;
}
