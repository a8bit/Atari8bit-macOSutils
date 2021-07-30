Adir v0.67 (c) 1999-2001 Jindrich Kubec <kubecj@asw.cz>
Latest version can be found at http://www.asw.cz/~kubecj

This program is provided 'as is', no warranty will be taken
for any damage caused by it or by any use of it.

The whole package is placed under the GNU Public License, for further
information on redistribution see the included file "GPL.TXT".


Description:
------------
Extracts files from Atari disk images.
Originally based on ATR2UNIX from Preston Crow, now rewritten totally using
virtual CFs and CDisk classes.

Currently supported are:
Atari DOS 2.0/2.5 and compatibles
Atari DOS 3.0
MyDOS
Sparta DOS and compatibles (BeWe)
K-Boot images
HowFen DOS
JonW multiboot
RobC multiboot
Bas2Boot images
boot images (extracts boot sector only)

Adir tries to guess the best filesystem. Could be of course overriden by 
switch.


Usage:
------
Adir [options] sourcefile [mask]

sourcefile is Atari disk image.

-? prints switches


History:
--------
Date, Author, Version
4/5/1999, kubecj, 0.10
First version

5/11/1999, kubecj, 0.20
Added DOS3 filesystem

5/17/1999, kubecj, 0.30
Added SpartaDos filesystem

5/22/1999, kubecj, 0.40
Added KBoot filesystem

5/23/1999, kubecj, 0.50
Added SCP & DCM disk drivers

6/3/1999, kubecj, 0.55
Disk drivers synchronized

6/11/1999, kubecj, 0.60
Added HowFen Dos filesystem

6/12/1999, kubecj, 0.61
Added Boot filesystem (virtual)

10/10/1999, kubecj, 0.62
Changed switches processing.

3/30/2000, kubecj, 0.63
Added autorepair.
Added Bas2Boot filesystem.

2/26/2001, kubecj, 0.64
Added handling of DI files.

3/1/2001, kubecj, 0.65
Compatibility macros for big-endian systems.

3/8/2001, kubecj, 0.66
Double side support for DI files.

6/12/2001, kubecj, 0.67
Added RobC fs.


To Do:
------
Add other file systems. (DOS4)
Add volume information (useful for Sparta only?)
Add write capability (after a long time).


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


