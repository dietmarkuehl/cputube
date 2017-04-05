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

    call<void>([](auto... a){ return CA::fill(a...); },
                   v0.begin(), v0.end(), 0);
    call<void>([](auto... a){ return CA::fill_n(a...); }, //-dk:TODO fix return type
               v0.begin(), 0, 0);
    call<void>([](auto... a){ return CA::generate(a...); },
               v0.begin(), v0.end(), [](){ return 0; });
    call<void>([](auto... a){ return CA::generate_n(a...); }, //-dk:TODO fix return type
               v0.begin(), 0, [](){ return 0; });

    call<iterator>([](auto... a){ return CA::remove(a...); },
                   v0.begin(), v0.end(), 0);
    call<iterator>([](auto... a){ return CA::remove_if(a...); },
                   v0.begin(), v0.end(), [](auto){ return true; });
    call<iterator>([](auto... a){ return CA::remove_copy(a...); },
                   v0.begin(), v0.end(), v1.begin(), 0);
    call<iterator>([](auto... a){ return CA::remove_copy_if(a...); },
                   v0.begin(), v0.end(), v1.begin(), [](auto){ return true; });

    call<iterator>([](auto... a){ return CA::unique(a...); },
                   v0.begin(), v0.end());
    call<iterator>([](auto... a){ return CA::unique(a...); },
                   v0.begin(), v0.end(), [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::unique_copy(a...); },
                   v0.begin(), v0.end(), v1.begin());
    call<iterator>([](auto... a){ return CA::unique_copy(a...); },
                   v0.begin(), v0.end(), v1.begin(), [](auto, auto){ return true; });

    call<void>([](auto... a){ return CA::reverse(a...); },
                   v0.begin(), v0.end());
    call<iterator>([](auto... a){ return CA::reverse_copy(a...); },
               v0.begin(), v0.end(), v1.begin());

    call<iterator>([](auto... a){ return CA::rotate(a...); },
                   v0.begin(), v0.begin(), v0.end());
    call<iterator>([](auto... a){ return CA::rotate_copy(a...); },
                   v0.begin(), v0.begin(), v0.end(), v1.begin());

    call<bool>([](auto... a){ return CA::is_partitioned(a...); },
               v0.begin(), v0.begin(), [](auto){ return true; });
    call<iterator>([](auto... a){ return CA::partition(a...); },
                   v0.begin(), v0.end(), [](auto){ return true; });
    call<iterator>([](auto... a){ return CA::stable_partition(a...); },
                   v0.begin(), v0.end(), [](auto){ return true; });
    call<std::pair<iterator, iterator>>([](auto... a){ return CA::partition_copy(a...); },
                                        v0.begin(), v0.end(), v1.begin(), v2.begin(),
                                        [](auto){ return true; });

    call<void>([](auto... a){ return CA::sort(a...); },
               v0.begin(), v0.end());
    call<void>([](auto... a){ return CA::sort(a...); },
               v0.begin(), v0.end(), [](auto, auto){ return true; });
    call<void>([](auto... a){ return CA::stable_sort(a...); },
               v0.begin(), v0.end());
    call<void>([](auto... a){ return CA::stable_sort(a...); },
               v0.begin(), v0.end(), [](auto, auto){ return true; });
    call<void>([](auto... a){ return CA::partial_sort(a...); },
               v0.begin(), v0.begin(), v0.end());
    call<void>([](auto... a){ return CA::partial_sort(a...); },
               v0.begin(), v0.begin(), v0.end(), [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::partial_sort_copy(a...); },
                   v0.begin(), v0.begin(), v1.begin(), v2.begin());
    call<iterator>([](auto... a){ return CA::partial_sort_copy(a...); },
                   v0.begin(), v0.begin(), v1.begin(), v2.begin(),
                   [](auto, auto){ return true; });
    call<bool>([](auto... a){ return CA::is_sorted(a...); },
               v0.begin(), v0.begin());
    call<bool>([](auto... a){ return CA::is_sorted(a...); },
               v0.begin(), v0.begin(), [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::is_sorted_until(a...); },
               v0.begin(), v0.begin());
    call<iterator>([](auto... a){ return CA::is_sorted_until(a...); },
               v0.begin(), v0.begin(), [](auto, auto){ return true; });

    call<void>([](auto... a){ return CA::nth_element(a...); },
               v0.begin(), v0.begin(), v0.end());
    call<void>([](auto... a){ return CA::nth_element(a...); },
               v0.begin(), v0.begin(), v0.end(), [](auto, auto){ return true; });

    call<iterator>([](auto... a){ return CA::merge(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(),
                   v2.begin());
    call<iterator>([](auto... a){ return CA::merge(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(),
                   v2.begin(), [](auto, auto){ return true; });

    call<void>([](auto... a){ return CA::inplace_merge(a...); },
               v0.begin(), v0.begin(), v0.end());
    call<void>([](auto... a){ return CA::inplace_merge(a...); },
               v0.begin(), v0.begin(), v0.end(), [](auto, auto){ return true; });

    call<bool>([](auto... a){ return CA::includes(a...); },
               v0.begin(), v0.end(), v1.begin(), v1.end());
    call<bool>([](auto... a){ return CA::includes(a...); },
               v0.begin(), v0.end(), v1.begin(), v1.end(), [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::set_union(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), v2.begin());
    call<iterator>([](auto... a){ return CA::set_union(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), v2.begin(),
                   [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::set_intersection(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), v2.begin());
    call<iterator>([](auto... a){ return CA::set_intersection(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), v2.begin(),
                   [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::set_difference(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), v2.begin());
    call<iterator>([](auto... a){ return CA::set_difference(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), v2.begin(),
                   [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::set_symmetric_difference(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), v2.begin());
    call<iterator>([](auto... a){ return CA::set_symmetric_difference(a...); },
                   v0.begin(), v0.end(), v1.begin(), v1.end(), v2.begin(),
                   [](auto, auto){ return true; });

    call<bool>([](auto... a){ return CA::is_heap(a...); },
               v0.begin(), v0.end());
    call<bool>([](auto... a){ return CA::is_heap(a...); },
               v0.begin(), v0.end(), [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::is_heap_until(a...); },
               v0.begin(), v0.end());
    call<iterator>([](auto... a){ return CA::is_heap_until(a...); },
               v0.begin(), v0.end(), [](auto, auto){ return true; });

    call<iterator>([](auto... a){ return CA::min_element(a...); },
               v0.begin(), v0.end());
    call<iterator>([](auto... a){ return CA::min_element(a...); },
               v0.begin(), v0.end(), [](auto, auto){ return true; });
    call<iterator>([](auto... a){ return CA::max_element(a...); },
               v0.begin(), v0.end());
    call<iterator>([](auto... a){ return CA::max_element(a...); },
               v0.begin(), v0.end(), [](auto, auto){ return true; });
    call<std::pair<iterator, iterator>>([](auto... a){ return CA::minmax_element(a...); },
                                        v0.begin(), v0.end());
    call<std::pair<iterator, iterator>>([](auto... a){ return CA::minmax_element(a...); },
                                        v0.begin(), v0.end(), [](auto, auto){ return true; });

    call<bool>([](auto... a){ return CA::lexicographical_compare(a...); },
               v0.begin(), v0.end(), v1.begin(), v1.end());
    call<bool>([](auto... a){ return CA::lexicographical_compare(a...); },
               v0.begin(), v0.end(), v1.begin(), v1.end(), [](auto, auto){ return true; });

    // ------------------------------------------------------------------------
    // <numeric>

    call<int>([](auto... a){ return CA::reduce(a...); },
              v0.begin(), v0.end());
    call<int>([](auto... a){ return CA::reduce(a...); },
              v0.begin(), v0.end(), 0);
    call<int>([](auto... a){ return CA::reduce(a...); },
              v0.begin(), v0.end(), 0, [](auto, auto){ return 0; });
    call<int>([](auto... a){ return CA::transform_reduce(a...); },
              v0.begin(), v0.end(), [](auto v){ return v; },
              0, [](auto, auto){ return 0; });

    call<int>([](auto... a){ return CA::inner_product(a...); },
              v0.begin(), v0.end(), v1.begin(), 0);
    call<int>([](auto... a){ return CA::inner_product(a...); },
              v0.begin(), v0.end(), v1.begin(), 0,
              [](auto, auto){ return 0; }, [](auto, auto){ return 0; });

    call<iterator>([](auto...a){ return CA::exclusive_scan(a...); },
                   v0.begin(), v0.end(), v1.begin(), 0);
    call<iterator>([](auto...a){ return CA::exclusive_scan(a...); },
                   v0.begin(), v0.end(), v1.begin(), 0, [](auto, auto){ return 0; });
    call<iterator>([](auto...a){ return CA::inclusive_scan(a...); },
                   v0.begin(), v0.end(), v1.begin());
    call<iterator>([](auto...a){ return CA::inclusive_scan(a...); },
                   v0.begin(), v0.end(), v1.begin(), [](auto, auto){ return 0; });
    call<iterator>([](auto...a){ return CA::inclusive_scan(a...); },
                   v0.begin(), v0.end(), v1.begin(), [](auto, auto){ return 0; }, 0);
    call<iterator>([](auto...a){ return CA::transform_exclusive_scan(a...); },
                   v0.begin(), v0.end(), v1.begin(),
                   [](auto){ return 0; }, 0, [](auto, auto){ return 0; });
    call<iterator>([](auto...a){ return CA::transform_inclusive_scan(a...); },
                   v0.begin(), v0.end(), v1.begin(),
                   [](auto){ return 0; }, [](auto, auto){ return 0; });
    call<iterator>([](auto...a){ return CA::transform_inclusive_scan(a...); },
                   v0.begin(), v0.end(), v1.begin(),
                   [](auto){ return 0; }, [](auto, auto){ return 0; }, 0);

    call<iterator>([](auto... a){ return CA::adjacent_difference(a...); },
                   v0.begin(), v0.end(), v1.begin());
    call<iterator>([](auto... a){ return CA::adjacent_difference(a...); },
                   v0.begin(), v0.end(), v1.begin(), [](auto, auto){ return 0; });
    // ------------------------------------------------------------------------
    // <memory>
    call<void>([](auto... a){ return CA::uninitialized_default_construct(a...); },
               v0.begin(), v0.begin());
    call<void>([](auto... a){ return CA::uninitialized_default_construct_n(a...); },
               v0.begin(), 0);
    call<void>([](auto... a){ return CA::uninitialized_value_construct(a...); },
               v0.begin(), v0.begin());
    call<void>([](auto... a){ return CA::uninitialized_value_construct_n(a...); },
               v0.begin(), 0);
    call<void>([](auto... a){ return CA::uninitialized_copy(a...); },
               v0.begin(), v0.begin(), v1.begin());
    call<void>([](auto... a){ return CA::uninitialized_copy_n(a...); },
               v0.begin(), 0, v1.begin());
    call<void>([](auto... a){ return CA::uninitialized_move(a...); },
               v0.begin(), v0.begin(), v1.begin());
    call<void>([](auto... a){ return CA::uninitialized_move_n(a...); },
               v0.begin(), 0, v1.begin());
    call<void>([](auto... a){ return CA::uninitialized_fill(a...); },
               v0.begin(), v0.begin(), 0);
    call<void>([](auto... a){ return CA::uninitialized_fill_n(a...); },
               v0.begin(), 0, 0);
    call<void>([](auto... a){ return CA::destroy(a...); },
               v0.begin(), v0.begin());
    call<void>([](auto... a){ return CA::destroy_n(a...); },
               v0.begin(), 0);

    std::cout << "done\n";
}
