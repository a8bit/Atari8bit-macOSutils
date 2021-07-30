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
	printf( "-help     -this help\n" );
	printf( "-d        -disassembles\n" );
	printf( "-silent   -silent operation\n" );
	printf( "-gaps     -length of gap fill (hexadecimal)\n" );
	printf( "-split    -split into blocks\n" );
	return FALSE;
}


BOOL SWFN_NUMH( int* piNum )
{
	char* szOrigSw = m_szSwitch;
	
	if ( !SWITCHES_GetValue() )
	{
		printf( "Missing value for switch '%s'\n", szOrigSw );
		return FALSE;
	}
	
	sscanf( m_szSwitch, "%X", piNum );
	
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
			if (!strcmp( m_szSwitch, "d" ) || !strcmp( m_szSwitch, "dis" ))
			{
				g_bDisassemble = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "silent" ) || !strcmp( m_szSwitch, "s" ))
			{
				g_bSilent = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "gaps" ))
			{
				int iTemp;
				bRet = SWFN_NUMH( &iTemp );
				g_wGapFill = iTemp;
				continue;
			}
			if (!strcmp( m_szSwitch, "split" ))
			{
				g_bSplit = TRUE;
				continue;
			}
			
			printf( "Invalid switch: %s\n", *( m_ppcArgvCurr - 1 ) );
			return FALSE;
		} //endif
	} //while
	
	*pargc = m_iArgsOrig;
	return bRet;
}
