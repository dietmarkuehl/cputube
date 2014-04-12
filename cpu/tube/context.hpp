// cpu/tube/context.hpp                                               -*-C++-*-
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

#ifndef INCLUDED_CPU_TUBE_CONTEXT
#define INCLUDED_CPU_TUBE_CONTEXT

#include "cpu/tube/timer.hpp"
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#define CPUTUBE_CONTEXT_ARGS(ac, av) ac, av, CPUTUBE_COMPILER, CPUTUBE_FLAGS

// ----------------------------------------------------------------------------

namespace cpu
{
    namespace tube
    {
        class context;
    }
}

// ----------------------------------------------------------------------------

class cpu::tube::context
{
private:
    char const* d_compiler;
    char const* d_flags;

    static void format(std::vector<std::string>&) {}
    template <typename H, typename... T>
    static void format(std::vector<std::string>& argv, H&& value, T&&... tail);

    void do_report(char const* name, cpu::tube::duration duration,
                   std::vector<std::string> const& argv);
public:
    context(int ac, char* av[], char const* compiler, char const* flags);

    cpu::tube::timer start();

    void stub(char const* name);
    template <typename... T>
    void report(char const* name, cpu::tube::timer& timer, T&&... args);
    template <typename... T>
    void report(char const* name, cpu::tube::duration duration, T&&... args);
};

// ----------------------------------------------------------------------------

template <typename H, typename... T>
void
cpu::tube::context::format(std::vector<std::string>& argv,
                           H&& value, T&&... tail)
{
    std::ostringstream out;
    out << value;
    argv.push_back(out.str());
    format(argv, std::forward<T>(tail)...);
}

inline cpu::tube::timer
cpu::tube::context::start()
{
    return cpu::tube::timer();
}

template <typename... T>
void
cpu::tube::context::report(char const*       name,
                           cpu::tube::timer& timer,
                           T&&...            args)
{
    this->report(name, timer.measure(), std::forward<T>(args)...);
}

template <typename... T>
void
cpu::tube::context::report(char const*         name,
                           cpu::tube::duration duration,
                           T&&...              args)
{
    std::vector<std::string> argv;
    cpu::tube::context::format(argv, std::forward<T>(args)...);
    this->do_report(name, duration, argv);
}

// ----------------------------------------------------------------------------

#endif
