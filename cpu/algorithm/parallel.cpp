// cpu/algorithm/parallel.cpp                                         -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#include <cpu/algorithm/parallel.h>
#include <algorithm>
#include <iostream>
#include <type_traits>

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
    if constexpr (std::is_void_v<R>) {
        f(a...);
        f(CE::seq, a...);
        f(CE::par, a...);
        f(CE::par_unseq, a...);
    }
    else {
        expect<R> = f(a...);
        expect<R> = f(CE::seq, a...);
        expect<R> = f(CE::par, a...);
        expect<R> = f(CE::par_unseq, a...);
    }
}

// ----------------------------------------------------------------------------

int main() {
    std::vector<int> v0;
    std::vector<int> v1;
    std::vector<int> v2;
    using iterator = std::vector<int>::iterator;

    call<bool>([](auto... a){ return CA::all_of(a...); },
               v0.begin(), v0.end(), [](auto){ return true; });
    call<bool>([](auto... a){ return CA::any_of(a...); },
               v0.begin(), v0.end(), [](auto){ return true; });
    call<bool>([](auto... a){ return CA::none_of(a...); },
               v0.begin(), v0.end(), [](auto){ return true; });

    call<void>([](auto... a){ return CA::for_each(a...); },
               v0.begin(), v0.end(), [](auto){ return true; });
    call<iterator>([](auto... a){ return CA::for_each_n(a...); },
                   v0.begin(), v0.end(), 0, [](auto){ return true; });

    call<iterator>([](auto... a){ return CA::find(a...); },
                   v0.begin(), v0.end(), 0);
    call<iterator>([](auto... a){ return CA::find_if(a...); },
                   v0.begin(), v0.end(), [](auto){ return true; });
    call<iterator>([](auto... a){ return CA::find_if_not(a...); },
                   v0.begin(), v0.end(), [](auto){ return true; });
    call<iterator>([](auto... a){ return CA::find_end(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end());
    call<iterator>([](auto... a){ return CA::find_end(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::find_first_of(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end());
    call<iterator>([](auto... a){ return CA::find_first_of(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::adjacent_find(a...); },
                   v0.begin(), v0.end());
    call<iterator>([](auto... a){ return CA::adjacent_find(a...); },
                   v0.begin(), v0.end(), [](auto, auto){ return true; });

    call<std::ptrdiff_t>([](auto... a){ return CA::count(a...); },
                         v0.begin(), v0.end(), 0);
    call<std::ptrdiff_t>([](auto... a){ return CA::count_if(a...); },
                         v0.begin(), v0.end(), [](auto){ return true; });

    call<std::pair<iterator, iterator>>([](auto... a){ return CA::mismatch(a...); },
                                        v0.begin(), v0.end(), v1.begin());
    call<std::pair<iterator, iterator>>([](auto... a){ return CA::mismatch(a...); },
                                        v0.begin(), v0.end(), v1.begin(), v1.end());
    call<std::pair<iterator, iterator>>([](auto... a){ return CA::mismatch(a...); },
                                        v0.begin(), v0.end(), v1.begin(), v1.end(),
                                        [](auto, auto){ return true; });
    call<std::pair<iterator, iterator>>([](auto... a){ return CA::mismatch(a...); },
                                        v0.begin(), v0.end(), v1.begin(),
                                        [](auto, auto){ return true; });

    call<bool>([](auto... a){ return CA::equal(a...); },
               v0.begin(), v0.end(), v1.begin());
    call<bool>([](auto... a){ return CA::equal(a...); },
               v0.begin(), v0.end(), v1.begin(), v1.end());
    call<bool>([](auto... a){ return CA::equal(a...); },
               v0.begin(), v0.end(), v1.begin(), v1.end(),
               [](auto, auto){ return true; });
    call<bool>([](auto... a){ return CA::equal(a...); },
               v0.begin(), v0.end(), v1.begin(),
               [](auto, auto){ return true; });

    call<iterator>([](auto... a){ return CA::search(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end());
    call<iterator>([](auto... a){ return CA::search(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(),
                   [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::search_n(a...); },
                   v0.begin(), v0.end(), 0, 0);
    call<iterator>([](auto... a){ return CA::search_n(a...); },
                   v0.begin(), v0.end(), 0, 0,
                   [](auto, auto){ return true; });

    call<iterator>([](auto... a){ return CA::copy(a...); },
                   v0.begin(), v0.end(), v1.begin());
    call<iterator>([](auto... a){ return CA::copy_n(a...); },
                   v0.begin(), 0, v1.begin());
    call<iterator>([](auto... a){ return CA::copy_if(a...); },
                   v0.begin(), v0.end(), v1.begin(), [](auto){ return true; });
    call<iterator>([](auto... a){ return CA::move(a...); },
                   v0.begin(), v0.end(), v1.begin());

    call<iterator>([](auto... a){ return CA::swap_ranges(a...); },
                   v0.begin(), v0.end(), v1.begin());

    call<iterator>([](auto... a){ return CA::transform(a...); },
                   v0.begin(), v0.end(), v1.begin(), [](auto v){ return v; });
    call<iterator>([](auto... a){ return CA::transform(a...); },
                   v0.begin(), v0.end(), v1.begin(), v2.begin(),
                   [](auto v, auto){ return v; });

    call<void>([](auto... a){ return CA::replace(a...); },
               v0.begin(), v0.end(), 0, 0);
    call<void>([](auto... a){ return CA::replace_if(a...); },
               v0.begin(), v0.end(), [](auto){ return true; }, 0);
    call<iterator>([](auto... a){ return CA::replace_copy(a...); },
                   v0.begin(), v0.end(), v1.begin(), 0, 0);
    call<iterator>([](auto... a){ return CA::replace_copy_if(a...); },
                   v0.begin(), v0.end(), v1.begin(), [](auto){ return true; }, 0);

    std::cout << "done\n";
}
