// cpu/algorithm/sort.cpp                                             -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#include "cpu/tube/context.hpp"

#include <algorithm>
#include <numeric>
#include <complex>
#include <random>
#ifdef HAS_PSTL
#include "experimental/algorithm"
#include "experimental/execution_policy"
namespace PSTL = std::experimental::parallel;
#endif
#ifdef HAS_SYCLSTL
#include "experimental/algorithm"
#include "experimental/execution_policy"
namespace SyclSTL = std::experimental::parallel;
#endif
#ifdef HAS_NSTL
#include "algorithm"
#include "execution_policy"
namespace NSTL = std;
#endif
#include "nstd/execution/execution.hpp"
#include "nstd/algorithm/sort.hpp"
#include "tbb/parallel_sort.h"

#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdlib.h>

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/include/parallel_sort.hpp>

// ----------------------------------------------------------------------------

namespace
{
    struct std_sort
    {
        static char const* name() { return "std::sort()"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            std::sort(begin, end, comp);
        }
    };
#ifdef HAS_PSTL
    struct pstl_sort_seq
    {
        static char const* name() { return "PSTL::sort(PSTL::seq)"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            PSTL::sort(PSTL::seq, begin, end, comp);
        }
    };
    struct pstl_sort_par
    {
        static char const* name() { return "PSTL::sort(PSTL::par)"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            PSTL::sort(PSTL::par, begin, end, comp);
        }
    };
#endif
    struct nstd_sort_seq
    {
        static char const* name() { return "nstd::algorithm::sort(nstd::execution::seq)"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            nstd::algorithm::sort(nstd::execution::seq, begin, end, comp);
        }
    };
    struct nstd_sort_par
    {
        static char const* name() { return "nstd::algorithm::sort(nstd::execution::par)"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            nstd::algorithm::sort(nstd::execution::par, begin, end, comp);
        }
    };
    struct nstd_sort_tbb
    {
        static char const* name() { return "nstd::algorithm::sort(nstd::execution::tbb)"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            nstd::algorithm::sort(nstd::execution::tbb, begin, end, comp);
        }
    };
#ifdef HAS_SYCLSTL
    struct syclstl_sort_seq
    {
        static char const* name() { return "SyclSTL::sort(SyclSTL::seq)"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            SyclSTL::sort(SyclSTL::seq, begin, end, comp);
        }
    };
    struct syclstl_sort_par
    {
        static char const* name() { return "SyclSTL::sort(SyclSTL::par)"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            SyclSTL::sort(SyclSTL::par, begin, end, comp);
        }
    };
#endif
    struct tbb_sort
    {
        static char const* name() { return "tbb::parallel_sort()"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            tbb::parallel_sort(begin, end, comp);
        }
    };
    struct hpx_sort
    {
        static char const* name() { return "hpx::parallel::sort()"; }
        template <typename InIt, typename Comp>
        void operator()(InIt begin, InIt end, Comp comp) const {
            hpx::parallel::sort(hpx::parallel::execution::par, begin, end, comp);
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    template <typename Competitor, typename Comp>
    void measure(cpu::tube::context&     context,
                 std::vector<int> const& from,
                 Comp                    comp,
                 Competitor const&       competitor)
    {
        std::vector<int> tmp(from);
        auto timer = context.start();
        competitor(tmp.begin(), tmp.end(), comp);
        auto time = timer.measure();

        std::ostringstream out;
        out << competitor.name() << " [" << from.size() << "]";
        context.report(out.str(), time, "<none>");
    }

    template <typename Comp>
    void run_tests(cpu::tube::context& context, int size, Comp comp) {
        std:: minstd_rand simple_rand;
        simple_rand.seed(42);
        std::vector<int> from;
        std::generate_n(std::back_inserter(from), size, simple_rand);

        measure(context, from, comp, std_sort());
#ifdef HAS_PSTL
        measure(context, from, comp, pstl_sort_seq());
        measure(context, from, comp, pstl_sort_par());
#endif
#ifdef HAS_SYCLSTL
        measure(context, from, comp, syclstl_sort_seq());
        measure(context, from, comp, syclstl_sort_par());
#endif
        measure(context, from, comp, nstd_sort_seq());
        measure(context, from, comp, nstd_sort_par());
        measure(context, from, comp, nstd_sort_tbb());
        measure(context, from, comp, tbb_sort());
        measure(context, from, comp, hpx_sort());
    }
}

// ----------------------------------------------------------------------------

template <typename Comp>
void run(cpu::tube::context& context, int size, Comp comp) {
    if (size) {
        run_tests(context, size, comp);
    }
    else {
        int offset(1000);
        for (int i(10); i <= 10000; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(context, offset * i * j, comp);
            }
        }
    }
}

// ----------------------------------------------------------------------------

int hpx_main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    int size(ac == 1? 0: atoi(av[1]));
    auto comp = [=](auto v0, auto v1){ return v0 < v1; };
    run(context, size, comp);
    return hpx::finalize();
}

int main(int ac, char* av[])
{
    std::vector<std::string> cfg{ "hpx.os_threads=all" };
    hpx::init(ac, av, cfg);
}
