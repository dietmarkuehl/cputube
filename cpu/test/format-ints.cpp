// cpu/test/format-ints.cpp                                           -*-C++-*-
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
#include "cpu/tube/protect.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <random>
#include <stdexcept>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

// ----------------------------------------------------------------------------
// This test simulates formatting a message ready to be logged somewhere. In
// the context of the discussion the following high-level description was
// given:
//     "Connection was aborted to {IP} {port}, cannot send message {message}"
// I don't know much about IPv6 so this just uses an IPv4 tuple of four 8 bit
// values and a 16 bit port. The message will be represented by an unsigned
// integer.

namespace {
    // Auxiliary class address to represent and IPv4 address
    struct address {
        unsigned char  ip[4];
        unsigned short port;
    };
    std::ostream& operator<< (std::ostream& out, address addr) {
        return out << int(addr.ip[0]) << '.'
                   << int(addr.ip[1]) << '.'
                   << int(addr.ip[2]) << '.'
                   << int(addr.ip[3]) << ':'
                   << int(addr.port);
    }

    // Auxiliary stream writing to a memory buffer as snprintf does
    struct membuf
        : std::streambuf {
        template <std::size_t N>
        explicit membuf(char (&buffer)[N]) {
            this->setp(buffer, buffer + N);
        }
        void reset()      { this->setp(this->pbase(), this->epptr()); }
        char* end() const { return this->pptr(); }
    };
    struct omemstream
        : virtual membuf
        , std::ostream {
        template <std::size_t N>
        explicit omemstream(char (&buffer)[N])
            : membuf(buffer)
            , std::ios(static_cast<std::streambuf*>(this))
            , std::ostream(static_cast<std::streambuf*>(this)) {
        }
        using membuf::end;
    };
}
 
namespace
{
    struct format_ostringstream
    {
        template <typename Fun>
        void process(address addr, std::vector<int> const& messages,
                     Fun fun) {
            for (int message: messages) {
                std::ostringstream out;
                out << "Connection was aborted to " << addr << ", "
                    << "cannot send message " << message << '\n';
                std::string result(out.str());
                fun(result.c_str(), result.c_str() + result.size());
            }
        }
    };

    struct format_ostringstream_hoisted
    {
        template <typename Fun>
        void process(address addr, std::vector<int> const& messages, Fun fun) {
            std::ostringstream out;
            for (int message: messages) {
                out.str(std::string());
                out << "Connection was aborted to " << addr << ", "
                    << "cannot send message " << message << '\n';
                std::string result(out.str());
                fun(result.c_str(), result.c_str() + result.size());
            }
        }
    };

    struct format_memstream
    {
        template <typename Fun>
        void process(address addr, std::vector<int> const& messages, Fun fun) {
            for (int message: messages) {
                char       buffer[256];
                omemstream out(buffer);
                out << "Connection was aborted to " << addr << ", "
                    << "cannot send message " << message << '\n';
                fun(buffer, out.end());
            }
        }
    };

    struct format_memstream_hoisted
    {
        template <typename Fun>
        void process(address addr, std::vector<int> const& messages, Fun fun) {
            char       buffer[256];
            omemstream out(buffer);
            
            for (int message: messages) {
                out.reset();
                out << "Connection was aborted to " << addr << ", "
                    << "cannot send message " << message << '\n';
                fun(buffer, out.end());
            }
        }
    };
 
    struct format_snprintf
    {
        template <typename Fun>
        void process(address addr, std::vector<int> const& messages, Fun fun) {
            for (int message: messages) {
                char buffer[256];
                int  end = snprintf(buffer, sizeof buffer,
                                    "Connection was aborted to "
                                    "%d.%d.%d.%d:%d, cannot send message %d\n",
                                    addr.ip[0],
                                    addr.ip[1],
                                    addr.ip[2],
                                    addr.ip[3],
                                    addr.port,
                                    message);
                fun(buffer, buffer + end);
            }
        }
    };
}

// ----------------------------------------------------------------------------

template <typename Formatter>
static void
measure(cpu::tube::context& context, char const* name, address addr,
        std::vector<int> const& values)
{
    auto timer = context.start();
    {
        Formatter formatter;
        int       count(30);
        for (int i = 0; i != 10000; ++i) {
            if (i == 50000) {
                formatter.process(addr, values,
                                  [=](char const* begin, char const* end) mutable {
                                      if (--count == 0) {
                                          std::cout << "result='";
                                          std::copy(begin, end,
                                                    std::ostreambuf_iterator<char>(std::cout));
                                          std::cout << "'\n";
                                      }
                                  });
            }
            else {
                formatter.process(addr, values,
                                  [](char const* begin, char const* end) {
                                      cpu::tube::prevent_optimize_away(begin);
                                      cpu::tube::prevent_optimize_away(end);
                                  });
            }
        }
    }
    context.report(name, timer);
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(ac, av, "<unknown>", "<unknown>", "<unknown>");
    std::vector<int>   values;
    std::minstd_rand   rand;
    rand.seed(17);
    address            addr = { { 192, 168, 0, 17 }, 4711 };
    std::generate_n(std::back_inserter(values), 1000, rand);
    std::locale::global(std::locale(std::locale(), new std::num_put<char, char*>()));

    measure<format_ostringstream>(        context, "ostringstream          ", addr, values);
    measure<format_ostringstream_hoisted>(context, "ostringstream (hoisted)", addr, values);
    measure<format_memstream>(            context, "memstream              ", addr, values);
    measure<format_memstream_hoisted>(    context, "memstream (hoisted)    ", addr, values);
    measure<format_snprintf>(             context, "snprintf               ", addr, values);
}
