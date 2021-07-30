DI78 v0.94 (c) 2002 Jindrich Kubec <jindroush@seznam.cz>
Latest version can be found at http://jindroush.atari.org

This program is provided 'as is', no warranty will be taken
for any damage caused by it or by any use of it.

The whole package is placed under the GNU Public License, for further
information on redistribution see the included file "GPL.TXT".


Description:
------------
Atari7800 rom disassembler.
Disassembler is only very simple - single pass, no data recognition.


Usage:
------
DI78 [options] sourcefile

sourcefile is Atari7800 *.a78 file.

dumps creates symbol table for in format specific to selected assembler

xa creates disassembly output for Andre Fachat's XA

xasm creates disassembly output for Piotr Fusik's X-ASM

dasm creates disassembly output for Matt Dillon's DASM

justmain outputs only main bank (if possible)

db prints only dbs

undoc_none no undocumented instructions decoded

undoc_full prints fully decoded undocumented instructions

full_cart creates dbs for a78 header (DASM mode only)


History:
--------
Date, Author, Version
5/8/2002, kubecj, 0.90
Initial release

5/18/2002, kubecj, 0.91
Preliminary support for DASM
Support for undocumented instructions
Few new banking options (see -justmain)

5/19/2002, kubecj, 0.92
Minor disassembler changes.
DASM .wx, .wy fix.

5/24/2002, kubecj, 0.93
Undocumented opcodes mnemonics for DASM.
Added -full_cart switch for DASM processing.

5/25/2002, kubecj, 0.94
Few DASM fixes.


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

For PC windows executables: Cygwin.
Sources were written/tested on GCC/Intel only. There should be only
minor problems to port it to different architectures/environments.
Makefile should need only minor changes.
You may need cygwin1.dll, see url below.
URL: http://sources.redhat.com


