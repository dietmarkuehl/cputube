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

NAME = smart-pointers
NAME = accumulate-int-array
NAME = unique-strings
NAME = search-integer

TESTS = \
	accumulate-int-array \
	functions \
	get-digits \
	ptr-function-calls  \
	replace \
	search-integer \
	search-short-string \
	sequence-iteration \
	smart-pointers \
	write-characters  \
	write-ints  \

#  ----------------------------------------------------------------------------

COMPILER  = gcc
GXX       = /opt/gcc-current/bin/g++
CLANGXX   = clang++
AR        = ar
ARFLAGS   = rcu
USE_CXX11 = yes
# BSL_CPPFLAGS += -I/usr/local/include/bsl -DHAS_BSL
# BSL_CPPFLAGS += -I/usr/local/include/bdl
# BSL_LDLIBS   += -lbsl

CPPFLAGS = $(BSL_CPPFLAGS)
LDLIBS   = $(BSL_LDLIBS)

ifeq ($(USE_CXX11),yes)
    CPPFLAGS += -DUSE_CXX11
endif

LIBCXX   = /Users/kuehl/src/llvm/libcxx
# LIBSTDCXX = /opt/gcc-current/include/c++/4.9.0

ifeq ($(COMPILER),gcc)
    CXX      = $(GXX)
    DEPFLAGS = -M
    xOPTFLAGS += -g -finline-functions
    xLOPTFLAGS += -pthread
    xLTOFLAGS = -flto
    LOPTFLAGS += -O3
    xLOPTFLAGS += -fno-tree-vectorize

    ifeq ($(USE_CXX11),yes)
        CPPFLAGS += -std=c++11
    else
        CPPFLAGS += -ansi -pedantic
    endif
    CPPLFAGS += -DIS_GCC
    CXXFLAGS += -W -Wall -Wno-unused-local-typedefs
    LDFLAGS  += $(LTOFLAGS)
endif
ifeq ($(COMPILER),clang)
    CXX      = $(CLANGXX)
    DEPFLAGS = -M
    xLTOFLAGS = -flto
    LOPTFLAGS = -O3

    ifeq ($(USE_CXX11),yes)
        CPPFLAGS += -std=c++11
    endif
    CPPFLAGS += -I$(LIBCXX)/include
    xCXXLIB = -std=libc++
    CXXFLAGS += -W -Wall $(CXXLIB) $(OPTFLAGS)
    LDFLAGS  = $(CXXLIB) -L$(LIBCXX)/lib
endif
ifeq ($(COMPILER),icc)
    IS_INTEL=yes
endif
ifeq ($(COMPILER),intel)
    IS_INTEL=yes
endif
ifeq ($(IS_INTEL),yes)
    # detect: __INTEL_COMPILER
    CXX      = icc
    DEPFLAGS = -M
    LOPTFLAGS = -O3
    ifeq ($(USE_CXX11),yes)
        xCPPFLAGS += -Icpu/icc-lib
        CXXFLAGS += -DINTEL -std=c++11
    endif
    CXXFLAGS += $(OPTFLAGS)
endif

OPTEXT   = $(shell echo $(OPTFLAGS) | tr -d '-' | tr ' ' '-')
ARCH     = $(shell uname -s)
OBJ      = $(ARCH)-$(COMPILER)-$(OPTEXT)
CC       = $(CXX)
OPTFLAGS = $(LOPTFLAGS) $(LTOFLAGS)
CPPFLAGS += -DCPUTUBE_ARCH='"$(ARCH)"' \
            -DCPUTUBE_COMPILER='"$(COMPILER)"' \
            -DCPUTUBE_FLAGS='"$(OPTFLAGS)"'

CPPFLAGS += -I/usr/local/include -I.
CXXFILES = \
	cpu/tube/chrono.cpp    \
	cpu/tube/timer.cpp     \
	cpu/tube/context.cpp   \
	cpu/tube/processor.cpp \
	cpu/tube/heap_fragment.cpp     \

LIBFILES  = $(CXXFILES:cpu/tube/%.cpp=$(OBJ)/cputube_%.o)
TESTFILES = $(OBJ)/cputest_$(NAME).o
ifeq ($(NAME),ptr-function-calls)
TESTFILES = $(OBJ)/cputest_$(NAME).o $(OBJ)/cputest_$(NAME)-impl.o
endif

.PHONY: default
default: check

.PHONY: charts
charts: $(OBJ)/cputube_charts
	@mkdir -p charts
	$(OBJ)/cputube_charts $(TESTS);
	cp charts/charts.html charts/index.html
	for f in $(TESTS); \
	do \
		$(MAKE) chart NAME=$$f; \
	done

chart: $(OBJ)/cputube_chart
	@mkdir -p charts
	$(OBJ)/cputube_chart $(NAME) */cputest_$(NAME).result

.PHONY: all
all:
	for f in $(TESTS); \
	do \
	    $(MAKE) check NAME=$$f; \
	done

build-all:
	for f in $(TESTS); \
	do \
	    $(MAKE) NAME=$$f $(OBJ)/cputest_$$f; \
	done


.PHONY: check
check: $(OBJ)/cputest_$(NAME)
	$(OBJ)/cputest_$(NAME) | tee $(OBJ)/cputest_$(NAME).result

.PHONY: $(OBJ)
$(OBJ):
	mkdir $(OBJ)

$(OBJ)/cputest_$(NAME): $(OBJ)/libcputube.a $(TESTFILES)
	$(CXX) -o $@ $(LDFLAGS) $(TESTFILES) -L$(OBJ) -lcputube $(LDLIBS)

$(OBJ)/libcputube.a: $(LIBFILES)
	$(AR) $(ARFLAGS) $@ $(LIBFILES)

$(OBJ)/cputube_%.o: cpu/tube/%.cpp
	@if [ ! -d $(OBJ) ]; then mkdir $(OBJ); fi
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LOPTFLAGS) -c -o $@ $(@:$(OBJ)/cputube_%.o=cpu/tube/%.cpp)

$(OBJ)/cputest_%.o: cpu/test/%.cpp
	@if [ ! -d $(OBJ) ]; then mkdir $(OBJ); fi
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OPTFLAGS) -c -o $@ $(@:$(OBJ)/cputest_%.o=cpu/test/%.cpp)

test: cpuid

.PHONY: clean
clean:
	$(RM) -r $(OBJ)
	$(RM) mkerr olderr *~

distclean:
	$(RM) -r ./$(ARCH)-icc* ./$(ARCH)-gcc* ./$(ARCH)-clang*
	$(RM) -r charts
	$(RM) mkerr olderr *~

.PHONY: depend
depend $(OBJ)/make.depend:
	@if [ ! -d $(OBJ) ]; then mkdir $(OBJ); fi
	$(CXX) $(CPPFLAGS) $(DEPFLAGS) $(CXXFILES) cpu/test/$(NAME).cpp | \
	    scripts/fixdepend  $(OBJ) > $(OBJ)/make.depend

include $(OBJ)/make.depend
