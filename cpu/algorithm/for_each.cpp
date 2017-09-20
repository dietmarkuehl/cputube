// cpu/algorithm/for_each.cpp                                         -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
// ----------------------------------------------------------------------------

#include "cpu/tube/context.hpp"

#include <algorithm>
#include <numeric>
#include <complex>
#include <thread>
#include <future>
#if 0
#include "experimental/algorithm"
#include "experimental/execution_policy"
#elif 0
#include "algorithm"
#include "execution_policy"
#define HAS_PSTL 1
#endif
#include "nstd/execution/execution.hpp"
#include "nstd/algorithm/for_each.hpp"
#include "tbb/parallel_for_each.h"

#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdlib.h>

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/include/parallel_for_each.hpp>

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
    struct thread_for_each
    {
        static char const* name() { return "thread for_each()"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            int hw(std::thread::hardware_concurrency());
            int size(std::distance(begin, end));
            int chunk(size / hw);
            std::vector<std::thread> threads;
            // threads.reserve(hw);
            for (int b(0); b < size; b += chunk) {
                threads.emplace_back([=](){
                        int e = std::min(size, b + chunk);
                        std::for_each(begin + b, begin + e, fun);
                    });
            }
            for (auto& t: threads) { t.join(); }
        }
    };
    struct async_for_each
    {
        static char const* name() { return "async for_each()"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            int hw(std::thread::hardware_concurrency());
            int size(std::distance(begin, end));
            int chunk(size / hw);
            std::vector<std::future<void>> futures;
            for (int b(0); b < size; b += chunk) {
                futures.emplace_back(std::async([=](){
                        int e = std::min(size, b + chunk);
                        std::for_each(begin + b, begin + e, fun);
                        }));
            }
            for (auto& f: futures) { f.get(); }
        }
    };
#ifdef HAS_PSTL
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
#endif
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
    struct nstd_for_each_omp
    {
        static char const* name() { return "nstd::for_each(nstd::omp)"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            nstd::algorithm::for_each(nstd::execution::omp, begin, end, fun);
        }
    };
    struct nstd_for_each_tbb
    {
        static char const* name() { return "nstd::for_each(nstd::tbb)"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            nstd::algorithm::for_each(nstd::execution::tbb, begin, end, fun);
        }
    };
    struct tbb_for_each
    {
        static char const* name() { return "tbb::parallel_for_each()"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            tbb::parallel_for_each(begin, end, fun);
        }
    };
    struct omp_for_each
    {
        static char const* name() { return "omp parallel for"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            #pragma omp parallel for
            // for (int i = 0; i < size; ++i) {
            for (auto it = begin; it < end; ++it) {
                fun(*it);
            }
        }
    };
    struct hpx_for_each
    {
        static char const* name() { return "hpx::parallel::for_each"; }
        template <typename InIt, typename Fun>
        void operator()(InIt begin, InIt end, Fun fun) const {
            //-dk:TODO use par
            hpx::parallel::for_each(hpx::parallel::execution::par,
                                    begin, end, fun);
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

    template <typename Fun>
    void run_tests(cpu::tube::context& context, int size, Fun fun) {
        std::vector<int> from;
        int value(0);
        std::generate_n(std::back_inserter(from), size,
                        [value]() mutable { return ++value; });

        measure(context, from, fun, std_for_each());
        measure(context, from, fun, std_for_each());
        measure(context, from, fun, thread_for_each());
        measure(context, from, fun, async_for_each());
#ifdef HAS_PSTL
        measure(context, from, fun, pstl_for_each_seq());
        measure(context, from, fun, pstl_for_each_par());
#endif
        measure(context, from, fun, nstd_for_each_seq());
        measure(context, from, fun, nstd_for_each_par());
        measure(context, from, fun, nstd_for_each_omp());
        measure(context, from, fun, nstd_for_each_tbb());
        measure(context, from, fun, tbb_for_each());
        measure(context, from, fun, omp_for_each());
        measure(context, from, fun, hpx_for_each());
    }
}

// ----------------------------------------------------------------------------

template <typename Fun>
void run(cpu::tube::context& context, int size, Fun fun) {
    if (size) {
        run_tests(context, size, fun);
    }
    else {
        int offset(10000);
        for (int i(10); i <= 10000; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(context, offset * i * j, fun);
            }
        }
    }
}

// ----------------------------------------------------------------------------

int hpx_main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    int size(ac == 1? 0: atoi(av[1]));
    auto fun = [](int& value){ value *= 17; };
    run(context, size, fun);
    return hpx::finalize();
}

int main(int ac, char* av[])
{
    std::vector<std::string> cfg{ "hpx.os_threads=all" };
    hpx::init(ac, av, cfg);
}
