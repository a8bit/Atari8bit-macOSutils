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
	printf( "-help        -this help\n" );
	printf( "-verbose     -verbose info\n" );
	printf( "-atari       -Atari output file (only lst is created)\n" );
	printf( "-short       -PC - lst only output (full output otherwise)\n" );
	printf( "-noinverse   -no inverse characters in DATA or REMs\n" );
	printf( "-bxl         -Basic XL\n" );
	printf( "-bxe         -Basic XE\n" );
	printf( "-tbs         -Turbo Basic\n" );
	return FALSE;
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
			if (!strcmp( m_szSwitch, "verbose" ) || !strcmp( m_szSwitch, "v" ))
			{
				g_bVerbose = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "atari" ))
			{
				g_bAtariOut = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "short" ))
			{
				g_bExtOut = FALSE;
				continue;
			}
			if (!strcmp( m_szSwitch, "noinverse" ))
			{
				g_bNoInverse = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "bxl" ))
			{
				g_Type = BASIC_XL;
				continue;
			}
			if (!strcmp( m_szSwitch, "bxe" ))
			{
				g_Type = BASIC_XE;
				continue;
			}
			if (!strcmp( m_szSwitch, "tbs" ))
			{
				g_Type = BASIC_TB;
				continue;
			}
			
			printf( "Invalid switch: %s\n", *( m_ppcArgvCurr - 1 ) );
			return FALSE;
		} //endif
	} //while
	
	*pargc = m_iArgsOrig;
	return bRet;
}
