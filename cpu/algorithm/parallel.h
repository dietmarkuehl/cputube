// cpu/algorithm/parallel.h                                           -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#ifndef INCLUDED_CPU_ALGORITHM_PARALLEL
#define INCLUDED_CPU_ALGORITHM_PARALLEL

#include <algorithm>
#include <execution_policy>
#include <iostream>

// ----------------------------------------------------------------------------

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

#if 0
        using PSTL::sequenced_policy;
#else
        using sequenced_policy = PSTL::sequential_execution_policy;
#endif
        using PSTL::seq;
#if 0
        using PSTL::parallel_policy;
#else
        using parallel_policy = PSTL::parallel_execution_policy;
#endif
        using PSTL::par;
#if 0
        using PSTL::par_unseq;
        using PSTL::parallel_unsequenced_policy;
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
            //-dk:TODO using std::for_each_n;
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
            //-dk:TODO using std::mismatch;
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
        }
    }
}

// ----------------------------------------------------------------------------

#endif
