// cpu/algorithm/parallel.cpp                                         -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#include <cpu/algorithm/parallel.h>
#include <algorithm>
#include <iostream>

namespace CA = cpu::algorithm;
namespace CE = cpu::execution;

// ----------------------------------------------------------------------------

static void call_transform() {
    std::vector<int> from;
    std::vector<int> to;
    using iterator = std::vector<int>::iterator;

    iterator it1 = CA::transform(from.begin(), from.end(), to.begin(),
                                [](auto v){ return v; });
    (void)it1;
    iterator it2 = CA::transform(CE::seq, from.begin(), from.end(), to.begin(),
                                 [](auto v){ return v; });
    (void)it2;
    iterator it3 = CA::transform(CE::par, from.begin(), from.end(), to.begin(),
                                 [](auto v){ return v; });
    (void)it3;
    iterator it4 = CA::transform(CE::par_unseq, from.begin(), from.end(), to.begin(),
                                 [](auto v){ return v; });
    (void)it4;
}

// ----------------------------------------------------------------------------

int main() {
    call_transform();
    std::cout << "done\n";
}
