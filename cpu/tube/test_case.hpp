// cpu/tube/test_case.hpp                                             -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2015 Dietmar Kuehl http://www.dietmar-kuehl.de         
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

#ifndef INCLUDED_CPU_TUBE_TEST_CAST
#define INCLUDED_CPU_TUBE_TEST_CAST

#include <string>

// ----------------------------------------------------------------------------

namespace cpu {
    namespace tube {
        template <typename T> class test_case;
        template <typename T>
        test_case<T> make_test_case(std::string const&, T const&);
    }
}

// ----------------------------------------------------------------------------

template <typename T>
class cpu::tube::test_case
{
    std::string d_name;
    T           d_test;
public:
    explicit test_case(std::string const& name, T const& test = T())
        : d_name(name)
        , d_test(test) {
    }
    std::string name() const { return this->d_name; }
    T const&    test() const { return this->d_test; }
};

// ----------------------------------------------------------------------------

template <typename T>
cpu::tube::test_case<T>
cpu::tube::make_test_case(std::string const& name, T const& test)
{
    return cpu::tube::test_case<T>(name, test);
}

// ----------------------------------------------------------------------------

#endif
