ChkBas v1.10 (c) 1999-2001 Jindrich Kubec <kubecj@asw.cz>
Latest version can be found at http://www.asw.cz/~kubecj

This program is provided 'as is', no warranty will be taken
for any damage caused by it or by any use of it.

The whole package is placed under the GNU Public License, for further
information on redistribution see the included file "GPL.TXT".


Description:
------------
Atari Basic/Turbo Basic/Basic XL/Basic XE decompiler.



Usage:
------
ChkBas [options] sourcefile [lstfile]

sourcefile is Atari basic file.

lstfile is output text file

-atari    - exports source only in Atari format
-short    - exports source only in PC format
-verbose  - prints verbose information
-tbs      - decodes Turbo Basic file
-bxl      - decodes Basic XL file
-bxe      - decodes Basic XE file

The export is by default more verbose and in PC format.


History:
--------
Date, Author, Version
4/5/1999, kubecj, 0.90
First version

4/20/1999, kubecj, 1.00
First public version

4/20/1999, kubecj, 1.01
BCD error bugfix

7/7/1999, kubecj, 1.02
Atari output file bugfix (it did break lines on 0x0A char)

9/5/1999, kubecj, 1.03
'Hacked' variable table bugfix (overflow when filled with chars under 0x80)

10/10/1999, kubecj, 1.04
Changed switches processing.

7/5/2001, kubecj, 1.05
Added Basic XL.

7/6/2001, kubecj, 1.06
Added Basic XE.

7/7/2001, kubecj, 1.10
Added Basic XE extended mode. Code cleanup.


To Do:
------
Add other basic dialects?


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


