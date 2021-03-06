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

#include "cpu/tube/context.hpp"
#include "cpu/tube/bind_mem_fn.hpp"
#include "cpu/tube/reference_wrapper.hpp"
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
            rc += std::isalnum(static_cast<unsigned char>(*it));
        }
        return rc;
    }

    template <typename Container>
    void baseline(cpu::tube::context& context, Container const& cont)
    {
        test::run(cont.begin(), cont.end());
        {
            // compete
            cpu::tube::timer timer = context.start();
            std::size_t result(0);
            for (int i(0); i != 10000; ++i) {
                result += test::run(cont.begin(), cont.end());
            }
            context.report("baseline", timer, result);
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
    void measure(cpu::tube::context& context,
                 char const* name, Container const& cont, Function function)
    {
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
            cpu::tube::timer timer = context.start();
            std::size_t result(0);
            for (int i(0); i != 10000; ++i) {
                result += test::run(cont.begin(), cont.end(), function);
            }
            context.report(name, timer, result);
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
        typedef unsigned char argument_type;
        typedef int           result_type;
        int operator()(unsigned char c) const {
            return std::isalnum(c);
        }
    };

    struct inline_isalnum_t
    {
        typedef unsigned char argument_type;
        typedef int           result_type;
        inline int operator()(unsigned char c) const {
            return std::isalnum(c);
        }
    };

    struct virtual_isalnum_t
    {
        typedef unsigned char argument_type;
        typedef int           result_type;
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

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));

    std::ifstream in("cpu/test/functions.cpp");
    std::vector<unsigned char> text((std::istreambuf_iterator<char>(in)),
                                    std::istreambuf_iterator<char>());

    test::baseline(context, text);
    test::measure(context, "function", text, test::isalnum);
    test::measure(context, "function pointer", text, &test::isalnum);
    test::measure(context, "function object", text, test::isalnum_t());
    test::measure(context, "function object (inline)", text, test::inline_isalnum_t());
    test::measure(context, "function object (virtual)", text, test::virtual_isalnum_t());
    test::measure(context, "member pointer", text, cpu::tube::bind_mem_fn(&test::member::isalnum, test::member()));
    test::measure(context, "inline member pointer", text, cpu::tube::bind_mem_fn(&test::inline_member::isalnum, test::inline_member()));
    test::measure(context, "virtual member pointer", text, cpu::tube::bind_mem_fn(&test::virtual_member::isalnum, test::virtual_member()));

    test::isalnum_t function_object;
    test::measure(context, "ref(function object)", text, cpu::tube::ref(function_object));
    test::inline_isalnum_t inline_function_object;
    test::measure(context, "ref(function object (inline))", text, cpu::tube::ref(inline_function_object));
    test::virtual_isalnum_t virtual_function_object;
    test::measure(context, "ref(function object (virtual))", text, cpu::tube::ref(virtual_function_object));
#if defined(USE_CXX11)
    test::measure(context, "lambda", text,
                  [](unsigned char c) { return std::isalnum(c); });
    test::measure(context, "lambda via function pointer", text,
                  static_cast<int(*)(unsigned char)>([](unsigned char c) { return std::isalnum(c); }));
#  if !defined(__INTEL_COMPILER)
    test::measure(context, "std::function (naive)", text,
                  std::function<int(unsigned char)>(&test::isalnum));
    test::measure(context, "std::function (naive2)", text,
                  std::function<int(unsigned char)>(test::isalnum));
    test::measure(context, "std::function (functor)", text,
                  std::function<int(unsigned char)>(test::isalnum_t()));
#  endif
#endif
    test::measure(context, "boost::function(naive)", text,
                  boost::function<int(unsigned char)>(&test::isalnum));
    test::measure(context, "boost::function(functor)", text,
                  boost::function<int(unsigned char)>(test::isalnum_t()));
    test::baseline(context, text);
}
