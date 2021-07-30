#=====================================================================
PRGNAME		= adir
#DEL = rm -f
#DELTREE = rm -rf
DEL = del
DELTREE = deltree /y

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
	@$(DEL) *.o
	@$(DEL) $(PRGNAME).
	@$(DEL) $(PRGNAME).exe
	@$(DEL) switches.cpp
	@$(DELTREE) rel
	@echo DEBUG: Cleaned.

#=====================================================================
INCLUDES =				\
		makefile		\
		adsk.h		\
		cdsk.h          \
		adsk_atr.h	\
		adsk_di.h	\
		adsk_xfd.h      \
		adsk_scp.h      \
		adsk_dcm.h      \
		cfs.h     	\
		cfs_dos2.h	\
		cfs_dos3.h	\
		cfs_dosm.h	\
		cfs_doss.h	\
		cfs_kboo.h	\
		cfs_b2b.h	\
		cfs_boot.h	\
		cfs_jonw.h	\
		cfs_robc.h	\
		cfs_howf.h	\
		cfile.h	\
		cgenfile.h	\
		cprefile.h	\
		wintypes.h      \
		cobj.h          \
		autil.h		\
		switches.def		\
		pub.def

OBJECTS =       			\
		adir.o  		\
		cdsk.o        		\
		adsk.o			\
		adsk_atr.o    		\
		adsk_di.o		\
		adsk_xfd.o    		\
		adsk_scp.o    		\
		adsk_dcm.o    		\
		cfs.o         		\
		cfs_dos2.o    		\
		cfs_dos3.o    		\
		cfs_dosm.o    		\
		cfs_doss.o    		\
		cfs_kboo.o    		\
		cfs_b2b.o		\
		cfs_boot.o    		\
		cfs_jonw.o    		\
		cfs_howf.o		\
		cfs_robc.o		\
		cfile.o			\
		cgenfile.o		\
		cprefile.o		\
		cobj.o        		\
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

%.o : %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

adsk_atr.o : adsk_atr.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) -D__CDISK_SAVE__ $(CFLAGS) $< -o $@

adsk_xfd.o : adsk_xfd.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) -D__CDISK_SAVE__ $(CFLAGS) $< -o $@

switches.cpp : switches.def switches.pl
	@perl switches.pl $< $@
