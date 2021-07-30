AExt v1.02 (c) 1998-2002 Jindrich Kubec <kubecj@asw.cz>
Latest version can be found at http://www.asw.cz/~kubecj

This program is provided 'as is', no warranty will be taken
for any damage caused by it or by any use of it.

The whole package is placed under the GNU Public License, for further
information on redistribution see the included file "GPL.TXT".


Description:
------------
Atari file extractor.
This is just simple utility for extracting binary block in binary/atari binary
block formats.


Usage:
------
AExt [options] infile [outfile]

If outfile is not specified, prg.axe is assumed.

-s, -e, -n specify start, end or length parameters of block in hexadecimal
	numbers.

-oa creates atari binary block (start, end, data).

-base is used with -oa to create based binary block. (The number specifies 
	start address of block).


History:
--------
Date, Author, Version
8/19/1999, kubecj, 0.10
First version.

10/10/1999, kubecj, 0.11
New archive processing, version incremented to avoid version problems.

10/10/1999, kubecj, 1.00
First release.

12/10/2001, kubecj, 1.01
Large block processing.
-e 0 means end ptr now.

1/23/2002, kubecj, 1.02
Added useful single density extracting parameters. Now include 's' as first
character in a number, e.g. -s s06 means start from sixth sector. By definition
works only on ATR files with 0x80 long sectors.


To Do:
------
Nothing.


Known Bugs:
-----------
None.


Compiling Tools:
----------------
For scripts: Perl.
URL: http://www.perl.com

For PC executables: DJGPP.
Sources were written/tested on GCC/Intel only. There should be only
minor problems to port it to different architectures/environments.
Makefile should need only minor changes.
URL: http://www.delorie.com


