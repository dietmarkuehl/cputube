// cpu/tube/timer.cpp                                                 -*-C++-*-
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
#include <iostream>

// ----------------------------------------------------------------------------
unsigned long cpu::tube::duration::microseconds() const
{
    using namespace cpu::tube::chrono;
    return duration_cast<cpu::tube::chrono::microseconds>(this->d_duration).count();
}

std::ostream& cpu::tube::duration::print(std::ostream& out) const
{
    return out << this->microseconds();
}

std::ostream& cpu::tube::operator<< (std::ostream&              out,
                                     cpu::tube::duration const& duration)
{
    return duration.print(out);
}

// ----------------------------------------------------------------------------

std::ostream& cpu::tube::operator<< (std::ostream&           out,
                                     cpu::tube::timer const& timer)
{
    using namespace cpu::tube::chrono;
    return out << timer.measure();
}
