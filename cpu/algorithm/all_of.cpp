// cpu/test/search-integer.cpp                                        -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#include "cpu/tube/context.hpp"

#include <algorithm>
#include <numeric>
#include "experimental/algorithm"
#include "experimental/execution_policy"

#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdlib.h>

namespace PSTL = std::experimental::parallel::v1;

// ----------------------------------------------------------------------------

namespace
{
    struct std_all_of
    {
        static char const* name() { return "std::all_of()"; }
        template <typename InIt, typename Predicate>
        bool operator()(InIt begin, InIt end, Predicate predicate) const {
            return std::all_of(begin, end, predicate);
        }
    };
    struct pstl_all_of_seq
    {
        static char const* name() { return "PSTL::all_of(PSTL::seq)"; }
        template <typename InIt, typename Predicate>
        bool operator()(InIt begin, InIt end, Predicate predicate) const {
            return PSTL::all_of(PSTL::seq, begin, end, predicate);
        }
    };
    struct pstl_all_of_par
    {
        static char const* name() { return "PSTL::all_of(PSTL::par)"; }
        template <typename InIt, typename Predicate>
        bool operator()(InIt begin, InIt end, Predicate predicate) const {
            return PSTL::all_of(PSTL::par, begin, end, predicate);
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    template <typename Competitor>
    void measure(cpu::tube::context&     context,
                 int                     limit,
                 std::vector<int> const& values,
                 Competitor const&       competitor)
    {
        auto timer = context.start();
        bool result = competitor(values.begin(), values.end(),
                                 [limit](int value){ return value < limit; });
        auto time = timer.measure();

        std::ostringstream out;
        out << competitor.name() << " [" << values.size() << "]";
        context.report(out.str(), time, result);
    }

    void run_tests(cpu::tube::context& context, int size) {
        std::vector<int> values;
        int value{};
        std::generate_n(std::back_inserter(values), size,
                        [value]() mutable { return ++value; });

        measure(context, size + 1, values, std_all_of());
        measure(context, size + 1, values, std_all_of());
        measure(context, size + 1, values, pstl_all_of_seq());
        measure(context, size + 1, values, pstl_all_of_par());
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
        int offset(1000);
        for (int i(10); i <= 10000; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(context, offset * i * j);
            }
        }
    }
}
