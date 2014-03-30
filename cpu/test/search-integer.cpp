// cpu/test/search-integer.cpp                                        -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2014 Dietmar Kuehl http://www.dietmar-kuehl.de         
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

#include "cpu/tube/timer.hpp"
#include "cpu/tube/processor.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <set>
#if !defined(__INTEL_COMPILER)
#include <unordered_set>
#endif
#include "boost/unordered_set.hpp"
#include <vector>
#if !defined(__INTEL_COMPILER)
#include "google/cpp-btree/btree_set.h"
#endif
#include <stdlib.h>

// ----------------------------------------------------------------------------

namespace
{
    struct vector_find
    {
        std::vector<int> d_values;
        vector_find(std::vector<int> const& values): d_values(values) {}
        bool contains(int value) const {
            return this->d_values.end()
                != std::find(this->d_values.begin(), this->d_values.end(),
                             value);
        }
    };

    struct vector_lower_bound
    {
        std::vector<int> d_values;
        vector_lower_bound(std::vector<int> const& values): d_values(values) {}
        bool contains(int value) const {
            std::vector<int>::const_iterator it(
                std::lower_bound(this->d_values.begin(), this->d_values.end(),
                                 value));
            return it != this->d_values.end() && *it == value;
        }
    };

    struct set_find
    {
        std::set<int> d_values;
        set_find(std::vector<int> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(int value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };

#if !defined(__INTEL_COMPILER)
    struct unordered_set_find
    {
        std::unordered_set<int> d_values;
        unordered_set_find(std::vector<int> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(int value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };
#endif

    struct boost_unordered_set_find
    {
        boost::unordered_set<int> d_values;
        boost_unordered_set_find(std::vector<int> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(int value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };

#if !defined(__INTEL_COMPILER)
    struct btree_set_find
    {
        btree::btree_set<int> d_values;
        btree_set_find(std::vector<int> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(int value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };
#endif

}

// ----------------------------------------------------------------------------

namespace
{
    template <typename Competitor>
    void measure(std::vector<int> const& sought,
                 char const*             name,
                 Competitor const&       competitor)
    {
        long total(0);
        typedef std::vector<int>::const_iterator const_iterator;
        for (const_iterator it(sought.begin()), end(sought.end());
             it != end; ++it) {
            if (competitor.contains(*it)) {
                ++total;
            }
        }

        cpu::tube::timer timer;
        for (int i(0); i != 100; ++i) {
            for (const_iterator it(sought.begin()), end(sought.end());
                 it != end; ++it) {
                if (competitor.contains(*it)) {
                    ++total;
                }
            }
        }

        cpu::tube::duration duration = timer.measure();
        std::cout << std::setw(40) << name << ' '
                  << std::setw(7)  << duration << ' '
                  << std::setw(10) << total << '\n' << std::flush;
            
    }

    void run_tests(int size) {
        std::cout << "size=" << size << '\n';
        int range(size + size / 2);
        std::vector<int> values;
        for (unsigned int usize(size); values.size() != usize; ) {
            int value(rand() % range);
            if (values.end()
                == std::find(values.begin(), values.end(), value)) {
                values.push_back(value);
            }
        }
        std::sort(values.begin(), values.end());
        std::vector<int> sought;
        while (sought.size() != 1000u) {
            sought.push_back(rand() % range);
        }

        measure(sought, "vector find()",              vector_find(values));
        measure(sought, "vector lower_bound()",       vector_lower_bound(values));
        measure(sought, "set find()",                 set_find(values));
#if !defined(__INTEL_COMPILER)
        measure(sought, "unordered set find()",       unordered_set_find(values));
#endif
        measure(sought, "boost unordered set find()", boost_unordered_set_find(values));
#if !defined(__INTEL_COMPILER)
        measure(sought, "b-tree set find()",          btree_set_find(values));
#endif
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    int size(ac == 1? 0: atoi(av[1]));
    std::cout << "processor=" << cpu::tube::processor() << '\n';
    if (size) {
        run_tests(size);
    }
    else {
        for (int i(10); i <= 100; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(i * j);
            }
        }
    }
}

