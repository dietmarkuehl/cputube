// cpu/tube/context.cpp                                               -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2014 Dietmar Kuehl http://www.dietmar-kuehl.de         
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

#include "cpu/tube/context.hpp"
#include "cpu/tube/processor.hpp"
#include <iostream>
#include <iomanip>
#include <iterator>
#include <algorithm>

// ----------------------------------------------------------------------------

cpu::tube::context::context(int, char*[],
                            char const* compiler, char const* flags)
    : d_compiler(compiler)
    , d_flags(flags)
      //-dk:TODO the fragmentation should probably be configurable
      // this will leave a the heap in a fragmented state
      // with many small allocations (1b-64k) littered around at random
      // , d_fragment(1024*1024, 64*1024)
    , d_fragment(1024, 64*1024)
{
    std::cout << "processor=" << cpu::tube::processor() << ' '
              << "compiler=" << compiler << ' '
              << "flags=" << flags << ' '
              << '\n';
}

void
cpu::tube::context::stub(char const* name)
{
    std::cout << std::setw(0) << name << "| 0 , ";
    std::cout << '\n';
}

void
cpu::tube::context::do_report(char const*                     name,
                              cpu::tube::duration             duration,
                              std::vector<std::string> const& argv)
{
    std::cout << std::setw(0) << name << '|'
              << std::setw(0) << duration << ',';
    std::copy(argv.begin(), argv.end(),
              std::ostream_iterator<std::string>(std::cout, ","));
    std::cout << '\n' << std::flush;
}
