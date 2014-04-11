// cpu/test/write-ints.cpp                                            -*-C++-*-
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
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

namespace
{
    struct copy_ostream_iterator
    {
        std::ofstream d_out;
        copy_ostream_iterator(char const* name): d_out(name) {}
        void process(std::vector<int> const& values) {
            std::copy(values.begin(), values.end(),
                      std::ostream_iterator<int>(this->d_out, " "));
            this->d_out << '\n';
        }
    };
 
#if !defined(__INTEL_COMPILER)
    struct to_string_stream
    {
        std::ofstream d_out;
        to_string_stream(char const* name): d_out(name) {}
        void process(std::vector<int> const& values) {
            for (int x: values) {
                this->d_out << std::to_string(x) << ' ';
            }
            this->d_out << '\n';
        }
    };
#endif
 
    struct fprintf_values
    {
        FILE* d_file;
        fprintf_values(char const* name): d_file(fopen(name, "w")) {
            if (!this->d_file) {
                throw std::runtime_error("failed to open file");
            }
            setvbuf(this->d_file, (char *)NULL, _IOFBF, 0);
        }
        ~fprintf_values() { fclose(this->d_file); }
        void process(std::vector<int> const& values) {
            for (int x: values) {
                fprintf(this->d_file, "%d ", x);
            }
            fputc('\n', this->d_file);
        }
    };

    struct snprintf_values
    {
        FILE*             d_file;
        std::vector<char> d_buffer;

        snprintf_values(char const* name)
            : d_file(fopen(name, "w"))
            , d_buffer(1000 * 12)
        {
            if (!this->d_file) {
                throw std::runtime_error("failed to open file");
            }
        }
        ~snprintf_values() { fclose(this->d_file); }
        void process(std::vector<int> const& values) {
            char* next(this->d_buffer.data());
            char* end(this->d_buffer.data() + this->d_buffer.size());
            for (auto&& x: values) {
                next += snprintf(next, end - next, "%d ", x);
            }
            fputs(this->d_buffer.data(), this->d_file);
            fputc('\n', this->d_file);
        }
    };
}

// ----------------------------------------------------------------------------

template <typename File>
static void
measure(cpu::tube::context& context, char const* name, char const* filename,
        std::vector<int> const& values)
{
    auto timer = context.start();
    {
        File file(filename);
        for (int i = 0; i != 10000; ++i) {
            file.process(values);
        }
    }
    context.report(name, timer);
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    char const* name(ac == 1? "/dev/null": av[1]);
    std::vector<int>   values;
    std::generate_n(std::back_inserter(values), 1000, &rand);
    measure<copy_ostream_iterator>(context, "copy ostream_iterator", name, values);
#if !defined(__INTEL_COMPILER)
    measure<to_string_stream>(context, "to_string()/ofstream", name, values);
#else
    context.stub("to_string()/ofstream");
#endif
    measure<fprintf_values>(context, "fprintf values", name, values);
    measure<snprintf_values>(context, "snprintf/fputs values", name, values);
}
