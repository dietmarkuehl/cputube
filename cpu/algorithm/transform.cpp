// cpu/algorithm/transform.cpp                                        -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#include "cpu/tube/context.hpp"

#include <algorithm>
#include <numeric>
#include <complex>
#include <tbb/parallel_for.h>
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
    struct std_transform
    {
        static char const* name() { return "std::transform()"; }
        template <typename InIt, typename OutIt, typename Fun>
        OutIt operator()(InIt begin, InIt end, OutIt to, Fun fun) const {
            return std::transform(begin, end, to, fun);
        }
    };
    struct pstl_transform_seq
    {
        static char const* name() { return "PSTL::transform(PSTL::seq)"; }
        template <typename InIt, typename OutIt, typename Fun>
        OutIt operator()(InIt begin, InIt end, OutIt to, Fun fun) const {
            return std::transform(PSTL::seq, begin, end, to, fun);
        }
    };
    struct pstl_transform_par
    {
        static char const* name() { return "PSTL::transform(PSTL::par)"; }
        template <typename InIt, typename OutIt, typename Fun>
        OutIt operator()(InIt begin, InIt end, OutIt to, Fun fun) const {
            return std::transform(PSTL::par, begin, end, to, fun);
        }
    };
    struct tbb_transform
    {
        static char const* name() { return "tbb-based transform()"; }
        template <typename InIt, typename OutIt, typename Fun>
        OutIt operator()(InIt begin, InIt end, OutIt to, Fun fun) const {
            tbb::parallel_for(tbb::blocked_range<int>(0, end - begin),
                              [=](tbb::blocked_range<int>const& r){
                                  for (auto it(r.begin()); it != r.end(); ++it) {
                                      to[it] = fun(begin[it]);
                                  }});
            return to + (end - begin);
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    template <typename Competitor, typename Fun>
    void measure(cpu::tube::context&     context,
                 std::vector<int> const& from,
                 std::vector<int>&       to,
                 Fun                     fun,
                 Competitor const&       competitor)
    {
        auto timer = context.start();
        competitor(from.begin(), from.end(), to.begin(), fun);
        auto time = timer.measure();

        std::ostringstream out;
        out << competitor.name() << " [" << from.size() << "]";
        context.report(out.str(), time, "<none>");
    }

    template <typename Fun>
    void run_tests(cpu::tube::context& context, int size, Fun fun) {
        std::vector<int> from;
        std::vector<int> to(size);
        int value(0);
        std::generate_n(std::back_inserter(from), size,
                        [value]() mutable { return ++value; });

        measure(context, from, to, fun, std_transform());
        measure(context, from, to, fun, std_transform());
        measure(context, from, to, fun, pstl_transform_seq());
        measure(context, from, to, fun, pstl_transform_par());
        measure(context, from, to, fun, tbb_transform());
    }
}

// ----------------------------------------------------------------------------

template <typename Fun>
void run_test_driver(cpu::tube::context& context, int size, Fun fun)
{
    if (size) {
        run_tests(context, size,
                  [fun, size](int value){ return fun(size, value); });
    }
    else {
        int offset(10000);
        for (int i(10); i <= 10000; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(context, offset * i * j,
                          [fun, size = offset * i * j](int value){ return fun(size, value); });
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
    run_test_driver(context, size, [](int size, int value){
            constexpr int max(2000);
            std::complex<double> p(2.5 * value / size - 0.5, 0.001);
            int count(0);
            for (std::complex<double> v(p); norm(v) < 4.0 && count != max; ++count) {
                v = v * v - p;
            }
            return count;
        });
}
