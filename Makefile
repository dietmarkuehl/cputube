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

NAME = functions

#  ----------------------------------------------------------------------------
COMPILER = gcc
GXX      = g++
CLANGXX  = clang++
AR       = ar
ARFLAGS  = rcu

ifeq ($(COMPILER),gcc)
    CXX      = $(GXX)
    CPPFLAGS += -std=c++11
    CXXFLAGS += -W -Wall -O3
endif
ifeq ($(COMPILER),clang)
    LIBCXX   = /Users/kuehl/src/llvm/libcxx
    CXX      = $(CLANGXX)
    CPPFLAGS += -std=c++11 -I$(LIBCXX)/include
    CXXFLAGS += -W -Wall -stdlib=libc++ -O3
    LDFLAGS  = -stdlib=libc++ -L$(LIBCXX)/lib
endif

ARCH     = $(shell uname -s)
OBJ      = $(ARCH)-$(COMPILER)
CPPFLAGS += -I.
CXXFILES = \
	cpu/tube/timer.cpp \
	cpu/tube/processor.cpp \

LIBFILES = $(CXXFILES:cpu/tube/%.cpp=$(OBJ)/cputube_%.o)

.PHONY: default
default: check

.PHONY: check
check: $(OBJ)/cputest_$(NAME)
	$(OBJ)/cputest_$(NAME)

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
