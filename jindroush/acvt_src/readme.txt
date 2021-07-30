Acvt v1.07 (c) 2000-2001 Jindrich Kubec <kubecj@asw.cz>
Latest version can be found at http://www.asw.cz/~kubecj

This program is provided 'as is', no warranty will be taken
for any damage caused by it or by any use of it.

The whole package is placed under the GNU Public License, for further
information on redistribution see the included file "GPL.TXT".


Description:
------------
Converts between XFD-SCP-ATR-DCM-DI formats.
ATR - SIO2PC images
XFD - XFormer images
DCM - DiskCommunicator images
SCP - SpartaDos SCOPY images
DI  - XL/ST link / XLDJ disk images

It's replacement for DOS version of Steven Tucker's Imagic.
It's in fact better because:
1) It's free.
2) It comes with source.
3) Therefore it could be ported to another architectures.
4) Contains less bugs. (Hope so.)
5) Can repair some invalid images.
6) Now also supports DI file format.

Thanks go to:
Ernest Schreurs for his DiskCommunicator dissection and paper about DCM format.
Chad Wagner for DCMtoATR utility (DCM driver is still loosely based on his
	code).
Burian brothers for lots of DI files.


Usage:
------
Acvt [options] sourcefile [destfile]

sourcefile is Atari disk image (ATR,XFD,SCP,DCM,DI).

destfile is output disk image. If omitted, output filename is created
 automatically.

-xfd -atr -scp -dcm -di options are output type modes. Default is -atr.

-batchmode: turns on multi file processing mode. Only input files are specified
 on command-line (Unix-like compatible regexp expressions are allowed when
 compiled under DJGPP). Output filenames are created automatically.

-autorepair: automatically determines the best repair method.

-norepair: doesn't ask for repair options and considers input file invalid.

-errstop: in batch mode ends on first error encountered.

-over: overwrites existing files. Otherwise returns an error.

-test: only loads input images and displays their state.

-classic: forces to create 'classic' Atari disk sizes.


History:
--------
Date, Author, Version
5/22/1999, kubecj, 0.10
First version

5/25/1999, kubecj, 0.15
SCP save added

5/29/1999, kubecj, 0.20
DCM save added, decompression corrected

5/30/1999, kubecj, 0.30
Code cleanup

6/1/1999, kubecj, 0.35
Invalid DD ATR repair added

6/2/1999, kubecj, 0.40
Invalid DD XFD repair added
Invalid ATR length repair added
Added cached read for DCM decompression

6/3/1999, kubecj, 0.42
Code cleanup

6/4/1999, kubecj, 0.43
Added checks for DCM decompression
Check for multi-arc

6/5/1999, kubecj, 1.00
First public version

6/10/1999, kubecj, 1.01
Compatibility macros & valid archive :)

6/11/1999, kubecj, 1.02
Repaired repairing ;)

6/13/1999, kubecj, 1.03
Changed switches processing

10/10/1999, kubecj, 1.04
Only changed generation of archive, new version to avoid version checking
problems.

6/6/2000, kubecj, 1.05
Minor bugfix concerning repairing.
Added -classic commandline switch to force to create 'classic' disk sizes.

2/26/2001, kubecj, 1.06
Added DI file format. Its support is just a beta!

3/8/2001, kubecj, 1.07
Handling of double sided diskettes in DI format.


To Do:
------
Maybe check pass count in DCM images?
Better handling of DI files.


Known Bugs:
-----------
DCM record 0x42 is not decompressed, code is present, but commented out and 
 untested. I need some sample first.

DI files are really an educated guess. There are many 'white places' for me.
I need the description of the disk format.

Function for actual conversion is stupid. In fact in order to convert ATR to
 XFD there is created Atr object, loaded in, and same Xfd object is created and 
 all the data are duplicated. With 16MB SpartaDOS Atr images it's simply too 
 much.


Compiling Tools:
----------------
For scripts: Perl.
URL: http://www.perl.com

For PC executables: DJGPP.
Sources were written/tested on GCC/Intel only. There should be only
minor problems to port it to different architectures/environments.
Makefile should need only minor changes.
URL: http://www.delorie.com


