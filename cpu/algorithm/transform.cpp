// cpu/algorithm/transform.cpp                                        -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#include "cpu/tube/context.hpp"

#include <algorithm>
#include <numeric>
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
}

// ----------------------------------------------------------------------------

namespace
{
    template <typename Competitor>
    void measure(cpu::tube::context&     context,
                 std::vector<int> const& from,
                 std::vector<int>&       to,
                 Competitor const&       competitor)
    {
        auto timer = context.start();
        competitor(from.begin(), from.end(), to.begin(),
                   [](auto value){ return value; });
        auto time = timer.measure();

        std::ostringstream out;
        out << competitor.name() << " [" << from.size() << "]";
        context.report(out.str(), time, "<none>");
    }

    void run_tests(cpu::tube::context& context, int size) {
        std::vector<int> from;
        std::vector<int> to(size);
        int value(0);
        std::generate_n(std::back_inserter(from), size,
                        [value]() mutable { return ++value; });

        measure(context, from, to, std_transform());
        measure(context, from, to, std_transform());
        measure(context, from, to, pstl_transform_seq());
        measure(context, from, to, pstl_transform_par());
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    int size(ac == 1? 0: atoi(av[1]));
    if (size) {
        run_tests(context, size);
    }
    else {
        int offset(10000);
        for (int i(10); i <= 10000; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(context, offset * i * j);
            }
        }
    }
}
