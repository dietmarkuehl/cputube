// cpu/algorithm/reduce.cpp                                           -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#include "cpu/tube/context.hpp"

#include <algorithm>
#include <functional>
#include <numeric>
#include <nstd/execution/execution.hpp>
#include <nstd/algorithm/reduce.hpp>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include "experimental/algorithm"
//#include "experimental/execution_policy"

#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdlib.h>

// namespace PSTL = std::experimental::parallel::v1;
// namespace PSTL = std::experimental::parallel;
namespace PSTL = std;

// ----------------------------------------------------------------------------

namespace
{
    struct std_accumulate
    {
        static char const* name() { return "std::accumulate()"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T init, Op op) const {
            return std::accumulate(begin, end, init, op);
        }
    };
#ifdef HAS_STD_REDUCE
    struct std_reduce
    {
        static char const* name() { return "std::reduce()"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T init, Op op) const {
            return std::reduce(begin, end, init, op);
        }
    };
#endif
    struct loop_reduce
    {
        static char const* name() { return "loop reduce"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt it, InIt end, T value, Op op) const {
            for (; it != end; ++it) {
                value = op(value, *it);
            }
            return value;
        }
    };
#ifdef HAS_PSTL_REDUCE
    struct pstl_reduce_seq
    {
        static char const* name() { return "PSTL::reduce(PSTL::seq)"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T init, Op op) const {
            return std::reduce(PSTL::seq, begin, end, init, op);
        }
    };
    struct pstl_reduce_par
    {
        static char const* name() { return "PSTL::reduce(PSTL::par)"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T init, Op op) const {
            return std::reduce(PSTL::par, begin, end, init, op);
        }
    };
#endif
    struct nstd_reduce_seq
    {
        static char const* name() { return "nstd::reduce(nstd::seq)"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T init, Op op) const {
            return nstd::algorithm::reduce(nstd::execution::seq, begin, end, init, op);
        }
    };
    struct nstd_reduce_par
    {
        static char const* name() { return "nstd::reduce(nstd::par)"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T init, Op op) const {
            return nstd::algorithm::reduce(nstd::execution::par, begin, end, init, op);
        }
    };
    struct nstd_reduce_par_unseq
    {
        static char const* name() { return "nstd::reduce(nstd::par_unseq)"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T init, Op op) const {
            return nstd::algorithm::reduce(nstd::execution::par_unseq, begin, end, init, op);
        }
    };
    struct nstd_reduce_tbb
    {
        static char const* name() { return "nstd::reduce(nstd::par_unseq)"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T init, Op op) const {
            return nstd::algorithm::reduce(nstd::execution::tbb, begin, end, init, op);
        }
    };
    struct omp_reduce
    {
        static char const* name() { return "OpenMp reduce"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T value, Op) const {
            // this is cheating: not using the reduction operation!
            #pragma omp parallel for default(shared) reduction(+:value)
            for (InIt it = begin; it < end; ++it) {
                value += *it;
            }
            return value;
        }
    };
    struct tbb_reduce
    {
        static char const* name() { return "tbb::parallel_reduce()"; }
        template <typename InIt, typename T, typename Op>
        T operator()(InIt begin, InIt end, T init, Op op) const {
            return tbb::parallel_reduce(tbb::blocked_range<int>(0, end - begin),
                                        init,
                                        [=](tbb::blocked_range<int> const& range,
                                            auto const& value) {
                                            return std::accumulate(begin + range.begin(),
                                                                   begin + range.end(),
                                                                   value, op);
                                        }, op);
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    template <typename Competitor, typename T, typename Op>
    void measure(cpu::tube::context&        context,
                 std::vector<double> const& range,
                 T                          init,
                 Op                         op,
                 Competitor const&          competitor)
    {
        auto timer = context.start();
        auto result = competitor(range.begin(), range.end(), init, op);
        auto time = timer.measure();

        std::ostringstream out;
        out << competitor.name() << " [" << range.size() << "]";
        context.report(out.str(), time, result);
    }

    template <typename T, typename Op>
    void run_tests(cpu::tube::context& context, int size, T init, Op op) {
        std::vector<double> range;
        int value(0);
        std::generate_n(std::back_inserter(range), size,
                        [value, size]() mutable {
                            return 2.5 * ++value / size - 0.5;
                        });

        measure(context, range, init, op, std_accumulate());
#ifdef HAS_STD_REDUCE
        measure(context, range, init, op, std_reduce());
#endif
        measure(context, range, init, op, loop_reduce());
#ifdef HAS_PSTL_REDUCE
        measure(context, range, init, op, pstl_reduce_seq());
        measure(context, range, init, op, pstl_reduce_par());
#endif
        measure(context, range, init, op, nstd_reduce_seq());
        measure(context, range, init, op, nstd_reduce_par());
        measure(context, range, init, op, nstd_reduce_par_unseq());
        measure(context, range, init, op, nstd_reduce_tbb());
        measure(context, range, init, op, omp_reduce());
        measure(context, range, init, op, tbb_reduce());
    }
}

// ----------------------------------------------------------------------------

template <typename T, typename Op>
void run_test_driver(cpu::tube::context& context, int size, T init, Op op)
{
    if (size) {
        run_tests(context, size, init, op);
    }
    else {
        int offset(10000);
        for (int i(10); i <= 10000; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(context, offset * i * j, init, op);
            }
        }
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    int size(ac == 1? 0: atoi(av[1]));
    // run_test_driver(context, size, [](int size, int value){ return value /= 17; });
    run_test_driver(context, size, double(), std::plus<>());
}
