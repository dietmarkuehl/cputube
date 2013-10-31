// functions.cpp                                                      -*-C++-*-
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

#include "cpu/tube/timer.hpp"
#include <cctype>
#include <cstddef>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

// ----------------------------------------------------------------------------

namespace test
{
    template <typename InIt, typename Predicate>
    std::size_t count(InIt it, InIt end, Predicate predicate)
    {
        std::size_t rc(0);
        for (; it != end; ++it) {
            if (predicate(*it)) {
                ++rc;
            }
        }
        return rc;
    }

    template <typename InIt, typename Predicate>
    void measure(char const* name, InIt begin, InIt end, Predicate predicate)
    {
        cpu::tube::timer timer;
        std::size_t result(0);
        for (int i(0); i != 100; ++i) {
            result += test::count(begin, end, predicate);
        }
        std::cout << std::setw(20) << name << ": "
                  << "time=" << std::setw(4) << timer << " "
                  << "result=" << result << '\n';
    }
}

// ----------------------------------------------------------------------------

namespace test
{
    inline bool isprint(unsigned char c)
    {
        return std::isprint(c);
    }

    struct isprint_t
    {
        bool operator()(unsigned char c) const {
            return std::isprint(c);
        }
    };

    struct inline_isprint_t
    {
        inline bool operator()(unsigned char c) const {
            return std::isprint(c);
        }
    };
}

// ----------------------------------------------------------------------------

int main()
{
    std::ifstream in("cpu/test/functions.cpp");
    std::vector<unsigned char> text{ std::istreambuf_iterator<char>(in),
                                     std::istreambuf_iterator<char>() };

    test::measure("heat", text.begin(), text.end(), &test::isprint);
    test::measure("naive", text.begin(), text.end(), &test::isprint);
    test::measure("naive2", text.begin(), text.end(), test::isprint);
    test::measure("lambda", text.begin(), text.end(),
                  [](unsigned char c) { return std::isprint(c); });
    test::measure("functor", text.begin(), text.end(), test::isprint_t());
    test::measure("inline functor", text.begin(), text.end(), test::inline_isprint_t());
    test::measure("function(naive)", text.begin(), text.end(),
                  std::function<bool(unsigned char)>(&test::isprint));
    test::measure("function(naive2)", text.begin(), text.end(),
                  std::function<bool(unsigned char)>(test::isprint));
    test::measure("function(functor)", text.begin(), text.end(),
                  std::function<bool(unsigned char)>(test::isprint_t()));
}
