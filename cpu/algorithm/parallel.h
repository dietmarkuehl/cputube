// cpu/algorithm/parallel.h                                           -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#ifndef INCLUDED_CPU_ALGORITHM_PARALLEL
#define INCLUDED_CPU_ALGORITHM_PARALLEL

#include <algorithm>
#include <execution_policy>
#include <iostream>
#include <memory>
#include <numeric>

// ----------------------------------------------------------------------------

namespace std {
    void adjacent_difference();
    void adjacent_find();
    void all_of();
    void any_of();
    void copy();
    void copy_if();
    void copy_n();
    void count();
    void count_if();
    void destroy();
    void destroy_n();
    void equal();
    void exclusive_scan();
    void fill();
    void fill_n();
    void find();
    void find_end();
    void find_first_of();
    void find_if();
    void find_if_not();
    void for_each();
    void for_each_n();
    void generate();
    void generate_n();
    void includes();
    void inclusive_scan();
    void inner_product();
    void inplace_merge();
    void is_heap();
    void is_heap_until();
    void is_partitioned();
    void is_sorted();
    void is_sorted_until();
    void lexicographical_compare();
    void max_element();
    void merge();
    void min_element();
    void minmax_element();
    void mismatch();
    void move();
    void none_of();
    void nth_element();
    void partial_sort();
    void partial_sort_copy();
    void partition();
    void partition_copy();
    void reduce();
    void remove();
    void remove_copy();
    void remove_copy_if();
    void remove_if();
    void replace();
    void replace_copy();
    void replace_copy_if();
    void replace_if();
    void reverse();
    void reverse_copy();
    void rotate();
    void rotate_copy();
    void search();
    void search_n();
    void set_difference();
    void set_intersection();
    void set_symmetric_difference();
    void set_union();
    void sort();
    void stable_partition();
    void stable_sort();
    void swap_ranges();
    void transform();
    void transform_exclusive_scan();
    void transform_inclusive_scan();
    void transform_reduce();
    void uninitialized_copy();
    void uninitialized_copy_n();
    void uninitialized_default_construct();
    void uninitialized_default_construct_n();
    void uninitialized_fill();
    void uninitialized_fill_n();
    void uninitialized_move();
    void uninitialized_move_n();
    void uninitialized_value_construct();
    void uninitialized_value_construct_n();
    void unique();
    void unique_copy();
}

namespace PSTL = std;

// ----------------------------------------------------------------------------

namespace cpu {
    namespace execution {
        template <typename T>
        class is_execution_policy
            : public PSTL::is_execution_policy<T> {
        };
        template <typename T>
        constexpr bool is_execution_policy_v = is_execution_policy<T>::value;

        using sequenced_policy = PSTL::sequential_execution_policy;
        using PSTL::seq;
        using parallel_policy = PSTL::parallel_execution_policy;
        using PSTL::par;
#if 0
        using parallel_unsequence_policy = PSTL::parallel_unsequenced_policy;
        using PSTL::par_unseq;
#else
        class parallel_unsequenced_policy {
        };

        constexpr parallel_unsequenced_policy par_unseq;
        template <>
        class is_execution_policy<cpu::execution::parallel_unsequenced_policy>
            : public std::true_type {
        };
#endif

        template <typename>
        inline char const* policy_name() {
            return "";
        }
        template <>
        char const* policy_name<sequenced_policy>() {
            return "seq, ";
        }
        template <>
        char const* policy_name<parallel_policy>() {
            return "par, ";
        }
        template <>
        char const* policy_name<parallel_unsequenced_policy>() {
            return "par_unseq, ";
        }
        template <typename F, typename... T>
        char const* policy_name(F, T...) {
            return policy_name<F>();
        }
    }
}

// ----------------------------------------------------------------------------

namespace cpu {
    namespace algorithm {
        inline
        namespace parallel {
            // ----------------------------------------------------------------
            // utility to make determination of the correct result easier
            template <typename F, typename S>
            auto get_real_argument(F f, S s) {
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    (void)f;
                    return s;
                }
                else {
                    (void)s;
                    return f;
                }
            }
            // ----------------------------------------------------------------
            template <typename F, typename S>
            auto get_real_value(F, S) {
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return typename S::value_type();
                }
                else {
                    return typename F::value_type();
                }
            }
            // ----------------------------------------------------------------
        }
        namespace parallel {
            // <algorithm>
            // ----------------------------------------------------------------
            using std::all_of;
            template <typename F, typename... T>
            bool all_of(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "all_of(" << name << "fb, fe, pred)\n";
                return 0;
            }
            // ----------------------------------------------------------------
            using std::any_of;
            template <typename F, typename... T>
            bool any_of(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "any_of(" << name << "fb, fe, pred)\n";
                return 0;
            }
            // ----------------------------------------------------------------
            using std::none_of;
            template <typename F, typename... T>
            bool none_of(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "none_of(" << name << "fb, fe, pred)\n";
                return 0;
            }
            // ----------------------------------------------------------------
            using std::for_each;
            template <typename F, typename... T>
            void for_each(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "for_each(" << name << "fb, fe, fun)\n";
            }
            // ----------------------------------------------------------------
            using std::for_each_n;
            template <typename F, typename S, typename... T>
            auto for_each_n(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "for_each_n(" << name << "fb, fe, count, fun)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::find;
            template <typename F, typename S, typename... T>
            auto find(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "find(" << name << "fb, fe, value)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::find_if;
            template <typename F, typename S, typename... T>
                auto find_if(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "find_if(" << name << "fb, fe, pred)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::find_if_not;
            template <typename F, typename S, typename... T>
            auto find_if_not(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "find_if_not(" << name << "fb, fe, pred)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::find_end;
            template <typename F, typename S, typename... T>
            auto find_end(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "find_end(" << name << "fb, fe, sb, se)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::find_first_of;
            template <typename F, typename S, typename... T>
            auto find_first_of(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "find_first_of(" << name << "fb, fe, sb, se)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::adjacent_find;
            template <typename F, typename S, typename... T>
            auto adjacent_find(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "adjacent_find(" << name << "fb, fe, sb, se)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::count;
            template <typename F, typename... T>
            std::ptrdiff_t count(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "count(" << name << "fb, fe, value)\n";
                return std::ptrdiff_t();
            }
            // ----------------------------------------------------------------
            using std::count_if;
            template <typename F, typename... T>
            std::ptrdiff_t count_if(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "count_if(" << name << "fb, fe, pred)\n";
                return std::ptrdiff_t();
            }
            // ----------------------------------------------------------------
            // using std::mismatch;
            template <typename F, typename S, typename T, typename... U>
            auto mismatch(F f, S s, T t, U...) {
                char const* name = cpu::execution::policy_name(f, s, t);
                std::cout << "mismatch(" << name << "fb, fe, sb, se)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return std::make_pair(s, t);
                }
                else {
                    return std::make_pair(f, s);
                }
            }
            // ----------------------------------------------------------------
            //-dk:TODO using std::equal;
            template <typename F, typename... T>
            bool equal(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "equal(" << name << "fb, fe, fun)\n";
                return false;
            }
            // ----------------------------------------------------------------
            using std::search;
            template <typename F, typename S, typename... T>
            auto search(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "search(" << name << "fb, fe, sb, se)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::search_n;
            template <typename F, typename S, typename... T>
            auto search_n(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "search_n(" << name << "fb, fe, n, value)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::copy;
            template <typename F, typename S, typename... T>
            auto copy(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "copy(" << name << "fb, fe, sb)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::copy_n;
            template <typename F, typename S, typename... T>
            auto copy_n(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "copy_n(" << name << "fb, n, sb)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::copy_if;
            template <typename F, typename S, typename... T>
            auto copy_if(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "copy_if(" << name << "fb, fe, sb, pred)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::move;
            template <typename F, typename S, typename... T>
            auto move(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "move(" << name << "fb, fe, sb)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::swap_ranges;
            template <typename F, typename S, typename... T>
            auto swap_ranges(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "swap_ranges(" << name << "fb, fe, sb, se)\n";
                //-dk:TODO: actually returns an iterator to the second range
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::transform;
            template <typename F, typename S, typename... T>
            auto transform(F f, S s, T... ) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "transform(" << name << "fb, fe, tb, fun)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::replace;
            template <typename F, typename... T>
            void replace(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "replace(" << name << "fb, fe, 0, 1)\n";
            }
            // ----------------------------------------------------------------
            using std::replace_if;
            template <typename F, typename... T>
            void replace_if(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "replace_if(" << name << "fb, fe, pred, 1)\n";
            }
            // ----------------------------------------------------------------
            using std::replace_copy;
            template <typename F, typename S, typename... T>
            auto replace_copy(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "replace_copy(" << name << "fb, fe, sb, 0, 1)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::replace_copy_if;
            template <typename F, typename S, typename... T>
            auto replace_copy_if(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "replace_copy_if(" << name << "fb, fe, sb, pred, 1)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::fill;
            template <typename F, typename... T>
            void fill(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "fill(" << name << "fb, fe, value)\n";
            }
            // ----------------------------------------------------------------
            using std::fill_n;
            template <typename F, typename S, typename... T>
            auto fill_n(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "fill_n(" << name << "fb, n, value)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::generate;
            template <typename F, typename... T>
            void generate(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "generate(" << name << "fb, fe, fun)\n";
            }
            // ----------------------------------------------------------------
            using std::generate_n;
            template <typename F, typename S, typename... T>
            auto generate_n(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "generate_n(" << name << "fb, n, fun)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::remove;
            template <typename F, typename S, typename... T>
            auto remove(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "remove(" << name << "fb, fe, value)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::remove_if;
            template <typename F, typename S, typename... T>
            auto remove_if(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "remove_if(" << name << "fb, fe, pred)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::remove_copy;
            template <typename F, typename S, typename... T>
            auto remove_copy(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "remove_copy(" << name << "fb, fe, sb, value)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::remove_copy_if;
            template <typename F, typename S, typename... T>
            auto remove_copy_if(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "remove_copy_if(" << name << "fb, fe, sb, pred)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::unique;
            template <typename F, typename S, typename... T>
            auto unique(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "unique(" << name << "fb, fe)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::unique_copy;
            template <typename F, typename S, typename... T>
            auto unique_copy(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f, s);
                std::cout << "unique_copy(" << name << "fb, fe, sb)\n";
                if constexpr (cpu::execution::is_execution_policy_v<F>) {
                    return s;
                }
                else {
                    return f;
                }
            }
            // ----------------------------------------------------------------
            using std::reverse;
            template <typename F, typename... T>
            void reverse(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "reverse(" << name << "fb, fe)\n";
            }
            // ----------------------------------------------------------------
            using std::reverse_copy;
            template <typename F, typename S, typename... T>
            auto reverse_copy(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "reverse_copy(" << name << "fb, fe, sb)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::rotate;
            template <typename F, typename S, typename... T>
            auto rotate(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "rotate(" << name << "fb, mid, fe)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::rotate_copy;
            template <typename F, typename S, typename... T>
            auto rotate_copy(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "rotate_copy(" << name << "fb, mid, fe, sb)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::is_partitioned;
            template <typename F, typename... T>
            bool is_partitioned(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "is_partitioned(" << name << "fb, fe, pred)\n";
                return false;
            }
            // ----------------------------------------------------------------
            using std::partition;
            template <typename F, typename S, typename... T>
            auto partition(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "partition(" << name << "fb, fe, pred)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::stable_partition;
            template <typename F, typename S, typename... T>
            auto stable_partition(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "stable_partition(" << name << "fb, fe, pred)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::partition_copy;
            template <typename F, typename S, typename... T>
            auto partition_copy(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "partition_copy(" << name << "fb, fe, sb, tb, pred)\n";
                auto r = get_real_argument(f, s);
                return std::make_pair(r, r);
            }
            // ----------------------------------------------------------------
            using std::sort;
            template <typename F, typename... T>
            void sort(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "sort(" << name << "fb, fe)\n";
            }
            // ----------------------------------------------------------------
            using std::stable_sort;
            template <typename F, typename... T>
            void stable_sort(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "stable_sort(" << name << "fb, fe)\n";
            }
            // ----------------------------------------------------------------
            using std::partial_sort;
            template <typename F, typename... T>
            void partial_sort(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "partial_sort(" << name << "fb, fe)\n";
            }
            // ----------------------------------------------------------------
            using std::partial_sort_copy;
            template <typename F, typename S,typename... T>
            auto partial_sort_copy(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "partial_sort_copy(" << name << "fb, fe, rb, re)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::is_sorted;
            template <typename F, typename... T>
            bool is_sorted(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "is_sorted(" << name << "fb, fe)\n";
                return true;
            }
            // ----------------------------------------------------------------
            using std::is_sorted_until;
            template <typename F, typename S, typename... T>
            auto is_sorted_until(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "is_sorted_until(" << name << "fb, fe)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::nth_element;
            template <typename F, typename... T>
            void nth_element(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "nth_element(" << name << "fb, mid, fe)\n";
            }
            // ----------------------------------------------------------------
            using std::merge;
            template <typename F, typename S, typename... T>
            auto merge(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "merge(" << name << "fb, fe, sb, se, tb)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::inplace_merge;
            template <typename F, typename... T>
            void inplace_merge(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "inplace_merge(" << name << "fb, fe, sb, se)\n";
            }
            // ----------------------------------------------------------------
            using std::includes;
            template <typename F, typename... T>
            bool includes(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "includes(" << name << "fb, fe, sb, se)\n";
                return true;
            }
            // ----------------------------------------------------------------
            using std::set_union;
            template <typename F, typename S, typename... T>
            auto set_union(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "set_union(" << name << "fb, fe, sb, se)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::set_intersection;
            template <typename F, typename S, typename... T>
            auto set_intersection(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "set_intersection(" << name << "fb, fe, sb, se)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::set_difference;
            template <typename F, typename S, typename... T>
            auto set_difference(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "set_difference(" << name << "fb, fe, sb, se)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::set_symmetric_difference;
            template <typename F, typename S, typename... T>
            auto set_symmetric_difference(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "set_symmetric_difference(" << name << "fb, fe, sb, se)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::is_heap;
            template <typename F, typename... T>
            bool is_heap(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "is_heap(" << name << "fb, fe)\n";
                return true;
            }
            // ----------------------------------------------------------------
            using std::is_heap_until;
            template <typename F, typename S, typename... T>
            auto is_heap_until(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "is_heap_until(" << name << "fb, fe)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::min_element;
            template <typename F, typename S, typename... T>
            auto min_element(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "min_element(" << name << "fb, fe)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::max_element;
            template <typename F, typename S, typename... T>
            auto max_element(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "max_element(" << name << "fb, fe)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::minmax_element;
            template <typename F, typename S, typename... T>
            auto minmax_element(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "minmax_element(" << name << "fb, fe)\n";
                auto r = get_real_argument(f, s);
                return std::make_pair(r, r);
            }
            // ----------------------------------------------------------------
            using std::lexicographical_compare;
            template <typename F, typename... T>
            bool lexicographical_compare(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "lexicographical_compare(" << name << "fb, fe, sb, se)\n";
                return true;
            }
            // ----------------------------------------------------------------
        }
        namespace parallel {
            // <numeric>
            // ----------------------------------------------------------------
            using std::reduce;
            template <typename F, typename S, typename... T>
            auto reduce(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "reduce(" << name << "fb, fe)\n";
                return get_real_value(f, s);
            }
            // ----------------------------------------------------------------
            using std::transform_reduce;
            template <typename F, typename S, typename... T>
            auto transform_reduce(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "transform_reduce(" << name << "fb, fe)\n";
                return get_real_value(f, s);
            }
            // ----------------------------------------------------------------
            using std::inner_product;
            template <typename F, typename S, typename... T>
            auto inner_product(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "inner_product(" << name << "fb, fe, sb, init)\n";
                return get_real_value(f, s);
            }
            // ----------------------------------------------------------------
            using std::exclusive_scan;
            template <typename F, typename S, typename... T>
            auto exclusive_scan(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "exclusive_scan(" << name << "fb, mid fe, sb)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::inclusive_scan;
            template <typename F, typename S, typename... T>
            auto inclusive_scan(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "inclusive_scan(" << name << "fb, mid fe, sb)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::transform_exclusive_scan;
            template <typename F, typename S, typename... T>
            auto transform_exclusive_scan(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "transform_exclusive_scan(" << name << "fb, mid fe, sb)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::transform_inclusive_scan;
            template <typename F, typename S, typename... T>
            auto transform_inclusive_scan(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "transform_inclusive_scan(" << name << "fb, mid fe, sb)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
            using std::adjacent_difference;
            template <typename F, typename S, typename... T>
            auto adjacent_difference(F f, S s, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "adjacent_difference(" << name << "fb, fe, sb)\n";
                return get_real_argument(f, s);
            }
            // ----------------------------------------------------------------
        }
        namespace parallel {
            // <memory>
            // ----------------------------------------------------------------
            using std::uninitialized_default_construct;
            template <typename F, typename... T>
            void uninitialized_default_construct(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_default_construct(" << name << "fb, fe)\n";
            }
            // ----------------------------------------------------------------
            using std::uninitialized_default_construct_n;
            template <typename F, typename... T>
            void uninitialized_default_construct_n(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_default_construct_n(" << name << "fb, n)\n";
            }
            // ----------------------------------------------------------------
            using std::uninitialized_value_construct;
            template <typename F, typename... T>
            void uninitialized_value_construct(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_value_construct(" << name << "fb, fe)\n";
            }
            // ----------------------------------------------------------------
            using std::uninitialized_value_construct_n;
            template <typename F, typename... T>
            void uninitialized_value_construct_n(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_value_construct_n(" << name << "fb, n)\n";
            }
            // ----------------------------------------------------------------
            using std::uninitialized_copy;
            template <typename F, typename... T>
            void uninitialized_copy(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_copy(" << name << "fb, fe, sb)\n";
            }
            // ----------------------------------------------------------------
            using std::uninitialized_copy_n;
            template <typename F, typename... T>
            void uninitialized_copy_n(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_copy_n(" << name << "fb, n, sb)\n";
            }
            // ----------------------------------------------------------------
            using std::uninitialized_move;
            template <typename F, typename... T>
            void uninitialized_move(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_move(" << name << "fb, fe, sb)\n";
            }
            // ----------------------------------------------------------------
            using std::uninitialized_move_n;
            template <typename F, typename... T>
            void uninitialized_move_n(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_move_n(" << name << "fb, n, sb)\n";
            }
            // ----------------------------------------------------------------
            using std::uninitialized_fill;
            template <typename F, typename... T>
            void uninitialized_fill(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_fill(" << name << "fb, fe, v)\n";
            }
            // ----------------------------------------------------------------
            using std::uninitialized_fill_n;
            template <typename F, typename... T>
            void uninitialized_fill_n(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "uninitialized_fill_n(" << name << "fb, n, v)\n";
            }
            // ----------------------------------------------------------------
            using std::destroy;
            template <typename F, typename... T>
            void destroy(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "destroy(" << name << "fb, fe)\n";
            }
            // ----------------------------------------------------------------
            using std::destroy_n;
            template <typename F, typename... T>
            void destroy_n(F f, T...) {
                char const* name = cpu::execution::policy_name(f);
                std::cout << "destroy_n(" << name << "fb, n)\n";
            }
            // ----------------------------------------------------------------
        }
    }
}

// ----------------------------------------------------------------------------

#endif
