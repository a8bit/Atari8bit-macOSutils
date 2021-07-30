ChkExe v2.62 (c) 1998-1999 Jindrich Kubec <kubecj@asw.cz>
Latest version can be found at http://www.asw.cz/~kubecj

This program is provided 'as is', no warranty will be taken
for any damage caused by it or by any use of it.

The whole package is placed under the GNU Public License, for further
information on redistribution see the included file "GPL.TXT".


Description:
------------
Atari executable checker/decompiler.
Based on Preston Crow's binload utility. Better command line switches,
better disassembler and gap filing.

Disassembler is only very simple - single pass, no data recognition.


Usage:
------
ChkExe [options] sourcefile [destfile]

sourcefile is Atari DOS exe file.

destfile is optional output file with changed blocks.

gaps is for changing the size of maximum 'gap' fill. It's the destination
  of two blocks under which blocks will be 'glued' together by zeroes.

silent outputs only OK/errors

split creates also blkXXXX.blk files with only one block inside

d creates also disassembly of each block


History:
--------
Date, Author, Version
6/2/1995, crow, 1.00
Initial public release

6/4/1995, crow, 2.00
Create fixed version of the file

6/7/1995, crow, 2.10
Use separate functions
Merge overlapping and adjacent blocks

6/9/1995, cmwagner, 2.20
Ported to MS-DOS machines, should compile and work under
MS-DOS,compile in COMPACT model.

11/11/1995, cmwagner, 2.30
Added d switch, which allow disassembly of blocks to stdout
disassemble_block() function added, outins() function added, instable[] added

11/16/1995, cmwagner, 2.40
Fixed bogus operands on output when operands extend beyond
end of block, just puts out a bogus instruction symbol now (???).

9/24/1998, kubecj, 2.50
Modularized, added symbol resolving in disassembler
Added -silent switch for batch processing
Added gap filling
Rewritten in CPP under DJGPP only, no portability

7/7/1999, kubecj, 2.60
Added file blocks splitting

8/20/1999, kubecj, 2.61
Fixed signedness bug concerning branches :(

10/10/1999, kubecj, 2.62
Changed switches processing.


To Do:
------
Maybe better disassembler.


Known Bugs:
-----------
None.


Compiling Tools:
----------------
For scripts: Perl.
URL: http://www.perl.com

For PC executables: DJGPP.
Sources were written/tested on GCC/Intel only. There should be only
slight problems to port it to different architectures/environments.
Makefile should need only slight changes.
URL: http://www.delorie.com


