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

#include "cpu/tube/context.hpp"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <set>
#if !defined(__INTEL_COMPILER)
#include <unordered_set>
#endif
#if defined(HAS_BOOST)
#include "boost/unordered_set.hpp"
#include "boost/container/flat_set.hpp"
#endif
#include <vector>
#if !defined(__INTEL_COMPILER) && defined(HAS_GOOGLE)
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

    struct dynamic_set_find
    {
        std::set<int, std::function<bool(int, int)>> d_values;
        dynamic_set_find(std::vector<int> const& values)
            : d_values(values.begin(), values.end(),
                       [](int a, int b){ return a < b; }) {}
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

#if defined(HAS_BOOST)
    struct boost_unordered_set_find
    {
        boost::unordered_set<int> d_values;
        boost_unordered_set_find(std::vector<int> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(int value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };

    struct boost_flat_set_find
    {
        boost::container::flat_set<int> d_values;
        boost_flat_set_find(std::vector<int> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(int value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };
#endif

#if !defined(__INTEL_COMPILER) && defined(HAS_GOOGLE)
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
    void measure(cpu::tube::context&     context,
                 std::vector<int> const& sought,
                 char const*             name,
                 int                     size,
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

        auto timer = context.start();
        for (int i(0); i != 1000; ++i) {
            for (const_iterator it(sought.begin()), end(sought.end());
                 it != end; ++it) {
                if (competitor.contains(*it)) {
                    ++total;
                }
            }
        }
        auto time = timer.measure();

        std::ostringstream out;
        out << name << " [" << size << "]";
        context.report(out.str(), time, total);
    }

    void run_tests(cpu::tube::context& context, int size) {
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

        if (size <= 1000) {
            measure(context, sought, "vector find()",              size, vector_find(values));
        }
        else {
            std::ostringstream out;
            out << "vector find() [" << size << "]";
            context.stub(out.str());
        }
        measure(context, sought, "vector lower_bound()",       size, vector_lower_bound(values));
        measure(context, sought, "set find()",                 size, set_find(values));
        measure(context, sought, "dynamic set find()",         size, dynamic_set_find(values));
#if !defined(__INTEL_COMPILER)
        measure(context, sought, "unordered set find()",       size, unordered_set_find(values));
#else
        {
            std::ostringstream out;
            out << "unordered set find() [" << size << "]";
            context.stub(out.str());
        }
#endif
#if defined(HAS_BOOST)
        measure(context, sought, "boost unordered set find()", size, boost_unordered_set_find(values));
        measure(context, sought, "boost flat set find()", size, boost_flat_set_find(values));
#endif
#if !defined(__INTEL_COMPILER) && defined(HAS_GOOGLE)
        measure(context, sought, "b-tree set find()",          size, btree_set_find(values));
#else
        {
            std::ostringstream out;
            out << "b-tree set find() [" << size << "]";
            context.stub(out.str());
        }
#endif
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
        for (int i(10); i <= 10000; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(context, i * j);
            }
        }
    }
}

