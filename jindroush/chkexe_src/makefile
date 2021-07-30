#=====================================================================
PRGNAME		= chkexe

release:
	@$(MAKE) $(PRGNAME) CFLAGS="-c -O2 -Wall" LDFLAGS=""
	@echo RELEASE: Compiled.

rel_strip:
	@strip $(PRGNAME).exe

rel_inst:
	@copy $(PRGNAME).exe $(ATAROOT)\\bin
	@echo RELEASE: Installed.

debug: 
	@$(MAKE) $(PRGNAME) CFLAGS="-c -g -Wall -D_DEBUG" LDFLAGS="-g"
	@echo DEBUG: Compiled.

clean:
	@del *.o
	@del $(PRGNAME).
	@del $(PRGNAME).exe
	@del switches.cpp
	@deltree /y rel
	@echo DEBUG: Cleaned.

#=====================================================================
INCLUDES =	makefile		\
		switches.def		\
		pub.def			\
		at_dis.h	\
		wintypes.h	\
		cfile.h	\
		cgenfile.h		


OBJECTS =	chkexe.o		\
		cfile.o			\
		cgenfile.o		\
		at_dis.o	

#=====================================================================
CC		= gcc
LD		= gcc
LDLIBS		= -lm

#=====================================================================

$(PRGNAME) : $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(PRGNAME)

%.o : %.cpp $(INCLUDES) switches.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

switches.cpp : switches.def switches.pl
	@perl switches.pl $< $@
