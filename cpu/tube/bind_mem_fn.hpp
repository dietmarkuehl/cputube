// cpu/tube/bind_mem_fn.hpp -*-C++-*-
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

#ifndef INCLUDED_CPU_TUBE_BIND_MEM_FN
#define INCLUDED_CPU_TUBE_BIND_MEM_FN

// ----------------------------------------------------------------------------

namespace cpu
{
    namespace tube
    {
        template <typename O, typename T>
        class bound_mem_fn;

        template <typename O, typename R, typename S, typename A>
        class bound_mem_fn<O, R (S::*)(A) const> {
            O* d_object;
            R (S::*d_member)(A) const;
        public:
            bound_mem_fn(O* object, R (S::*member)(A) const)
                : d_object(object)
                , d_member(member) {
            }
            R operator()(A arg) const {
                return (this->d_object->*this->d_member)(arg);
            }
        };

        template <typename O, typename T>
        bound_mem_fn<O, T> bind_mem_fn(T member, O& object) {
            return bound_mem_fn<O, T>(&object, member);
        }
        template <typename O, typename T>
        bound_mem_fn<O const, T> bind_mem_fn(T member, O const& object) {
            return bound_mem_fn<O const, T>(&object, member);
        }
        template <typename O, typename T>
        bound_mem_fn<O, T> bind_mem_fn(T member, O* object) {
            return bound_mem_fn<O, T>(object, member);
        }
    }
}

// ----------------------------------------------------------------------------

#endif
