#=====================================================================
PRGNAME		= acvt

release:
	@$(MAKE) $(PRGNAME) CFLAGS="-c -O2 -Wall -D__CDISK_SAVE__" LDFLAGS=""
	@echo RELEASE: Compiled.

rel_strip:
	@strip $(PRGNAME).exe

rel_inst:
	@copy $(PRGNAME).exe $(ATAROOT)\\bin
	@echo RELEASE: Installed.

debug:
	@$(MAKE) $(PRGNAME) CFLAGS="-c -g -Wall -D_DEBUG -D__CDISK_SAVE__" LDFLAGS="-g"
	@echo DEBUG: Compiled.

clean:
	@del *.o
	@del $(PRGNAME).
	@del $(PRGNAME).exe
	@del switches.cpp
	@deltree /y rel
	@echo DEBUG: Cleaned.

#=====================================================================
INCLUDES =			\
		makefile	\
		switches.def	\
		pub.def		\
		adsk.h		\
		cdsk.h          \
		adsk_atr.h	\
		adsk_xfd.h      \
		adsk_scp.h      \
		adsk_dcm.h      \
		adsk_di.h	\
		wintypes.h      \
		cobj.h          \
		autil.h		\
		cfile.h	\
		cgenfile.h	\
		cprefile.h	



OBJECTS =        			\
		acvt.o        \
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

%.o : %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

switches.cpp : switches.def switches.pl
	@perl switches.pl $< $@
