// cpu/tube/reference_wrapper.hpp -*-C++-*-
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

#ifndef INCLUDED_CPU_TUBE_REFERENCE_WRAPPER
#define INCLUDED_CPU_TUBE_REFERENCE_WRAPPER

#if defined(USE_CXX11) && !defined(__INTEL_COMPILER)

// ----------------------------------------------------------------------------

#include <functional>

namespace cpu
{
    namespace tube
    {
        using std::ref;
    }
}

// ----------------------------------------------------------------------------

#else // USE_CXX11

// ----------------------------------------------------------------------------

namespace cpu
{
    namespace tube
    {
        template <typename T>
        class reference_wrapper
        {
            T* d_pointer;
        public:
            reference_wrapper(T& object): d_pointer(&object) {}

            typename T::result_type operator()(typename T::argument_type arg) {
                return (*this->d_pointer)(arg);
            }
        };

        template <typename T>
        reference_wrapper<T const> ref(T const& object) {
            return reference_wrapper<T const>(object);
        }
        template <typename T>
        reference_wrapper<T> ref(T& object) {
            return reference_wrapper<T>(object);
        }
    }
}

// ----------------------------------------------------------------------------

#endif // USE_CXX11

#endif // INCLUDE_CPU_TUBE_REFERENCE_WRAPPER
