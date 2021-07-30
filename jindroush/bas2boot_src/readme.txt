Bas2Boot v1.02 (c) 1998-2001 Jindrich Kubec <kubecj@asw.cz>
Latest version can be found at http://www.asw.cz/~kubecj

This program is provided 'as is', no warranty will be taken
for any damage caused by it or by any use of it.

The whole package is placed under the GNU Public License, for further
information on redistribution see the included file "GPL.TXT".


Description:
------------
Converts Basic files to disk images.



Usage:
------
Bas2Boot [options] basicfile [diskfile]

basicfile is source basic filename.
 
diskfile is output filename. If not specified it's guessed from input filename.

-atr -xfd -scp -dcm -di specify output file format. Default is ATR.

-long forces to use 'classic' disk sizes (SD,DD,ED)

-message text -sets message displayed while loading program


History:
--------
Date, Author, Version
4/5/1999, kubecj, 0.92
First version

6/14/1999, kubecj, 1.00
First public version

10/10/1999, kubecj, 1.01
Only version number increment because of different archive processing.

2/26/2001, kubecj, 1.02
Added DI file format.


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

For Atari assembler sources: XA65 assembler.
URL: http://stockholm.ptloma.edu/cbm/xa/.


