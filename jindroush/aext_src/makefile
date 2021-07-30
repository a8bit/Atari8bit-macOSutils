#=====================================================================
PRGNAME		= aext

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
INCLUDES =	makefile	\
		wintypes.h \
		switches.def \
		pub.def	


OBJECTS =	aext.o
		
#=====================================================================
CC		= gcc
LD		= gcc
LDLIBS		= -lm

#=====================================================================

$(PRGNAME) : $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(PRGNAME)

%.o:%.cpp $(INCLUDES) switches.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

switches.h : switches.def switches.pl
	@perl switches.pl $< $@

