// cpu/tube/processor.cpp                                             -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2013 Dietmar Kuehl http://www.dietmar-kuehl.de         
//                                                                       
//  Permission is hereby granted, free of charge, to any person          
//  obtaining a copy of this software and associated documentation       
//  files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify,        
//  merge, publish, distribute, sublicense, and/or sell copies of        
//  the Software, and to permit persons to whom the Software is          
//  furnished to do so, subject to the following conditions:             
//                                                                       
//  The above copyright notice and this permission notice shall be       
//  included in all copies or substantial portions of the Software.      
//                                                                       
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
//  OTHER DEALINGS IN THE SOFTWARE. 
// ----------------------------------------------------------------------------

#include "processor.hpp"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <ostream>
#include <cctype>
#include <inttypes.h>

using std::placeholders::_1;

// ----------------------------------------------------------------------------

namespace
{
    std::ostream& print(std::ostream& out, uint32_t value) {
        for (int shift(0); shift != 32; shift += 8) {
            unsigned char c(static_cast<unsigned char>(value >> shift));
            if (std::isprint(c)) {
                out << c;
            }
        }
        return out;
    }
}

// ----------------------------------------------------------------------------

#if defined(__x86_64__) || defined(_M_AMD64) || defined (_M_X64)
#  define REG "q %%rbx"
#else
#  define REG "l %%ebx"
#endif

namespace
{
    void cpuid(uint32_t eax, uint32_t (&words)[4])
    {
        __asm__ __volatile__(
            "push" REG "\n"
            "cpuid\n"
            "movl %%ebx ,%[ebx]\n"
            "pop"  REG "\n"
            : "=a"(words[0])
              , [ebx] "=r"(words[1]), "=c"(words[3]), "=d"(words[2])
            : "a"(eax));
    }
}

#undef REG

// ----------------------------------------------------------------------------

static uint32_t print_manufacturer(std::ostream& out)
{
    uint32_t words[4];
    cpuid(0, words);
    std::for_each(words + 1, words + 4, std::bind(&::print, std::ref(out), _1));
    return words[0];
}

// ----------------------------------------------------------------------------

namespace { template <typename T> void use(T const&) {} }

std::ostream& cpu::tube::processor::print(std::ostream& out)
{
    uint32_t ids = print_manufacturer(out);
    use(ids);
#if 0
    out << "\n" << std::hex << std::setfill('0');
    for (uint32_t eax(1); eax != ids; ++eax) {
        uint32_t words[4];
        cpuid(eax, words);
        out << "eax=" << std::setw(8) << words[0] << ' ';
        out << "erx=" << std::setw(8) << words[1] << ' ';
        out << "edx=" << std::setw(8) << words[2] << ' ';
        out << "ecx=" << std::setw(8) << words[3] << ' ';
        out << '\n';
    }
#endif
    out << std::dec << std::setfill(' ');
    return out;
}

// ----------------------------------------------------------------------------

std::ostream& cpu::tube::operator<< (std::ostream&               out,
                                     cpu::tube::processor const& processor)
{
    return processor.print(out);
}
