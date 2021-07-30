#=====================================================================
PRGNAME		= chkbas

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
		basic.h			\
		basic_tb.h		\
		basic_xl.h		\
		basic_xe.h		\
		carray.h	\
		wintypes.h	


OBJECTS =	chkbas.o	\
		carray.o	\
		cgenfile.o	\
		cfile.o	

#=====================================================================
CC		= gcc
LD		= gcc
LDLIBS		= -lm

#=====================================================================

$(PRGNAME) : $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(PRGNAME)

%.o:%.cpp $(INCLUDES) switches.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

switches.cpp : switches.def switches.pl
	@perl switches.pl $< $@
