// cpu/tube/chrono.hpp                                                -*-C++-*-
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
// This file implements a minimalistic <chrono> replacement unless USE_CXX11
// is defined. In either case it will make suitable names for a time available
// in namespace cpu::tube::chrono.

#ifndef INCLUDED_CPU_TUBE_CHRONO
#define INCLUDED_CPU_TUBE_CHRONO

#ifdef USE_CXX11

// ----------------------------------------------------------------------------

#include <chrono>

namespace cpu
{
    namespace tube
    {
        namespace chrono
        {
            using std::chrono::high_resolution_clock;
            using std::chrono::duration_cast;
            using std::chrono::microseconds;
        }
    }
}

// ----------------------------------------------------------------------------

#else // USE_CXX11

#include "cpu/tube/ratio.hpp"

// ----------------------------------------------------------------------------

namespace cpu
{
    namespace tube
    {
        template <typename, typename> struct common_type;
        template <typename T> struct common_type<T, T> { typedef T type; };

        namespace chrono
        {
            template <typename Rep, typename Period = ratio<1> >
            class duration;

            template <typename Rep1, typename Period1,
                      typename Rep2, typename Period2>
            typename cpu::tube::common_type<cpu::tube::chrono::duration<Rep1, Period1>,
                                      cpu::tube::chrono::duration<Rep2, Period2> >::type
                operator-(duration<Rep1, Period1> const& lhs,
                          duration<Rep2, Period2> const& rhs);

            template <typename ToDuration, typename Rep, typename Period>
            ToDuration duration_cast(duration<Rep, Period> const&);

            typedef duration<long, cpu::tube::micro> microseconds;

            template <class Clock, class Duration = typename Clock::duration>
            class time_point;

            template <class Clock, class Duration1, class Duration2>
            typename cpu::tube::common_type<Duration1, Duration2>::type
            operator-(time_point<Clock, Duration1> const& lhs,
                      time_point<Clock, Duration2> const& rhs);

            class high_resolution_clock;
        }
    }
}

// ----------------------------------------------------------------------------

template <typename Rep, typename Period>
class cpu::tube::chrono::duration
{
public:
    typedef Rep    rep;
    typedef Period period;

private:
    rep d_value;

public:
    duration(rep value): d_value(value) {}

    rep count() const { return this->d_value; }
};

// ----------------------------------------------------------------------------

template <typename Rep1, typename Period1, typename Rep2, typename Period2>
typename cpu::tube::common_type<cpu::tube::chrono::duration<Rep1, Period1>,
                                          cpu::tube::chrono::duration<Rep2, Period2> >::type
cpu::tube::chrono::operator-(cpu::tube::chrono::duration<Rep1, Period1> const& lhs,
                             cpu::tube::chrono::duration<Rep2, Period2> const& rhs)
{
    return lhs.count() - rhs.count();
}

// ----------------------------------------------------------------------------

template <typename ToDuration, typename Rep, typename Period>
ToDuration
cpu::tube::chrono::duration_cast(cpu::tube::chrono::duration<Rep, Period> const& from) {
    return ToDuration(from);
}

// ----------------------------------------------------------------------------

template <class Clock, class Duration>
class cpu::tube::chrono::time_point
{
public:
    typedef Clock                     clock;
    typedef Duration                  duration;
    typedef typename duration::rep    rep;
    typedef typename duration::period period;

private: 
    rep d_value;

public:
    time_point(rep value): d_value(value) {}
    
    duration time_since_epoch() const {
        return duration(this->d_value);
    }
};

// ----------------------------------------------------------------------------

template <class Clock, class Duration1, class Duration2>
typename cpu::tube::common_type<Duration1, Duration2>::type
cpu::tube::chrono::operator-(cpu::tube::chrono::time_point<Clock, Duration1> const& lhs,
                       cpu::tube::chrono::time_point<Clock, Duration2> const& rhs)
{
    return lhs.time_since_epoch() - rhs.time_since_epoch();
}

// ----------------------------------------------------------------------------

class cpu::tube::chrono::high_resolution_clock
{
public:
    typedef long                                         rep;
    typedef cpu::tube::ratio<1, 1000000>                 period;
    typedef cpu::tube::chrono::duration<rep, period>     duration;
    typedef cpu::tube::chrono::time_point<rep, duration> time_point;

    static bool const is_steady = true;

    static time_point now();
};

// ----------------------------------------------------------------------------

#endif // USE_CXX11

#endif
