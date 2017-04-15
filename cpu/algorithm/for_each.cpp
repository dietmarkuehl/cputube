// cpu/algorithm/for_each.cpp                                         -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#include "cpu/tube/context.hpp"

#include <algorithm>
#include <numeric>
#if 0
#include "experimental/algorithm"
#include "experimental/execution_policy"
#else
#include "algorithm"
#include "execution_policy"
#endif
#include "nstd/execution/execution.hpp"
#include "nstd/algorithm/for_each.hpp"

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
    struct std_for_each
    {
        static char const* name() { return "std::for_each()"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            std::for_each(begin, end, fun);
        }
    };
    struct pstl_for_each_seq
    {
        static char const* name() { return "PSTL::for_each(PSTL::seq)"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            PSTL::for_each(PSTL::seq, begin, end, fun);
        }
    };
    struct pstl_for_each_par
    {
        static char const* name() { return "PSTL::for_each(PSTL::par)"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            PSTL::for_each(PSTL::par, begin, end, fun);
        }
    };
    struct nstd_for_each_seq
    {
        static char const* name() { return "nstd::for_each(nstd::seq)"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            nstd::algorithm::for_each(nstd::execution::seq, begin, end, fun);
        }
    };
    struct nstd_for_each_par
    {
        static char const* name() { return "nstd::for_each(nstd::par)"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            nstd::algorithm::for_each(nstd::execution::par, begin, end, fun);
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    template <typename Competitor, typename Fun>
    void measure(cpu::tube::context&     context,
                 std::vector<int> const& from,
                 Fun                     fun,
                 Competitor const&       competitor)
    {
        std::vector<int> tmp(from);
        auto timer = context.start();
        competitor(tmp.begin(), tmp.end(), fun);
        auto time = timer.measure();

        std::ostringstream out;
        out << competitor.name() << " [" << from.size() << "]";
        context.report(out.str(), time, "<none>");
    }

    void run_tests(cpu::tube::context& context, int size) {
        std::vector<int> from;
        int value(0);
        std::generate_n(std::back_inserter(from), size,
                        [value]() mutable { return ++value; });
        auto fun = [](int& value){ value *= 17; };

        measure(context, from, fun, std_for_each());
        measure(context, from, fun, std_for_each());
        measure(context, from, fun, pstl_for_each_seq());
        measure(context, from, fun, pstl_for_each_par());
        measure(context, from, fun, nstd_for_each_seq());
        measure(context, from, fun, nstd_for_each_par());
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
