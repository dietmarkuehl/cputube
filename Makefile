#  Makefile -*-makefile-*-
#  ----------------------------------------------------------------------------
#   Copyright (C) 2013 Dietmar Kuehl http://www.dietmar-kuehl.de         
#                                                                        
#   Permission is hereby granted, free of charge, to any person          
#   obtaining a copy of this software and associated documentation       
#   files (the "Software"), to deal in the Software without restriction, 
#   including without limitation the rights to use, copy, modify,        
#   merge, publish, distribute, sublicense, and/or sell copies of        
#   the Software, and to permit persons to whom the Software is          
#   furnished to do so, subject to the following conditions:             
#                                                                        
#   The above copyright notice and this permission notice shall be       
#   included in all copies or substantial portions of the Software.      
#                                                                        
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
#   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
#   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
#   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
#   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
#   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
#   OTHER DEALINGS IN THE SOFTWARE. 
#  ----------------------------------------------------------------------------

NAME = write-ints

TESTS = \
	accumulate-int-array \
	functions \
	search-integer \
	search-short-string \
	sequence-iteration \

#  ----------------------------------------------------------------------------

COMPILER = gcc
GXX      = g++
CLANGXX  = clang++
AR       = ar
ARFLAGS  = rcu

LIBCXX   = /Users/kuehl/src/llvm/libcxx
LIBSTDCXX = /opt/gcc-current/include/c++/4.9.0

ifeq ($(COMPILER),gcc)
    CXX      = $(GXX)
    DEPFLAGS = -M
    xCXXFLAGS += -O3
    xOPTFLAGS = -g -finline-functions
    LTOFLAGS = -flto
    OPTFLAGS = -O3 $(LTOFLAGS)

    CPPFLAGS += -std=c++11
    CPPLFAGS += -DIS_GCC
    CXXFLAGS += -W -Wall $(OPTFLAGS) $(LTOFLAGS)
    LDFLAGS  += $(LTOFLAGS)
    xCXXFLAGS += -fno-tree-vectorize
endif
ifeq ($(COMPILER),clang)
    CXX      = $(CLANGXX)
    DEPFLAGS = -M
    OPTFLAGS = -O4

    CPPFLAGS += -std=c++11 -I$(LIBCXX)/include
    CXXFLAGS += -W -Wall -stdlib=libc++ $(OPTFLAGS)
    LDFLAGS  = -stdlib=libc++ -L$(LIBCXX)/lib
endif
ifeq ($(COMPILER),icc)
    IS_INTEL=yes
endif
ifeq ($(COMPILER),intel)
    IS_INTEL=yes
endif
ifeq ($(IS_INTEL),yes)
    # detect: __INTEL_COMPILER
    CXX      = /usr/bin/icc
    DEPFLAGS = -M
    OPTFLAGS = -O3
    CPPFLAGS += -Icpu/icc-lib
    CXXFLAGS += -std=c++11 $(OPTFLAGS)
endif

CPPFLAGS += -DCPUTUBE_COMPILER='"$(COMPILER)"' -DCPUTUBE_FLAGS='"$(OPTFLAGS)"'

OPTEXT   = $(shell echo $(OPTFLAGS) | tr -d '-' | tr ' ' '-')
ARCH     = $(shell uname -s)
OBJ      = $(ARCH)-$(COMPILER)-$(OPTEXT)
CPPFLAGS += -I.
CXXFILES = \
	cpu/tube/context.cpp   \
	cpu/tube/processor.cpp \
	cpu/tube/timer.cpp     \
	cpu/tube/heap_fragment.cpp     \
	
LIBFILES = $(CXXFILES:cpu/tube/%.cpp=$(OBJ)/cputube_%.o)

.PHONY: default
default: check

.PHONY: all
all:
	for f in $(TESTS); \
	do \
	    $(MAKE) check NAME=$$f; \
	done

.PHONY: check
check: $(OBJ)/cputest_$(NAME)
	$(OBJ)/cputest_$(NAME) | tee $(HOME)/aaa/$(COMPILER).csv

$(OBJ)/cputest_$(NAME): $(OBJ)/libcputube.a $(OBJ)/cputest_$(NAME).o
	$(CXX) -o $@ $(LDFLAGS) $(OBJ)/cputest_$(NAME).o -L$(OBJ) -lcputube

$(OBJ)/libcputube.a: $(LIBFILES)
	$(AR) $(ARFLAGS) $@ $(LIBFILES)

$(OBJ)/cputube_%.o: cpu/tube/%.cpp
	@if [ ! -d $(OBJ) ]; then mkdir $(OBJ); fi
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $(@:$(OBJ)/cputube_%.o=cpu/tube/%.cpp)

$(OBJ)/cputest_%.o: cpu/test/%.cpp
	@if [ ! -d $(OBJ) ]; then mkdir $(OBJ); fi
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $(@:$(OBJ)/cputest_%.o=cpu/test/%.cpp)

test: cpuid

.PHONY: clean
clean:
	$(RM) -r $(OBJ)

.PHONY: depend
depend $(OBJ)/make.depend:
	@if [ ! -d $(OBJ) ]; then mkdir $(OBJ); fi
	$(CXX) $(CPPFLAGS) $(DEPFLAGS) $(CXXFILES) | \
	    scripts/fixdepend  $(OBJ) > $(OBJ)/make.depend

include $(OBJ)/make.depend
