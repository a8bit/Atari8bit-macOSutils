IMID v1.02 (c) 2000 Jindrich Kubec <kubecj@asw.cz>
Latest version can be found at http://www.asw.cz/~kubecj

This program is provided 'as is', no warranty will be taken
for any damage caused by it or by any use of it.

Description:
------------
Identifies disk/file images
IMID is program for identifying Atari binary files. It helps the collectors
to sort their collections.

Note that you also need IMID.DAT file. Download IMIDdb package and unzip
it to the same directory as this program.


Usage:
------
IMID [options] path

path is path where are Atari images stored

-report saves file 'report.rep' into current directory.


History:
--------
Date, Author, Version
8/11/2000, kubecj, 0.01
First version

9/10/2000, kubecj, 1.00
Decided that this will be first release.
Incremented file format version to be 1.01.
Removed IMID.DAT from the binary distribution - now it's distributed in
archive.
Added thorough checks of integrity of IMID.DAT.

9/12/2000, kubecj, 1.01
File format version incremented to 1.02.

9/17/2000, kubecj, 1.02
Alternative EXE checksumming - now it should be immune to block size
changes.


To Do:
------
Maybe ZIP, ARC, TAR, GZIP support.


Known Bugs:
-----------
Exceptions in file open are not checked.
Very slow on ATR files containing DOS2 filesystems.
Doesn't trap CTRL-C -> leaves temporary files in current directory.


Compiling Tools:
----------------
For scripts: Perl.
URL: http://www.perl.com

For PC executables: DJGPP.
Sources were written/tested on GCC/Intel only. There should be only
minor problems to port it to different architectures/environments.
Makefile should need only minor changes.
URL: http://www.delorie.com


