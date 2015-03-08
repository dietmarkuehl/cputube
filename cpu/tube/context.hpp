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
#include "cpu/tube/heap_fragment.hpp"
#include "cpu/tube/test_case.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#define CPUTUBE_CONTEXT_ARGS(ac, av) ac, av, CPUTUBE_ARCH, CPUTUBE_COMPILER, CPUTUBE_FLAGS

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
    std::string     d_testname;
    std::string     d_arch;
    std::string     d_processor;
    char const*     d_compiler;
    char const*     d_flags;
    heap_fragmenter d_fragment;
    std::ofstream   d_json;
	
    template <typename T>
    static void format(std::vector<std::string>& argv, T const& value);

    void do_report(char const* name, cpu::tube::duration duration,
                   std::vector<std::string> const& argv);

    template <typename Measure, typename Case>
    void intern_run(std::ostream&, int start, int end, Measure measure,
                    cpu::tube::test_case<Case> case_, char const* = "");
    template <typename Measure, typename Case, typename... Cases>
    void intern_run(std::ostream&, int start, int end, Measure measure,
                    cpu::tube::test_case<Case> case_,
                    cpu::tube::test_case<Cases>... cases);

    context(context const&) = delete;
    void operator=(context const&) = delete;
public:
    context(int ac, char* av[], char const* arch, char const* compiler, char const* flags);
    ~context();

    cpu::tube::timer start();

    void stub(char const* name);
    void stub(std::string const& name) { this->stub(name.c_str()); }
    void report(char const* name, cpu::tube::timer& timer);
    template <typename T>
    void report(char const* name, cpu::tube::timer& timer, T const& arg);
    void report(char const* name, cpu::tube::duration duration);
    template <typename T>
    void report(char const* name, cpu::tube::duration duration, T const& arg);

    template <typename T>
    void report(std::string const& name, cpu::tube::timer& timer, T const& arg) {
        this->report(name.c_str(), timer, arg);
    }
    template <typename T>
    void report(std::string const& name, cpu::tube::duration duration, T const& arg) {
        this->report(name.c_str(), duration, arg);
    }

    template <typename Measure, typename... Cases>
    void run(int start, int end, std::string const& group, Measure measure,
             cpu::tube::test_case<Cases>... cases);
};

// ----------------------------------------------------------------------------

template <typename T>
void
cpu::tube::context::format(std::vector<std::string>& argv, T const& value)
{
    std::ostringstream out;
    out << value;
    argv.push_back(out.str());
}

inline cpu::tube::timer
cpu::tube::context::start()
{
    return cpu::tube::timer();
}

inline void
cpu::tube::context::report(char const* name, cpu::tube::timer& timer)
{
    this->report(name, timer.measure());
}

template <typename T>
void
cpu::tube::context::report(char const*       name,
                           cpu::tube::timer& timer,
                           T const&          arg)
{
    this->report(name, timer.measure(), arg);
}

inline void
cpu::tube::context::report(char const* name, cpu::tube::duration duration)
{
    std::vector<std::string> argv;
    this->do_report(name, duration, argv);
}

template <typename T>
void
cpu::tube::context::report(char const*         name,
                           cpu::tube::duration duration,
                           T const&            arg)
{
    std::vector<std::string> argv;
    cpu::tube::context::format(argv, arg);
    this->do_report(name, duration, argv);
}

// ----------------------------------------------------------------------------

template <typename Measure, typename Case>
void
cpu::tube::context::intern_run(std::ostream&              out,
                               int                        start,
                               int                        end,
                               Measure                    measure,
                               cpu::tube::test_case<Case> case_,
                               char const*                separator)
{
    out << "{ "
        << "name:\"" << case_.name() << "\", "
        << "results:[ ";
    bool first(true);
    for (int i(start); i <= end; i *= 10) {
        for (int j(1); j < 10; j *= 2) {
            int size(i * j);
            auto result = measure.measure(*this, size, case_.test());
            this->report(case_.name().c_str(), result.first, size);
            out << (first? "": ",")
                << " { "
                << "result:\"" << result.second << "\", "
                << "size:" << size << ", "
                << "time:" << result.first << "  "
                << "} " << std::flush;
            first = false;
        }
    }
    out << "] }" << separator << "\n";
}

template <typename Measure, typename Case, typename... Cases>
void
cpu::tube::context::intern_run(std::ostream& out,
                               int start, int end,
                               Measure measure,
                               cpu::tube::test_case<Case> case_,
                               cpu::tube::test_case<Cases>... cases)
{
    this->intern_run(out, start, end, measure, case_, ", ");
    this->intern_run(out, start, end, measure, cases...);
}

template <typename Measure, typename... Cases>
void
cpu::tube::context::run(int                             start,
                        int                             end,
                        std::string const&              group,
                        Measure                         measure,
                        cpu::tube::test_case<Cases>...  cases)
{
    if (this->d_json) {
        this->d_json << ",\n";
    }
    else {
        this->d_json.open("results/" + this->d_testname + ".json");
        this->d_json << "[ ";
    }
    this->d_json << "{ "
                 << "group:\"" << group << "\", "
                 << "arch:\"" << this->d_arch << "\" "
                 << "processor:\"" << this->d_processor << "\" "
                 << "compiler:\"" << this->d_compiler << "\" "
                 << "flags:\"" << this->d_flags << "\" "
                 << "test_cases:[\n";
    this->intern_run(this->d_json, start, end, measure, cases...);
    this->d_json << "] }\n";
}

// ----------------------------------------------------------------------------

#endif
