// cpu/test/write-characters.cpp                                      -*-C++-*-
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

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

static char const* text[] = {
    "Permission is hereby granted, free of charge, to any person          \n", 
    "obtaining a copy of this software and associated documentation       \n", 
    "files (the \"Software\"), to deal in the Software without restriction, \n", 
    "including without limitation the rights to use, copy, modify,        \n", 
    "merge, publish, distribute, sublicense, and/or sell copies of        \n", 
    "the Software, and to permit persons to whom the Software is          \n", 
    "furnished to do so, subject to the following conditions:             \n", 
    "                                                                     \n", 
    "The above copyright notice and this permission notice shall be       \n", 
    "included in all copies or substantial portions of the Software.      \n", 
    "                                                                     \n", 
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,      \n", 
    "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      \n", 
    "OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             \n", 
    "NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          \n", 
    "HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         \n", 
    "WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         \n", 
    "FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        \n", 
    "OTHER DEALINGS IN THE SOFTWARE.\n"
};
 
// ----------------------------------------------------------------------------

namespace
{
    struct FStreamWrite
    {
        std::ofstream d_stream;
        FStreamWrite(char const* filename): d_stream(filename) {}
        void process(char const* line) {
            this->d_stream.write(line, strlen(line));
        }
    };

    struct FStreambufSputn
    {
        std::ofstream d_stream;
        FStreambufSputn(char const* filename): d_stream(filename) {}
        void process(char const* line) {
            this->d_stream.rdbuf()->sputn(line, strlen(line));
        }
    };

    struct FStreamPut
    {
        std::ofstream d_stream;
        FStreamPut(char const* filename): d_stream(filename) {}
        void process(char const* line) {
            for (; *line; ++line) {
                this->d_stream.put(*line);
            }
        }
    };

    struct FStreambufSputc
    {
        std::ofstream d_stream;
        FStreambufSputc(char const* filename): d_stream(filename) {}
        void process(char const* line) {
            std::streambuf* sbuf = this->d_stream.rdbuf();
            for (; *line; ++line) {
                sbuf->sputc(*line);
            }
        }
    };

    struct FStreamOStreamIterator
    {
        std::ofstream d_stream;
        FStreamOStreamIterator(char const* filename): d_stream(filename) {}
        void process(char const* line) {
            std::copy(line, line + strlen(line),
                      std::ostream_iterator<char>(this->d_stream));
        }
    };

    struct FStreamOStreambufIterator
    {
        std::ofstream d_stream;
        FStreamOStreambufIterator(char const* filename): d_stream(filename) {}
        void process(char const* line) {
            std::copy(line, line + strlen(line),
                      std::ostreambuf_iterator<char>(this->d_stream));
        }
    };

    struct FStreamOStreambufIteratorLoop
    {
        std::ofstream d_stream;
        FStreamOStreambufIteratorLoop(char const* filename): d_stream(filename) {}
        void process(char const* line) {
            std::ostreambuf_iterator<char> to(this->d_stream);
            for (; *line; ++line, ++to) {
                *to = *line;
            }
        }
    };

    struct FILEFWrite
    {
        FILE* d_file;
        FILEFWrite(char const* filename): d_file(fopen(filename, "w")) {
            if (!this->d_file) {
                throw std::runtime_error("failed to fopen() the file");
            }
        }
        ~FILEFWrite() { fclose(this->d_file); }
        void process(char const* line) {
            fwrite(line, strlen(line), 1, this->d_file);
        }
    };

    struct FILEFPrintf
    {
        FILE* d_file;
        FILEFPrintf(char const* filename): d_file(fopen(filename, "w")) {
            if (!this->d_file) {
                throw std::runtime_error("failed to fopen() the file");
            }
        }
        ~FILEFPrintf() { fclose(this->d_file); }
        void process(char const* line) {
            fprintf(this->d_file, line);
        }
    };

    struct FILEFPuts
    {
        FILE* d_file;
        FILEFPuts(char const* filename): d_file(fopen(filename, "w")) {
            if (!this->d_file) {
                throw std::runtime_error("failed to fopen() the file");
            }
        }
        ~FILEFPuts() { fclose(this->d_file); }
        void process(char const* line) {
            fputs(line, this->d_file);
        }
    };

    struct FILEFputc
    {
        FILE* d_file;
        FILEFputc(char const* filename): d_file(fopen(filename, "w")) {
            if (!this->d_file) {
                throw std::runtime_error("failed to fopen() the file");
            }
        }
        ~FILEFputc() { fclose(this->d_file); }
        void process(char const* line) {
            for (; *line; ++line) {
                fputc(*line, this->d_file);
            }
        }
    };

    struct FILEPutc
    {
        FILE* d_file;
        FILEPutc(char const* filename): d_file(fopen(filename, "w")) {
            if (!this->d_file) {
                throw std::runtime_error("failed to fopen() the file");
            }
        }
        ~FILEPutc() { fclose(this->d_file); }
        void process(char const* line) {
            for (; *line; ++line) {
                putc(*line, this->d_file);
            }
        }
    };

    struct FILEPutcUnlocked
    {
        FILE* d_file;
        FILEPutcUnlocked(char const* filename): d_file(fopen(filename, "w")) {
            if (!this->d_file) {
                throw std::runtime_error("failed to fopen() the file");
            }
        }
        ~FILEPutcUnlocked() { fclose(this->d_file); }
        void process(char const* line) {
            for (; *line; ++line) {
                putc_unlocked(*line, this->d_file);
            }
        }
    };

    struct FILEWrite
    {
        int d_fd;
        FILEWrite(char const* filename): d_fd(open(filename, O_WRONLY)) {
            if (this->d_fd < 0) {
                throw std::runtime_error("failed to open() the file");
            }
        }
        ~FILEWrite() { close(this->d_fd); }
        void process(char const* line) {
            write(this->d_fd, line, strlen(line));
        }
    };
}

// ----------------------------------------------------------------------------

template <typename File>
static void
measure(cpu::tube::context& context, char const* name, char const* filename)
{
    auto timer = context.start();
    {
        File file(filename);
        for (int i = 0; i != 1000; ++i) {
            for (char const* line: text) {
                file.process(line);
            }
        }
    }
    context.report(name, timer);
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    try
    {
        cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
        char const* name(ac == 1? "/dev/null": av[1]);
        measure<FILEWrite>(context, "::write", name);
        measure<FILEFPrintf>(context, "std::fprintf", name);
        measure<FILEFPuts>(context, "std::fputs", name);
        measure<FILEFWrite>(context, "std::fwrite", name);
        measure<FILEFputc>(context, "std::fputc", name);
        measure<FILEPutc>(context, "std::putc", name);
        measure<FILEPutcUnlocked>(context, "std::putc_unlocked", name);
        measure<FStreamWrite>(context, "std::ofstream::write", name);
        measure<FStreambufSputn>(context, "std::streambuf::sputn", name);
        measure<FStreamPut>(context, "std::ofstream::put", name);
        measure<FStreambufSputc>(context, "std::streambuf::sputc", name);
        measure<FStreamOStreamIterator>(context, "std::ostream_iterator", name);
        measure<FStreamOStreambufIterator>(context, "std::ostreambuf_iterator", name);
        measure<FStreamOStreambufIteratorLoop>(context, "std::ostreambuf_iterator loop", name);
    }
    catch (std::exception const& ex)
    {
        std::cerr << "ERROR: " << ex.what() << "\n";
    }
}
