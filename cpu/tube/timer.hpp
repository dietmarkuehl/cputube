// cpu/tube/timer.hpp                                                 -*-C++-*-
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

#ifndef INCLUDED_CPU_TUBE_TIMER
#define INCLUDED_CPU_TUBE_TIMER

#include <chrono>
#include <iosfwd>

// ----------------------------------------------------------------------------

namespace cpu
{
    namespace tube
    {
        class timer;
        class duration;
        std::ostream& operator<< (std::ostream&, cpu::tube::duration const&);
        std::ostream& operator<< (std::ostream&, cpu::tube::timer const&);
    }
}

// ----------------------------------------------------------------------------

class cpu::tube::duration
{
private:
    typedef std::chrono::high_resolution_clock clock;
    clock::time_point::duration d_duration;

public:
    duration(cpu::tube::timer const& timer);
    duration(clock::time_point::duration value): d_duration(value) {}
    std::ostream& print(std::ostream& out) const;
    unsigned long long microseconds() const;
};

// ----------------------------------------------------------------------------

class cpu::tube::timer
{
private:
    friend class cpu::tube::duration;
    typedef std::chrono::high_resolution_clock clock;
    clock::time_point d_start;

    timer(timer const&) = delete;
    void operator=(timer) = delete;

    clock::time_point::duration intern_measure() const {
        return clock::now() - this->d_start;
    }
public:
    timer(): d_start(clock::now()) {}
    timer(timer&& other): d_start(other.d_start) {}

    cpu::tube::duration measure() const {
        return this->intern_measure();
    }
};

// ----------------------------------------------------------------------------

inline cpu::tube::duration::duration(cpu::tube::timer const& timer)
    : d_duration(timer.intern_measure())
{
}

// ----------------------------------------------------------------------------

#endif
