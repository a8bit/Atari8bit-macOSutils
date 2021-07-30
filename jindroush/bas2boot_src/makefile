#=====================================================================
PRGNAME		= bas2boot

release:
	@$(MAKE) $(PRGNAME) CFLAGS="-c -O2 -Wall -D__CDISK_SAVE__ -D__CDISK_NOLOAD__" LDFLAGS=""
	@echo RELEASE: Compiled.

rel_strip:
	@strip $(PRGNAME).exe

rel_inst:
	@copy $(PRGNAME).exe $(ATAROOT)\\bin
	@copy bas2boot.boo $(ATAROOT)\\bin
	@echo RELEASE: Installed.

debug:
	@$(MAKE) $(PRGNAME) CFLAGS="-c -g -Wall -D_DEBUG -D__CDISK_SAVE__ -D__CDISK_NOLOAD__" LDFLAGS="-g"
	@echo DEBUG: Compiled.

clean:
	@rm *.o
	@rm $(PRGNAME).
	@rm $(PRGNAME).exe
	@rm switches.cpp
	@deltree /y rel
	@echo DEBUG: Cleaned.

#=====================================================================
INCLUDES =	makefile	\
		switches.def	\
		pub.def		\
		adsk.h		\
		cdsk.h          \
		adsk_atr.h	\
		adsk_xfd.h      \
		adsk_scp.h      \
		adsk_dcm.h      \
		adsk_di.h	\
		cfile.h	\
		cgenfile.h	\
		cprefile.h	\
		wintypes.h      \
		cobj.h          \
		autil.h		


OBJECTS =	bas2boot.o	\
		adsk_atr.o    \
		adsk_xfd.o    \
		adsk_scp.o    \
		adsk_dcm.o	\
		adsk_di.o	\
		adsk.o	\
		cdsk.o        \
		cobj.o        \
		cfile.o	\
		cgenfile.o	\
		cprefile.o	\
		autil.o

#=====================================================================
CC		= g++
LD		= g++
LDLIBS		= -lm

#=====================================================================

$(PRGNAME) : $(OBJECTS) bas2boot.boo
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(PRGNAME)

%.o : %.cpp $(INCLUDES) switches.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

switches.cpp : switches.def switches.pl
	@perl switches.pl $< $@

bas2boot.boo : bas2boot.asm
	xa -M $< -o $@
