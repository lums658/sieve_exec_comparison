#
# This file is part of the course materials for CSE P 524 at the University of Washington,
# Winter 2022
#
# Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License
# https://creativecommons.org/licenses/by-nc-sa/4.0/
#
# Author: Andrew Lumsdaine
# 

LEVEL		:= 20
LANG		:= -std=c++$(LEVEL)
PICKY		:= -Wall  # -fconcepts-diagnostics-depth=2

CXX		:= c++
#CXX		:= g++-11

SHELL           := /bin/bash
ARCH            := $(shell arch)
UNAME_S         := $(shell uname -s)
CXXVERSION 	:= $(shell ${CXX} --version | head -1)

ifeq 'g++' '$(findstring g++, $(CXXVERSION))'
  CXXFAM := gnu
endif

ifeq 'clang' '$(findstring clang, $(CXXVERSION))'
  CXXFAM := clang
endif

P2300		:= ./wg21_p2300_std_execution
INCLUDES	+= -I$(P2300)/include -I$(P2300)/test -Iconcurrencpp/include
VPATH		:= concurrencpp

# Set up arch specific optimization flags
ifeq ($(ARCH),arm64)
ifeq ($(findstring g++,$(CXX)),g++)
  NATIVE	:= -march=native
else
  NATIVE	:= -mcpu=apple-m1
endif
else
  NATIVE	:= -march=native
endif

# Set up TBB for Linux
ifeq ($(UNAME_S),Linux)
PTHREAD         := -pthread
LIB_TBB		:= -ltbb
# for oneapi-tbb
TBB_H		:= /opt/intel/oneapi/tbb/2021.5.1/include/tbb/tbb.h
ifeq ($(shell test -e $(TBB_H) && echo -n yes),yes)
TBBROOT		:= /opt/intel/oneapi/tbb/2021.5.1
INCLUDES	+= -I$(TBBROOT)/include
LIBS		+= -L$(TBBROOT)/lib/intel64/gcc4.8
LIBS		+= -Wl,-rpath,$(TBBROOT)/lib/intel64/gcc4.8
endif
LIBS		+= $(LIB_TBB)
endif

# Set up TBB for MacOS
ifeq ($(UNAME_S),Darwin)
ifeq ($(shell test -d /opt/homebrew && echo -n yes),yes)
HOMEBREW	:= /opt/homebrew
TBBROOT		:= $(HOMEBREW)
else ifeq ($(shell test -d /usr/local/cellar && echo -n yes),yes)
HOMEBREW	:= /usr/local/Cellar
TBBROOT		:= $(HOMEBREW)/tbb/2021.5.0
else
HOMEBREW	:= /set/HOMEBREW/please
foo		:= $(shell touch 3)
endif
INCLUDES	+= -I$(TBBROOT)/include
LIBS		+= -L$(TBBROOT)/lib
LIBS		+= -ltbb
endif

ifeq ($(UNAME_S),Darwin)
  XDEFS		 += -mmacosx-version-min=11.6
  ifeq ($(CXXFAM),clang)
    XDEFS        += -Xpreprocessor

    ifeq ($(shell test -d /opt/homebrew && echo -n yes),yes)
      HOMEBREW	:= /opt/homebrew
      OMP	:= $(HOMEBREW)
    else ifeq ($(shell test -d /usr/local/opt && echo -n yes),yes)
      HOMEBREW	:= /usr/local/opt
      OMP	:= $(HOMEBREW)/libomp
    else
      HOMEBREW	:= /set/HOMEBREW/please
    endif
    INCLUDES	+= -I$(OMP)/include
    LIBS	+= -L$(OMP)/lib
    LIBS	+= -lomp # -lpthread -latomic # pthread not needed 
  endif


  ifeq ($(CXXFAM),gnu)
  #  LIBS		+= -L/usr/local/lib -lomp

  endif
endif

XDEFS		+= -fopenmp

ifeq ($(CXXFAM),clang)
XDEFS		+= -fcoroutines-ts
else ifeq ($(CXXFAM),gnu)
XDEFS		+= -fcoroutines
endif



# Uncomment the appropriate set of optimization flags
# (or use OPTS="-O3" -- e.g. -- on the command line with make)
# OPTS		= -g
# OPTS		= -O3 -DNDEBUG
# OPTS		= -O3 -march=native -DNDEBUG
# OPTS		= -Ofast -DNDEBUG
OPTS		= -Ofast $(NATIVE) -DNDEBUG

# LIBS		+= -latomic
LIBS		+= -Lconcurrencpp/build -lconcurrencpp

REPORTING   	= -Rpass=.*

CXXFLAGS	= $(OPTS) $(LANG) $(PICKY) $(INCLUDES) $(DEFS) $(XDEFS) $(PTHREAD)

SOURCES		= sieve_async_fun.cpp sieve_cc_fun.cpp sieve_direct_fun.cpp sieve_p2300_fun.cpp sieve_tbb_fun.cpp

HEADERS		= $(SOURCES:.cpp=.hpp)
OBJECTS		= $(SOURCES:.cpp=.o)
TARGETS		= $(SOURCES:.cpp=.exe)
PCHS		= $(HEADERS:=.gch)

.PHONY		: defreport optreport clean depend all debug

all		: $(TARGETS)

%.exe        	: %.o
		  $(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

.PRECIOUS	: %.o

%.o 		: %.cpp
		  $(CXX) -c $(CXXFLAGS) $< -o $@

%.s 		: %.cpp
		  $(CXX) -S -fverbose-asm $(CXXFLAGS) $<
defreport	:
		  $(CXX) -dM -E -x c++ /dev/null

optreport	:
		  echo 'int;' | $(CXX) -xc++ $(CXXFLAGS) - -o /dev/null -\#\#\#

clean		:
		  /bin/rm -f $(TARGETS) $(OBJECTS) $(PCHS) a.out *~

depend: $(ALL_SRC)
	@ $(CXX) -MM $(LANG) $(INCLUDES) $(SOURCES) > makedep
	@ echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	@ echo '$$r makedep' >>eddep
	@ echo 'w' >>eddep
	@ cp Makefile Makefile.bak
	@ ed - Makefile < eddep
	@ /bin/rm eddep makedep
	@ echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	@ echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	@ echo '# see make depend above' >> Makefile


# The following 4 (yes 4) comment lines must be in all the subdirectory makefiles
#-----------------------------------------------------------------
# DO NOT DELETE THIS LINE -- make depend uses it
# DEPENDENCIES MUST END AT END OF FILE
sieve_async_fun.o: sieve_async_fun.cpp sieve_fun.hpp sieve.hpp
sieve_cc_fun.o: sieve_cc_fun.cpp sieve_fun.hpp sieve.hpp
sieve_direct_fun.o: sieve_direct_fun.cpp sieve_fun.hpp sieve.hpp
sieve_p2300_fun.o: sieve_p2300_fun.cpp sieve_fun.hpp sieve.hpp
sieve_tbb_fun.o: sieve_tbb_fun.cpp sieve_fun.hpp sieve.hpp
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
