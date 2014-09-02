// cpu/test/ptr-function-calls.hpp                                    -*-C++-*-
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

#ifndef INCLUDED_CPU_TEST_PTR_FUNCTION_CALLS
#define INCLUDED_CPU_TEST_PTR_FUNCTION_CALLS

// ----------------------------------------------------------------------------

namespace pfc
{
    // naming schema:
    // nm => non-member
    // s  => static
    // nv => non-virtual
    // v  => virtual
    // o  => defined in other source file
    // i  => defined inline
    // c  => defined in the class body or upon its first declaration
    class X
    {
    protected:
        unsigned long x;
    public:
        X(): x() {}
        virtual ~X() {}
        virtual unsigned long  result() const { return x; }
        
        virtual void        v(unsigned long i);
        void                nv(unsigned long i);
        virtual void        ov(unsigned long i);
        void                onv(unsigned long i);
        virtual void        iv(unsigned long i);
        void                inv(unsigned long i);
        virtual void        cv(unsigned long i) { this->x ^= i; }
        void                cnv(unsigned long i) { this->x ^= i; }
        inline virtual void civ(unsigned long i) { this->x ^= i; }
        inline void         cinv(unsigned long i) { this->x ^= i; }
    };

    class Y: public X
    {
    public:
        Y(): X() {}

        virtual void        v(unsigned long i);
        virtual void        ov(unsigned long i);
        virtual void        iv(unsigned long i);
        virtual void        cv(unsigned long i)  { this->x ^= i; }
        inline virtual void civ(unsigned long i) { this->x ^= i; }
    };

    struct S
    {
        unsigned long x;
        S(): x() {}

        static        void s(S* s, unsigned long i);
        static        void os(S* s, unsigned long i);
        static        void is(S* s, unsigned long i);
        static        void cs(S* s, unsigned long i)  { s->x ^= i; }
        static inline void cis(S* s, unsigned long i) { s->x ^= i; }
    };

           void nm(S* s, unsigned long i);
           void onm(S* s, unsigned long i);
           void inm(S* s, unsigned long i);
    inline void cinm(S* s, unsigned long i) { s->x ^= i; }
}

// ----------------------------------------------------------------------------

inline void pfc::X::iv(unsigned long i)  { this->x ^= i; }
inline void pfc::X::inv(unsigned long i) { this->x ^= i; }
inline void pfc::Y::iv(unsigned long i)  { this->x ^= i; }
inline void pfc::S::is(pfc::S* s, unsigned long i) { s->x ^= i; }
inline void pfc::inm(pfc::S* s, unsigned long i) { s->x ^= i; }

// ----------------------------------------------------------------------------

#endif
