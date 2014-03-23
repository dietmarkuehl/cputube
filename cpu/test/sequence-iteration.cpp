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

#include "cpu/tube/processor.hpp"
#include "cpu/tube/timer.hpp"

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
    template <typename Cont>
    Cont initialize(int const* begin, int const* end) {
        return Cont(begin, end);
    }

#if !defined(__INTEL_COMPILER)
    template <>
    std::array<int, 1023> initialize<std::array<int, 1023> >(int const* begin,
                                                             int const* end) {
        std::array<int, 1023> rc;
        std::copy(begin, end, rc.begin());
        return rc;
    }
#endif

    template <typename Cont>
    void measure(char const* name) {
        int array[1023];
        for (int i(0); i != 1023; ++i) {
            array[i] = i;
        }

        Cont cont = test::initialize<Cont>(std::begin(array), std::end(array));
        long tmp = std::accumulate(cont.begin(), cont.end(), 0);
        tmp += std::accumulate(cont.begin(), cont.end(), 0);

        long total(0);
        cpu::tube::timer timer;
        for (int i(0); i != 100000; ++i) {
            total += std::accumulate(cont.begin(), cont.end(), 0);
        }
        cpu::tube::duration duration = timer.measure();
        std::cout << std::setw(40) << name << ' '
                  << std::setw(6)  << duration << ' '
                  << std::setw(10) << total << '\n';
            
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
        T values[Size]; 
        template <typename InIt>
        array(InIt it, InIt end) { std::copy(it, end, this->values); }
        test::array_iterator<T> begin() const { return this->values; }
        test::array_iterator<T> end() const   { return this->values + Size; }
    };
}

// ----------------------------------------------------------------------------

int main()
{
    std::cout << "processor=" << cpu::tube::processor() << '\n';

#if !defined(__INTEL_COMPILER)
    test::measure<std::array<int, 1023> >("std::array<int, 1023>");
#endif
    test::measure<std::deque<int> >("std::deque<int>");
#if !defined(__INTEL_COMPILER)
    test::measure<std::forward_list<int> >("std::forward_list<int>");
#endif
    test::measure<std::list<int> >("std::list<int>");
    test::measure<std::set<int> >("std::set<int>");
#if !defined(__INTEL_COMPILER)
    test::measure<std::unordered_set<int> >("std::unordered_set<int>");
#endif
    test::measure<std::vector<int> >("std::vector<int>");
    test::measure<test::array<int, 1023> >("test::array<int, 1023>");
#if !defined(__INTEL_COMPILER)
    test::measure<btree::btree_set<int> >("btree::btree_set<int>");
#endif
}
