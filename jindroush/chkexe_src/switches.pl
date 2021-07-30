$infile = "switches.def";
$outfile = "switches.cpp";

if ( @ARGV )
{
	$infile = shift @ARGV;
}

if ( @ARGV )
{
	$outfile = shift @ARGV;
}

############################################################################
## Script for generating switches.cpp
##
############################################################################

open INFILE, $infile or die;
@inf = <INFILE>;
close INFILE;

unshift @inf, "=";
unshift @inf, "=bRet = SWFN_HELP( USAGE );";
unshift @inf, "this help";
unshift @inf, "help, ?";

push @out, <<LOOP_START;

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

LOOP_START

for(;;)
{
	$line = shift @inf;

	last unless defined( $line );

	chomp $line;

	$line =~ s/\s+$//;
	$line =~ s/^\s+//;

	next unless $line;

	unshift @inf, $line;

	&switch();

};

push @out, <<LOOP_END;

printf( "Invalid switch: %s\\n", *( m_ppcArgvCurr - 1 ) );
return FALSE;
} //endif
} //while

*pargc = m_iArgsOrig;
return bRet;
}
LOOP_END

if ( $CODE_INCLUDE{ 'SWFN_GETPATH' } )
{
	unshift @out, <<SWFN_GETPATH_CODE;

BOOL SWFN_GETPATH( char* szPath )
{
char* szOrigSw = m_szSwitch;

if ( !SWITCHES_GetValue() )
{
printf( "Missing value for switch '%s'\\n", szOrigSw );
return FALSE;
}

strcpy( szPath, m_szSwitch );
return TRUE;
}

SWFN_GETPATH_CODE
}

if ( $CODE_INCLUDE{ 'SWFN_NUMH' } )
{
	unshift @out, <<SWFN_NUMH_CODE;

BOOL SWFN_NUMH( int* piNum )
{
char* szOrigSw = m_szSwitch;

if ( !SWITCHES_GetValue() )
{
printf( "Missing value for switch '%s'\\n", szOrigSw );
return FALSE;
}

sscanf( m_szSwitch, "%X", piNum );

return TRUE;
}
SWFN_NUMH_CODE
}

unshift @out, <<SWFN_HELP_CODE2;
return FALSE;
}

SWFN_HELP_CODE2

$longest = 0;

foreach $line ( @help )
{
	my ( $switch, $desc, $par ) = split( /\|/, $line, 3 );

	my $len = length( $switch ) + length( $par );

	$longest = $len if ( $len > $longest );
}

$longest += 2 + 9;
foreach $line ( @help )
{
	my ( $switch, $desc, $par ) = split( /\|/, $line, 3 );

	my $len = length( $switch ) + length( $par );

	my $l = "printf( \"-$switch";

	if ( $par )
	{
		$l .= " $par";
	}

	$l .= ' ' x ( $longest - length( $l ) );

	$l .= "  -$desc\\n\" );\n";

	push @helpf, $l;

}

unshift @out, @helpf;

unshift @out, <<SWFN_HELP_CODE1;

BOOL SWFN_HELP( char* szUsage )
{
if ( szUsage )
	printf( "%s\\n", szUsage );
SWFN_HELP_CODE1


unshift @out, <<SWITCHES_GLOBALS;

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

SWITCHES_GLOBALS

open OUTFILE, ">$outfile";

print OUTFILE "//THIS IS A GENERATED FILE. DO NOT EDIT!!!\n";
print OUTFILE "//EDIT $infile INSTEAD!\n\n";

$indent = 0;

$bigline = join( '', @out );
@out = split( /\n/, $bigline );

foreach $line ( @out )
{
	$indent-- if ( $line =~ /}/ );

	print OUTFILE "\t" x $indent, $line, "\n";

	$indent++ if ( $line =~ /{/ );

}

close OUTFILE;

sub switch()
{
	my $switch_desc = "";

	my $switches_names = shift @inf;
	my $switches_desc = shift @inf;

	chomp $switches_desc;

	my @switches = split( /,/, $switches_names );

	foreach $switch ( @switches )
	{
		$switch =~ s/\s+$//;
		$switch =~ s/^\s+//;

		if ( defined ( $SWITCHES{ $switch } ) )
		{
			die "Duplicate switch $switch";
		}

		$SWITCHES{ $switch } = 1;

	}

	$switches_count = @switches;

	$switch_desc = $switches[ 0 ] . "|" . $switches_desc . "|";

	push @out, "if (";

	my @asc;

	foreach $switch (@switches )
	{
		push @asc, "!strcmp( m_szSwitch, \"$switch\" )";
	}

	push @out, join( " || ", @asc );

	push @out, ")";
	
	push @out, "\n{\n";

	my @switch_pars = ();

	for(;;)
	{
		my $par_desc = shift @inf;

		my ( $par, $code ) = split( /=/, $par_desc, 2 );

		$par =~ s/\s+$//;
		$par =~ s/^\s+//;

		$code =~ s/\s+$//;
		$code =~ s/^\s+//;

		last if ( !$par && !$code );

		if ( $code =~ /(SWFN_\w+)/ )
		{
			$CODE_INCLUDE{ $1 } = 1;
		}

		if ( $par )
		{
			push @switch_pars, $par;
		}

		push @out, $code . "\n";
	}

	push @out, "continue;\n";

	$switch_desc .= join( ' ', @switch_pars );

	push @help, $switch_desc;

	push @out, "}\n";

}
