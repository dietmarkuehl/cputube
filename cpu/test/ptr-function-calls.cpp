// cpu/test/ptr-function-calls.cpp                                    -*-C++-*-
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
// The test in this file are based on tests from C++ Performance
// TR18015 http://www.open-std.org/JTC1/SC22/WG21/docs/TR18015.pdf.
// This file contains specificallly tests for those from section 5.3
// (Classes and Interfaces). The tests are not identical to those
// used by the performance TR, primarily to address some slightly
// questionable pratices (e.g., not initializing variables) and to
// fit them into the commonly used framework. Also, xor instead of
// addition is used because compilers see through the use addition
// and optimize the entire loop out.
// ----------------------------------------------------------------------------

#include "ptr-function-calls.hpp"
#include "cpu/tube/context.hpp"
#include <memory>
#include <stdlib.h>

// ----------------------------------------------------------------------------

namespace
{
    template <typename Function>
    void measure(cpu::tube::context& context,
                 unsigned long       n,
                 char const*         name,
                 Function            function)
    {
        cpu::tube::timer timer = context.start();
        unsigned long result =  function(n);
        context.report(name, timer, result);
    }

    std::auto_ptr<pfc::X> get_x(bool use_x)
    {
        return std::auto_ptr<pfc::X>(use_x? new pfc::X: new pfc::Y);
    }
}

// ----------------------------------------------------------------------------

namespace
{
    unsigned long base(unsigned long n)
    {
        unsigned long x(0);
        for (unsigned long i(0); i != n; ++i) {
            x ^= i;
        }
        return x;
    }
}

// ----------------------------------------------------------------------------

namespace
{
    template <typename T>
    unsigned long object_v(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.v(i);
        }
        return x.result();
    }

    template <typename T>
    unsigned long object_nv(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.nv(i);
        }
        return x.result();
    }

    template <typename T>
    unsigned long object_ov(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.ov(i);
        }
        return x.result();
    }

    template <typename T>
    unsigned long object_onv(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.onv(i);
        }
        return x.result();
    }

    template <typename T>
    unsigned long object_iv(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.iv(i);
        }
        return x.result();
    }

    template <typename T>
    unsigned long object_inv(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.inv(i);
        }
        return x.result();
    }

    template <typename T>
    unsigned long object_cv(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.civ(i);
        }
        return x.result();
    }

    template <typename T>
    unsigned long object_cnv(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.cinv(i);
        }
        return x.result();
    }

    template <typename T>
    unsigned long object_civ(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.civ(i);
        }
        return x.result();
    }

    template <typename T>
    unsigned long object_cinv(unsigned long n)
    {
        T x;
        for (unsigned long i(0); i != n; ++i) {
            x.cinv(i);
        }
        return x.result();
    }
}

// ----------------------------------------------------------------------------

namespace
{
    class pointer_helper
    {
        pfc::X*         d_pointer;
        unsigned long (*d_fun)(unsigned long, pfc::X*);
    public:
        pointer_helper(pfc::X* pointer, unsigned long(*fun)(unsigned long, pfc::X*))
            : d_pointer(pointer)
            , d_fun(fun) {
        }
        unsigned long operator()(unsigned long n) {
            return (this->d_fun)(n, this->d_pointer);
        }
    };

    unsigned long pointer_v(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->v(i);
        }
        return xp->result();
    }

    unsigned long pointer_nv(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->nv(i);
        }
        return xp->result();
    }

    unsigned long pointer_ov(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->ov(i);
        }
        return xp->result();
    }

    unsigned long pointer_onv(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->onv(i);
        }
        return xp->result();
    }

    unsigned long pointer_iv(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->iv(i);
        }
        return xp->result();
    }

    unsigned long pointer_inv(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->inv(i);
        }
        return xp->result();
    }

    unsigned long pointer_cv(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->cv(i);
        }
        return xp->result();
    }

    unsigned long pointer_cnv(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->cnv(i);
        }
        return xp->result();
    }

    unsigned long pointer_civ(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->civ(i);
        }
        return xp->result();
    }

    unsigned long pointer_cinv(unsigned long n, pfc::X* xp)
    {
        for (unsigned long i(0); i != n; ++i) {
            xp->cinv(i);
        }
        return xp->result();
    }
}

// ----------------------------------------------------------------------------

namespace
{
    unsigned long static_s(unsigned long n)
    {
        pfc::S o;
        for (unsigned long i(0); i != n; ++i) {
            pfc::S::s(&o, i);
        }
        return o.x;
    }

    unsigned long static_os(unsigned long n)
    {
        pfc::S o;
        for (unsigned long i(0); i != n; ++i) {
            pfc::S::os(&o, i);
        }
        return o.x;
    }

    unsigned long static_is(unsigned long n)
    {
        pfc::S o;
        for (unsigned long i(0); i != n; ++i) {
            pfc::S::is(&o, i);
        }
        return o.x;
    }

    unsigned long static_cs(unsigned long n)
    {
        pfc::S o;
        for (unsigned long i(0); i != n; ++i) {
            pfc::S::cs(&o, i);
        }
        return o.x;
    }

    unsigned long static_cis(unsigned long n)
    {
        pfc::S o;
        for (unsigned long i(0); i != n; ++i) {
            pfc::S::cis(&o, i);
        }
        return o.x;
    }

    unsigned long global_nm(unsigned long n)
    {
        pfc::S o;
        for (unsigned long i(0); i != n; ++i) {
            nm(&o, i);
        }
        return o.x;
    }

    unsigned long global_onm(unsigned long n)
    {
        pfc::S o;
        for (unsigned long i(0); i != n; ++i) {
            onm(&o, i);
        }
        return o.x;
    }

    unsigned long global_inm(unsigned long n)
    {
        pfc::S o;
        for (unsigned long i(0); i != n; ++i) {
            inm(&o, i);
        }
        return o.x;
    }

    unsigned long global_cinm(unsigned long n)
    {
        pfc::S o;
        for (unsigned long i(0); i != n; ++i) {
            cinm(&o, i);
        }
        return o.x;
    }
}

// ----------------------------------------------------------------------------

namespace
{
    class member
    {
        void (pfc::X::*d_fun)(unsigned long);
    public:
        member(void (pfc::X::*fun)(unsigned long)): d_fun(fun) {}
        unsigned long operator()(unsigned long n) {
            void (pfc::X::*fun)(unsigned long) = this->d_fun;
            pfc::X x;
            for (unsigned long i(0); i != n; ++i) {
                (x.*fun)(i);
            }
            return x.result();
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    class non_member
    {
        void (*d_fun)(pfc::S*, unsigned long);
    public:
        non_member(void (fun)(pfc::S*, unsigned long)): d_fun(fun) {}
        unsigned long operator()(unsigned long n) {
            void (*fun)(pfc::S*, unsigned long) = this->d_fun;
            pfc::S x;
            for (unsigned long i(0); i != n; ++i) {
                (fun)(&x, i);
            }
            return x.x;
        }
    };
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    unsigned long n(2 <= ac? atol(av[1]): 10000000u);
    bool use_x(ac == 3); // make the choice of pointer depend on a parameter
    // There are a few more variations how to define a function but it seems
    // they don't reall matter. Set run_all to true to verify.
    bool run_all(false);
    // In theory there could be a difference between using an object of a
    // class or a derived class. It seems that isn't the case. Set use_derived
    // to true to verify.
    bool run_derived(false);

    measure(context, n, "base", base);

    measure(context, n, "x.v(i)",     &object_v<pfc::X>);
    measure(context, n, "x.nv(i)",    &object_nv<pfc::X>);
    measure(context, n, "x.ov(i)",    &object_ov<pfc::X>);
    measure(context, n, "x.onv(i)",   &object_onv<pfc::X>);
    measure(context, n, "x.iv(i)",    &object_iv<pfc::X>);
    measure(context, n, "x.inv(i)",   &object_inv<pfc::X>);
    if (run_all) {
        measure(context, n, "x.cv(i)",    &object_cv<pfc::X>);
        measure(context, n, "x.cnv(i)",   &object_cnv<pfc::X>);
        measure(context, n, "x.civ(i)",   &object_civ<pfc::X>);
        measure(context, n, "x.cinv(i)",  &object_cinv<pfc::X>);
    }

    measure(context, n, "xp->v(i)",    pointer_helper(get_x(use_x).get(), pointer_v));
    measure(context, n, "xp->nv(i)",   pointer_helper(get_x(use_x).get(), pointer_nv));
    measure(context, n, "xp->ov(i)",   pointer_helper(get_x(use_x).get(), pointer_ov));
    measure(context, n, "xp->onv(i)",  pointer_helper(get_x(use_x).get(), pointer_onv));
    measure(context, n, "xp->iv(i)",   pointer_helper(get_x(use_x).get(), pointer_iv));
    measure(context, n, "xp->inv(i)",  pointer_helper(get_x(use_x).get(), pointer_inv));
    if (run_all) {
        measure(context, n, "xp->cv(i)",   pointer_helper(get_x(use_x).get(), pointer_cv));
        measure(context, n, "xp->cnv(i)",  pointer_helper(get_x(use_x).get(), pointer_cnv));
        measure(context, n, "xp->civ(i)",  pointer_helper(get_x(use_x).get(), pointer_civ));
        measure(context, n, "xp->cinv(i)", pointer_helper(get_x(use_x).get(), pointer_cinv));
    }

    if (run_derived) {
        // The tests on Y objects should yield the same results as those on X objects.
        measure(context, n, "y.v(i)",     &object_v<pfc::Y>);
        measure(context, n, "y.nv(i)",    &object_nv<pfc::Y>);
        measure(context, n, "y.ov(i)",    &object_ov<pfc::Y>);
        measure(context, n, "y.onv(i)",   &object_onv<pfc::Y>);
        measure(context, n, "y.iv(i)",    &object_iv<pfc::Y>);
        measure(context, n, "y.inv(i)",   &object_inv<pfc::Y>);

        if (run_all) {
            measure(context, n, "y.cv(i)",    &object_cv<pfc::Y>);
            measure(context, n, "y.cnv(i)",   &object_cnv<pfc::Y>);
            measure(context, n, "y.civ(i)",   &object_civ<pfc::Y>);
            measure(context, n, "y.cinv(i)",  &object_cinv<pfc::Y>);
        }

        measure(context, n, "yp->v(i)",    pointer_helper(get_x(!use_x).get(), pointer_v));
        measure(context, n, "yp->nv(i)",   pointer_helper(get_x(!use_x).get(), pointer_nv));
        measure(context, n, "yp->ov(i)",   pointer_helper(get_x(!use_x).get(), pointer_ov));
        measure(context, n, "yp->onv(i)",  pointer_helper(get_x(!use_x).get(), pointer_onv));
        measure(context, n, "yp->iv(i)",   pointer_helper(get_x(!use_x).get(), pointer_iv));
        measure(context, n, "yp->inv(i)",  pointer_helper(get_x(!use_x).get(), pointer_inv));
        if (run_all) {
            measure(context, n, "yp->cv(i)",   pointer_helper(get_x(!use_x).get(), pointer_cv));
            measure(context, n, "yp->cnv(i)",  pointer_helper(get_x(!use_x).get(), pointer_cnv));
            measure(context, n, "yp->civ(i)",  pointer_helper(get_x(!use_x).get(), pointer_civ));
            measure(context, n, "yp->cinv(i)", pointer_helper(get_x(!use_x).get(), pointer_cinv));
        }
    }

    measure(context, n, "S::s(&o, i)",    &static_s);
    measure(context, n, "S::os(&o, i)",   &static_os);
    measure(context, n, "S::is(&o, i)",   &static_is);
    if (run_all) {
        measure(context, n, "S::cs(&o, i)",   &static_cs);
        measure(context, n, "S::cis(&o, i)",  &static_cis);
    }
    measure(context, n, "nm(&o, i)",      &global_nm);
    measure(context, n, "onm(&o, i)",     &global_onm);
    measure(context, n, "inm(&o, i)",     &global_inm);
    if (run_all) {
        measure(context, n, "cinm(&o, i)",    &global_cinm);
    }

    measure(context, n, "(x.*pv)(i)",   member(&pfc::X::v));
    measure(context, n, "(x.*pnv)(i)",  member(&pfc::X::nv));
    measure(context, n, "(x.*pov)(i)",  member(&pfc::X::ov));
    measure(context, n, "(x.*ponv)(i)", member(&pfc::X::onv));

    measure(context, n, "ps(&o, i)",   non_member(&pfc::S::s));
    measure(context, n, "pos(&o, i)",  non_member(&pfc::S::os));
    measure(context, n, "pis(&o, i)",  non_member(&pfc::S::is));
    measure(context, n, "pnm(&o, i)",  non_member(&pfc::nm));
    measure(context, n, "ponm(&o, i)", non_member(&pfc::onm));
    measure(context, n, "pinm(&o, i)", non_member(&pfc::inm));
}

// ----------------------------------------------------------------------------
// Putting the implementations late used to prevent compilers from inlining
// the function. That doesn't seem to be the case anymore.

void pfc::X::v(unsigned long i)  { this->x ^= i; }
void pfc::X::nv(unsigned long i) { this->x ^= i; }
void pfc::Y::v(unsigned long i)  { this->x ^= i; }
void pfc::S::s(pfc::S* s, unsigned long i) { s->x ^= i; }
void pfc::nm(pfc::S* s, unsigned long i) { s->x ^= i; }
