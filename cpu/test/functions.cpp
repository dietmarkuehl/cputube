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
#include "cpu/tube/processor.hpp"
#include "boost/function.hpp"
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
        std::cout << std::setw(40) << "baseline" << ": ";
        test::run(cont.begin(), cont.end());
        {
            // compete
            cpu::tube::timer timer;
            std::size_t result(0);
            for (int i(0); i != 100; ++i) {
                result += test::run(cont.begin(), cont.end());
            }
            cpu::tube::duration duration(timer);
            std::cout << "time=" << std::setw(4) << duration << ' '
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
        std::cout << std::setw(40) << name << ": ";
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
            cpu::tube::duration duration(timer);
            std::cout << "time=" << std::setw(4) << duration << ' '
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

    struct virtual_isalnum_t
    {
        virtual int operator()(unsigned char c) const {
            return std::isalnum(c);
        }
    };

    struct member
    {
        int isalnum(unsigned char c) const {
            return std::isalnum(c);
        }
    };

    struct inline_member
    {
        inline int isalnum(unsigned char c) const {
            return std::isalnum(c);
        }
    };

    struct virtual_member
    {
        virtual int isalnum(unsigned char c) const {
            return std::isalnum(c);
        }
    };
}

// ----------------------------------------------------------------------------

int main()
{
    std::cout << "processor=" << cpu::tube::processor() << '\n';
    std::ifstream in("cpu/test/functions.cpp");
    std::vector<unsigned char> text{ std::istreambuf_iterator<char>(in),
                                     std::istreambuf_iterator<char>() };

    test::baseline(text);
    test::measure("function", text, test::isalnum);
    test::measure("function pointer", text, &test::isalnum);
    test::measure("member pointer", text, std::bind(&test::member::isalnum, test::member(), std::placeholders::_1));
    test::measure("inline member pointer", text, std::bind(&test::inline_member::isalnum, test::inline_member(), std::placeholders::_1));
    test::measure("virtual member pointer", text, std::bind(&test::virtual_member::isalnum, test::virtual_member(), std::placeholders::_1));
    test::measure("lambda", text,
                  [](unsigned char c) { return std::isalnum(c); });
    test::measure("function object", text, test::isalnum_t());
    test::measure("function object (inline)", text, test::inline_isalnum_t());
    test::measure("function object (virtual)", text, test::virtual_isalnum_t());
    test::isalnum_t function_object;
    test::measure("ref(function object)", text, std::ref(function_object));
    test::inline_isalnum_t inline_function_object;
    test::measure("ref(function object (inline))", text, std::ref(inline_function_object));
    test::virtual_isalnum_t virtual_function_object;
    test::measure("ref(function object (virtual))", text, std::ref(virtual_function_object));
    test::measure("function(naive)", text,
                  std::function<int(unsigned char)>(&test::isalnum));
    test::measure("function(naive2)", text,
                  std::function<int(unsigned char)>(test::isalnum));
    test::measure("function(functor)", text,
                  std::function<int(unsigned char)>(test::isalnum_t()));
    test::measure("boost::function(naive)", text,
                  boost::function<int(unsigned char)>(&test::isalnum));
    test::measure("boost::function(functor)", text,
                  boost::function<int(unsigned char)>(test::isalnum_t()));
}
