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

namespace {
    template <typename T>
    struct expecter {
        template <typename S> void operator=(S) const = delete;
        void operator=(T) const {}
    };
    template <typename T>
    constexpr expecter<T> expect;
}

// ----------------------------------------------------------------------------

template <typename R, typename F, typename... A>
static void call(F f, A... a) {
    expect<R> = f(a...);
    expect<R> = f(CE::seq, a...);
    expect<R> = f(CE::par, a...);
    expect<R> = f(CE::par_unseq, a...);
}

// ----------------------------------------------------------------------------

int main() {
    std::vector<int> v0;
    std::vector<int> v1;
    using iterator = std::vector<int>::iterator;

    call<bool>([](auto... a){ return CA::all_of(a...); },
               v0.begin(), v0.end(), [](auto){ return true; });
    call<bool>([](auto... a){ return CA::any_of(a...); },
               v0.begin(), v0.end(), [](auto){ return true; });
    call<iterator>([](auto... a){ return CA::transform(a...); },
                   v0.begin(), v0.end(), v1.begin(), [](auto v){ return v; });
    std::cout << "done\n";
}
