#=====================================================================
PRGNAME		= Di78

debug: 
	@$(MAKE) $(PRGNAME) CFLAGS="-c -g -fno-rtti -Wall -D__A7_SYMBOLS__ -D_DEBUG" LDFLAGS="-g"
	@echo DEBUG: Compiled.

release:
	@$(MAKE) $(PRGNAME) CFLAGS="-c -O2 -fno-rtti -D__A7_SYMBOLS__ -Wall" LDFLAGS=""
	@echo RELEASE: Compiled.

rel_strip:
	@strip $(PRGNAME).exe

rel_inst:
	@copy $(PRGNAME).exe $(ATAROOT)\\bin
	@echo RELEASE: Installed.

clean:
	@rm -f *.o
	@rm -f *.da
	@rm -f *.bb
	@rm -f *.bbg
	@rm -f *.gcov
	@rm -f gmon.out
	@rm -f $(PRGNAME).
	@rm -f $(PRGNAME).exe
	@rm -f switches.cpp
	@rm -rf rel
	@echo DEBUG: Cleaned.

#=====================================================================
INCLUDES =	Makefile		\
		switches.def		\
		pub.def			\
		CDis.h	\
		wintypes.h	\
		cfile.h	\
		cgenfile.h		


OBJECTS =	CDis.o		\
		cfile.o			\
		cgenfile.o		\
		Di78.o	

#=====================================================================
CC		= gcc
LD		= gcc
LDLIBS		= -lm

#=====================================================================

$(PRGNAME) : $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(PRGNAME)

%.o : %.cpp $(INCLUDES) switches.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

switches.cpp : switches.def switches.pl
	@perl switches.pl $< $@
