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
	printf( "-v            -verbose output\n" );
	printf( "-atr          -output is ATR\n" );
	printf( "-di           -output is DI\n" );
	printf( "-xfd          -output is XFD\n" );
	printf( "-scp          -output is SCP\n" );
	printf( "-dcm          -output is DCM\n" );
	printf( "-batch        -batch mode\n" );
	printf( "-errstop      -in batchmode stop on first error\n" );
	printf( "-over         -turns on file overwriting\n" );
	printf( "-autorepair   -auto repair mode\n" );
	printf( "-norepair     -don't repair\n" );
	printf( "-test         -only test input files\n" );
	printf( "-classic      -force to create 'classic' disk formats\n" );
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
			if (!strcmp( m_szSwitch, "v" ))
			{
				g_bVerbose = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "atr" ) || !strcmp( m_szSwitch, "a" ))
			{
				g_dtypeOut = DISK_ATR;
				continue;
			}
			if (!strcmp( m_szSwitch, "di" ))
			{
				g_dtypeOut = DISK_DI;
				continue;
			}
			if (!strcmp( m_szSwitch, "xfd" ) || !strcmp( m_szSwitch, "x" ))
			{
				g_dtypeOut = DISK_XFD;
				continue;
			}
			if (!strcmp( m_szSwitch, "scp" ) || !strcmp( m_szSwitch, "s" ))
			{
				g_dtypeOut = DISK_SCP;
				continue;
			}
			if (!strcmp( m_szSwitch, "dcm" ) || !strcmp( m_szSwitch, "d" ))
			{
				g_dtypeOut = DISK_DCM;
				continue;
			}
			if (!strcmp( m_szSwitch, "batch" ) || !strcmp( m_szSwitch, "batchmode" ) || !strcmp( m_szSwitch, "b" ))
			{
				g_bBatchMode = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "errstop" ))
			{
				g_bFirstErrStop = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "over" ) || !strcmp( m_szSwitch, "overwrite" ))
			{
				g_bOverWrite = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "autorepair" ))
			{
				g_bRepairAuto = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "norepair" ))
			{
				g_bRepair = FALSE;
				continue;
			}
			if (!strcmp( m_szSwitch, "test" ))
			{
				g_bTestOnly = TRUE;
				continue;
			}
			if (!strcmp( m_szSwitch, "classic" ))
			{
				g_bForceClassic = TRUE;
				continue;
			}
			
			printf( "Invalid switch: %s\n", *( m_ppcArgvCurr - 1 ) );
			return FALSE;
		} //endif
	} //while
	
	*pargc = m_iArgsOrig;
	return bRet;
}
