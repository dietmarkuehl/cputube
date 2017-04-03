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
        template <typename F, typename S>
        char const* policy_name(F, S) {
            return policy_name<F>();
        }
    }
}

// ----------------------------------------------------------------------------

namespace cpu {
    namespace algorithm {
        inline
        namespace parallel {
                   
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
        }
    }
}

// ----------------------------------------------------------------------------

#endif
