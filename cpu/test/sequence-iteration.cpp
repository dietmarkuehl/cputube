// cpu/test/sequence-iteration.cpp                                    -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2013 Dietmar Kuehl http://www.dietmar-kuehl.de         
//                                                                       
//  Permission is hereby granted, free of charge, to any person          
//  obtaining a copy of this software and associated documentation       
//  files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify,        
//  merge, publish, distribute, sublicense, and/or sell copies of        
//  the Software, and to permit persons to whom the Software is          
//  furnished to do so, subject to the following conditions:             
//                                                                       
//  The above copyright notice and this permission notice shall be       
//  included in all copies or substantial portions of the Software.      
//                                                                       
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
//  OTHER DEALINGS IN THE SOFTWARE. 
// ----------------------------------------------------------------------------

#include "cpu/tube/context.hpp"

#include <algorithm>
#if !defined(__INTEL_COMPILER)
#include <array>
#endif
#include <deque>
#if !defined(__INTEL_COMPILER)
#include <forward_list>
#endif
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <set>
#if !defined(__INTEL_COMPILER)
#include <unordered_set>
#endif
#include <vector>
#if !defined(__INTEL_COMPILER)
#include "google/cpp-btree/btree_set.h"
#endif

// ----------------------------------------------------------------------------

#if defined(__INTEL_COMPILER)
namespace std
{
    template <typename T, int Size>
    T* begin(T (&array)[Size]) {
        return array;
    }
    template <typename T, int Size>
    T* end(T (&array)[Size]) {
        return array + Size;
    }
}
#endif

// ----------------------------------------------------------------------------

namespace test
{
    template <typename Cont, typename T>
    Cont initialize(T const* begin, T const* end) {
        return Cont(begin, end);
    }

#if !defined(__INTEL_COMPILER)
    template <>
    std::array<int, 1023> initialize<std::array<int, 1023>, int>(int const* begin,
                                                                 int const* end) {
        std::array<int, 1023> rc;
        std::copy(begin, end, rc.begin());
        return rc;
    }
    template <>
    std::array<float, 1023> initialize<std::array<float, 1023>, float>(float const* begin,
                                                                       float const* end) {
        std::array<float, 1023> rc;
        std::copy(begin, end, rc.begin());
        return rc;
    }
    template <>
    std::array<double, 1023> initialize<std::array<double, 1023>, double>(double const* begin,
                                                                          double const* end) {
        std::array<double, 1023> rc;
        std::copy(begin, end, rc.begin());
        return rc;
    }

    template <>
    std::array<int, 511> initialize<std::array<int, 511>, int>(int const* begin,
                                                               int const* end) {
        std::array<int, 511> rc;
        std::copy(begin, end, rc.begin());
        return rc;
    }
    template <>
    std::array<long long, 511> initialize<std::array<long long, 511>, long long>(long long const* begin,
                                                               long long const* end) {
        std::array<long long, 511> rc;
        std::copy(begin, end, rc.begin());
        return rc;
    }
    template <>
    std::array<float, 511> initialize<std::array<float, 511>, float>(float const* begin,
                                                                     float const* end) {
        std::array<float, 511> rc;
        std::copy(begin, end, rc.begin());
        return rc;
    }
    template <>
    std::array<double, 511> initialize<std::array<double, 511>, double>(double const* begin,
                                                                        double const* end) {
        std::array<double, 511> rc;
        std::copy(begin, end, rc.begin());
        return rc;
    }
#endif

    template <typename Cont>
    void measure(cpu::tube::context& context, char const* name) {
        typename Cont::value_type array[511];
        for (int i(0); i != std::end(array) - std::begin(array); ++i) {
            array[i] = i;
        }

        Cont cont = test::initialize<Cont>(std::begin(array), std::end(array));
        long tmp = std::accumulate(cont.begin(), cont.end(), 0);
        tmp += std::accumulate(cont.begin(), cont.end(), 0);

        long total(0);
        
        auto timer = context.start();
        for (int i(0); i != 100000; ++i) {
            total += std::accumulate(cont.begin(), cont.end(), 0);
        }
        context.report(name, timer, total);
    }
}

// ----------------------------------------------------------------------------

namespace test
{
    template <typename T>
    struct array_iterator
    {
        T const* p;
        array_iterator(T const* p): p(p) {}
       
        T const& operator*() const { return *this->p; }
        array_iterator& operator++()    { ++this->p; return *this; }
        array_iterator  operator++(int) {
            array_iterator rc(*this);
            this->operator++();
            return rc;
        }

        bool operator== (array_iterator<T> const& other) const {
            return this->p == other.p;
        }
        bool operator!= (array_iterator<T> const& other) const {
            return !(*this == other);
        }
    };

    template <typename T, int Size>
    struct array
    {
        typedef T value_type;
        T values[Size]; 
        template <typename InIt>
        array(InIt it, InIt end) { std::copy(it, end, this->values); }
        test::array_iterator<T> begin() const { return this->values; }
        test::array_iterator<T> end() const   { return this->values + Size; }
    };
}

// ----------------------------------------------------------------------------

namespace test
{
    template <typename T>
    void measure_type(cpu::tube::context& context, char const* type)
    {
        std::cout << type << '\n';

#if !defined(__INTEL_COMPILER)
        test::measure<std::array<T, 511> >(context, "std::array<T  511>");
#else
        context.stub("std::array<T  511>");
#endif
        test::measure<std::deque<T> >(context, "std::deque<T>");
#if !defined(__INTEL_COMPILER)
        test::measure<std::forward_list<T> >(context, "std::forward_list<T>");
#else
        context.stub("std::forward_list<T>");
#endif
        test::measure<std::list<T> >(context, "std::list<T>");
        test::measure<std::set<T> >(context, "std::set<T>");
#if !defined(__INTEL_COMPILER)
        test::measure<std::unordered_set<T> >(context, "std::unordered_set<T>");
#else
        context.stub("std::unordered_set<T>");
#endif
        test::measure<std::vector<T> >(context, "std::vector<T>");
        test::measure<test::array<T, 511> >(context, "test::array<T  511>");
#if !defined(__INTEL_COMPILER)
        test::measure<btree::btree_set<T> >(context, "btree::btree_set<T>");
#else
        context.stub("btree::btree_set<T>");
#endif
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    test::measure_type<int>(context, "int");
    test::measure_type<long long>(context, "long long");
    test::measure_type<float>(context, "float");
    test::measure_type<double>(context, "double");
}
