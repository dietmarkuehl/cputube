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

// ----------------------------------------------------------------------------

namespace
{
    template <typename Function>
    void measure(cpu::tube::context& context,
                 char const*         name,
                 Function            function)
    {
        auto timer = context.start();
        auto result = function();
        context.report(name, timer, result);
    }

    std::auto_ptr<pfc::X> get_ptr(bool use_x)
    {
        return std::auto_ptr<pfc::X>(use_x? new pfc::X: new pfc::Y);
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    unsigned int n(2 <= ac? atol(av[1]): 10000000u);
    bool use_x(ac == 3); // make the choice of pointer depend on a parameter

    measure(context, "base", [=]()->unsigned int{ unsigned int x(0); for (unsigned int i(0); i != n; ++i) { x ^= i; } return x; });

    measure(context, "x.v(i)",    [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.v(i);    } return x.result(); });
    measure(context, "x.nv(i)",   [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.nv(i);   } return x.result(); });
    measure(context, "x.ov(i)",   [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.ov(i);   } return x.result(); });
    measure(context, "x.onv(i)",  [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.onv(i);  } return x.result(); });
    measure(context, "x.iv(i)",   [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.iv(i);   } return x.result(); });
    measure(context, "x.inv(i)",  [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.inv(i);  } return x.result(); });
    measure(context, "x.cv(i)",   [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.cv(i);   } return x.result(); });
    measure(context, "x.cnv(i)",  [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.cnv(i);  } return x.result(); });
    measure(context, "x.civ(i)",  [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.civ(i);  } return x.result(); });
    measure(context, "x.cinv(i)", [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { x.cinv(i); } return x.result(); });

    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->v(i)",    [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->v(i);    } return xp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->nv(i)",   [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->nv(i);   } return xp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->ov(i)",   [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->ov(i);   } return xp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->onv(i)",  [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->onv(i);  } return xp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->iv(i)",   [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->iv(i);   } return xp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->inv(i)",  [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->inv(i);  } return xp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->cv(i)",   [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->cv(i);   } return xp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->cnv(i)",  [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->cnv(i);  } return xp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->civ(i)",  [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->civ(i);  } return xp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(use_x)); pfc::X* xp(ap.get()); measure(context, "xp->cinv(i)", [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { xp->cinv(i); } return xp->result(); }); }

    measure(context, "y.v(i)",    [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.v(i);    } return y.result(); });
    measure(context, "y.nv(i)",   [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.nv(i);   } return y.result(); });
    measure(context, "y.ov(i)",   [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.ov(i);   } return y.result(); });
    measure(context, "y.onv(i)",  [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.onv(i);  } return y.result(); });
    measure(context, "y.iv(i)",   [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.iv(i);   } return y.result(); });
    measure(context, "y.inv(i)",  [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.inv(i);  } return y.result(); });
    measure(context, "y.cv(i)",   [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.cv(i);   } return y.result(); });
    measure(context, "y.cnv(i)",  [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.cnv(i);  } return y.result(); });
    measure(context, "y.civ(i)",  [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.civ(i);  } return y.result(); });
    measure(context, "y.cinv(i)", [=]()->unsigned int{ pfc::Y y; for (unsigned int i(0); i != n; ++i) { y.cinv(i); } return y.result(); });

    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->v(i)",    [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->v(i);    } return yp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->nv(i)",   [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->nv(i);   } return yp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->ov(i)",   [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->ov(i);   } return yp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->onv(i)",  [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->onv(i);  } return yp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->iv(i)",   [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->iv(i);   } return yp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->inv(i)",  [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->inv(i);  } return yp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->cv(i)",   [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->cv(i);   } return yp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->cnv(i)",  [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->cnv(i);  } return yp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->civ(i)",  [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->civ(i);  } return yp->result(); }); }
    { std::auto_ptr<pfc::X> ap(get_ptr(!use_x)); pfc::X* yp(ap.get()); measure(context, "yp->cinv(i)", [=]()->unsigned int{ for (unsigned int i(0); i != n; ++i) { yp->cinv(i); } return yp->result(); }); }

    measure(context, "S::s(&o, i)",    [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pfc::S::s(&o, i);    } return o.x; });
    measure(context, "S::os(&o, i)",   [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pfc::S::os(&o, i);   } return o.x; });
    measure(context, "S::is(&o, i)",   [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pfc::S::is(&o, i);   } return o.x; });
    measure(context, "S::cs(&o, i)",   [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pfc::S::cs(&o, i);   } return o.x; });
    measure(context, "S::cis(&o, i)",  [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pfc::S::cis(&o, i);  } return o.x; });
    measure(context, "nm(&o, i)",      [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pfc::nm(&o, i);      } return o.x; });
    measure(context, "onm(&o, i)",     [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pfc::onm(&o, i);     } return o.x; });
    measure(context, "inm(&o, i)",     [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pfc::inm(&o, i);     } return o.x; });
    measure(context, "cinm(&o, i)",    [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pfc::cinm(&o, i);    } return o.x; });

    { void (pfc::X::*pv)(unsigned int)    = &pfc::X::v;    measure(context, "(x.*pv)(i)",      [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { (x.*pv)(i);    } return x.result(); }); }
    { void (pfc::X::*pnv)(unsigned int)   = &pfc::X::nv;   measure(context, "(x.*pnv)(i)",     [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { (x.*pnv)(i);   } return x.result(); }); }
    { void (pfc::X::*pov)(unsigned int)   = &pfc::X::ov;   measure(context, "(x.*pov)(i)",     [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { (x.*pov)(i);   } return x.result(); }); }
    { void (pfc::X::*ponv)(unsigned int)  = &pfc::X::onv;  measure(context, "(x.*ponv)(i)",    [=]()->unsigned int{ pfc::X x; for (unsigned int i(0); i != n; ++i) { (x.*ponv)(i);  } return x.result(); }); }

    { void (*ps)(pfc::S*, unsigned int)  = &pfc::S::s;  measure(context, "ps(&o, i)",    [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { ps(&o, i);    } return o.x; }); }
    { void (*pos)(pfc::S*, unsigned int) = &pfc::S::os; measure(context, "pos(&o, i)",    [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pos(&o, i);    } return o.x; }); }
    { void (*pis)(pfc::S*, unsigned int) = &pfc::S::is; measure(context, "pis(&o, i)",    [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pis(&o, i);    } return o.x; }); }
    { void (*pnm)(pfc::S*, unsigned int) = &pfc::nm; measure(context, "pnm(&o, i)",    [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pnm(&o, i);    } return o.x; }); }
    { void (*ponm)(pfc::S*, unsigned int) = &pfc::onm; measure(context, "ponm(&o, i)",    [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { ponm(&o, i);    } return o.x; }); }
    { void (*pinm)(pfc::S*, unsigned int) = &pfc::inm; measure(context, "pinm(&o, i)",    [=]()->unsigned int{ pfc::S o; for (unsigned int i(0); i != n; ++i) { pinm(&o, i);    } return o.x; }); }
}

// ----------------------------------------------------------------------------
// Putting the implementations late used to prevent compilers from inlining
// the function. That doesn't seem to be the case anymore.

void pfc::X::v(unsigned int i)  { this->x ^= i; }
void pfc::X::nv(unsigned int i) { this->x ^= i; }
void pfc::Y::v(unsigned int i)  { this->x ^= i; }
void pfc::S::s(pfc::S* s, unsigned int i) { s->x ^= i; }
void pfc::nm(pfc::S* s, unsigned int i) { s->x ^= i; }
