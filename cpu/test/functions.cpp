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
    template <typename InIt>
    std::size_t run(InIt it, InIt end)
    {
        std::size_t rc(0);
        for (; it != end; ++it) {
            rc += std::isalnum(*it);
        }
        return rc;
    }

    template <typename Container>
    void baseline(Container const& cont)
    {
        std::cout << std::setw(20) << "baseline" << ": ";
        test::run(cont.begin(), cont.end());
        {
            // compete
            cpu::tube::timer timer;
            std::size_t result(0);
            for (int i(0); i != 100; ++i) {
                result += test::run(cont.begin(), cont.end());
            }
            std::cout << "time=" << std::setw(4) << timer << ' '
                      << "result=" << result << '\n';
        }
    }

    template <typename InIt, typename Function>
    std::size_t run(InIt it, InIt end, Function function)
    {
        std::size_t rc(0);
        for (; it != end; ++it) {
            rc += function(*it);
        }
        return rc;
    }

    template <typename Container, typename Function>
    void measure(char const* name, Container const& cont, Function function)
    {
        std::cout << std::setw(20) << name << ": ";
        // qualify
        typedef unsigned char uc_t;
        for (unsigned short s(0); s != std::numeric_limits<uc_t>::max(); ++s) {
            if (std::isalnum(uc_t(s)) != function(uc_t(s))) {
                std::cout << "qualification failed: "
                          << "c=" << s << ' '
                          << "expect=" << std::isalnum(uc_t(s)) << ' '
                          << "got=" << function(uc_t(s)) << '\n';
                return;
            }
        }
        // warm up
        test::run(cont.begin(), cont.end(), function);
        {
            // compete
            cpu::tube::timer timer;
            std::size_t result(0);
            for (int i(0); i != 100; ++i) {
                result += test::run(cont.begin(), cont.end(), function);
            }
            std::cout << "time=" << std::setw(4) << timer << ' '
                      << "result=" << result << '\n';
        }
    }
}

// ----------------------------------------------------------------------------

namespace test
{
    inline int isalnum(unsigned char c)
    {
        return std::isalnum(c);
    }

    struct isalnum_t
    {
        int operator()(unsigned char c) const {
            return std::isalnum(c);
        }
    };

    struct inline_isalnum_t
    {
        inline int operator()(unsigned char c) const {
            return std::isalnum(c);
        }
    };
}

// ----------------------------------------------------------------------------

int main()
{
    std::ifstream in("cpu/test/functions.cpp");
    std::vector<unsigned char> text{ std::istreambuf_iterator<char>(in),
                                     std::istreambuf_iterator<char>() };

    test::baseline(text);
    test::baseline(text);
    test::baseline(text);
    test::measure("naive", text, &test::isalnum);
    test::measure("naive2", text, test::isalnum);
    test::measure("lambda", text,
                  [](unsigned char c) { return std::isalnum(c); });
    test::measure("functor", text, test::isalnum_t());
    test::measure("inline functor", text, test::inline_isalnum_t());
    test::measure("function(naive)", text,
                  std::function<int(unsigned char)>(&test::isalnum));
    test::measure("function(naive2)", text,
                  std::function<int(unsigned char)>(test::isalnum));
    test::measure("function(functor)", text,
                  std::function<int(unsigned char)>(test::isalnum_t()));
}
