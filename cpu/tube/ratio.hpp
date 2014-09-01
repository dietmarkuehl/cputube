// cpu/tube/ratio.hpp -*-C++-*-
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
// This is a replacement for <ratio> for non-C++11 compilers. If USE_CXX11 is
// defined <ratio> is used. In either case, the necessary names are made
// available in namespace cpu::tube.

#ifndef INCLUDED_CPU_TUBE_RATIO
#define INCLUDED_CPU_TUBE_RATIO

#ifdef USE_CXX11

// ----------------------------------------------------------------------------

#include <ratio>

namespace cpu
{
    namespace tube
    {
        using std::ratio;
        using std::milli;
        using std::micro;
    }
}

// ----------------------------------------------------------------------------

#else // USE_CXX11

// ----------------------------------------------------------------------------

namespace cpu
{
    namespace tube
    {
        template <long, long = 1> class ratio;

        typedef ratio<1, 1000>    milli;
        typedef ratio<1, 1000000> micro;
    }
}

// ----------------------------------------------------------------------------

#endif // USE_CXX11

#endif
