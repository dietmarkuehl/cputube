// cpu/test/search-short-string.cpp                                   -*-C++-*-
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
#include <iomanip>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <sstream>
#if !defined(__INTEL_COMPILER)
#include <unordered_set>
#endif
#include "boost/unordered_set.hpp"
#include <vector>
#if !defined(__INTEL_COMPILER)
#include "google/cpp-btree/btree_set.h"
#endif
#include <stdlib.h>

#if defined(IS_GCC)
#include <ext/vstring.h>
#endif

// ----------------------------------------------------------------------------

namespace {
#if defined(IS_GCC)
    typedef __gnu_cxx::__sso_string string_type;
#else
    typedef std::string string_type;
#endif
}

#if defined(IS_GCC)
namespace boost
{
    template <>
    class hash<string_type> {
        std::hash<string_type> d_hash;
    public:
        int operator()(string_type const& value) const {
            return this->d_hash(value);
        }
    };
}
#endif

// ----------------------------------------------------------------------------

namespace
{
    struct vector_find
    {
        std::vector<string_type> d_values;
        vector_find(std::vector<string_type> const& values): d_values(values) {}
        bool contains(string_type const& value) const {
            return this->d_values.end()
                != std::find(this->d_values.begin(), this->d_values.end(),
                             value);
        }
    };

    struct vector_lower_bound
    {
        std::vector<string_type> d_values;
        vector_lower_bound(std::vector<string_type> const& values): d_values(values) {}
        bool contains(string_type const& value) const {
            std::vector<string_type>::const_iterator it(
                std::lower_bound(this->d_values.begin(), this->d_values.end(),
                                 value));
            return it != this->d_values.end() && *it == value;
        }
    };

    struct set_find
    {
        std::set<string_type> d_values;
        set_find(std::vector<string_type> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(string_type const& value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };

#if !defined(__INTEL_COMPILER)
    struct unordered_set_find
    {
        std::unordered_set<string_type> d_values;
        unordered_set_find(std::vector<string_type> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(string_type const& value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };
#endif

    struct boost_unordered_set_find
    {
        boost::unordered_set<string_type> d_values;
        boost_unordered_set_find(std::vector<string_type> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(string_type value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };

#if !defined(__INTEL_COMPILER)
    struct btree_set_find
    {
        btree::btree_set<string_type> d_values;
        btree_set_find(std::vector<string_type> const& values)
            : d_values(values.begin(), values.end()) {}
        bool contains(string_type value) const {
            return this->d_values.find(value) != this->d_values.end();
        }
    };
#endif

}

// ----------------------------------------------------------------------------

namespace
{
    template <typename Competitor>
    void measure(cpu::tube::context&             context,
                 std::vector<string_type> const& sought,
                 char const*                     name,
                 int                             size,
                 Competitor const&               competitor)
    {
        long total(0);
        typedef std::vector<string_type>::const_iterator const_iterator;
        for (const_iterator it(sought.begin()), end(sought.end());
             it != end; ++it) {
            if (competitor.contains(*it)) {
                ++total;
            }
        }

        auto timer = context.start();
        for (int i(0); i != 100; ++i) {
            for (const_iterator it(sought.begin()), end(sought.end());
                 it != end; ++it) {
                if (competitor.contains(*it)) {
                    ++total;
                }
            }
        }

        context.report(name, timer, size, total);
    }

    void run_tests(cpu::tube::context& context, int size,
                   std::vector<string_type> const& strings)
    {
        std::cout << "size=" << size << '\n';
        int range(size + size / 2);
        std::vector<string_type> values;
        for (unsigned int usize(size); values.size() != usize; ) {
            int value(rand() % range);
            if (values.end()
                == std::find(values.begin(), values.end(), strings[value])) {
                values.push_back(strings[value]);
            }
        }
        std::sort(values.begin(), values.end());
        std::vector<string_type> sought;
        while (sought.size() != 1000u) {
            sought.push_back(strings[rand() % range]);
        }

        measure(context, sought, "vector find()",              size, vector_find(values));
        measure(context, sought, "vector lower_bound()",       size, vector_lower_bound(values));
        measure(context, sought, "set find()",                 size, set_find(values));
#if !defined(__INTEL_COMPILER)
        measure(context, sought, "unordered set find()",       size, unordered_set_find(values));
#endif
        measure(context, sought, "boost unordered set find()", size, boost_unordered_set_find(values));
#if !defined(__INTEL_COMPILER)
        measure(context, sought, "b-tree set find()",          size, btree_set_find(values));
#endif
    }

    std::vector<string_type> make_strings(unsigned int size)
    {
        string_type codes[] = {
            "BE", "DE", "DK", "FR", "GB", "JP", "NL", "NO", "SE", "US"
        };
        std::vector<string_type> rc;
        while (rc.size() < size) {
            std::ostringstream out;
            out.fill('0');
            out << codes[rand() % 10] << std::setw(9)
                << (std::rand() % 1000000000) << '0';
            string_type value(out.str().c_str());
            if (std::find(rc.begin(), rc.end(), value) == rc.end()) {
                rc.push_back(value);
            }
        }
        return rc;
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    int size(ac == 1? 0: atoi(av[1]));
    int const max = 100;
    std::vector<string_type> strings(make_strings(2 * (size? size: max * 20)));
    if (size) {
        run_tests(context, size, strings);
    }
    else {
        for (int i(10); i <= max; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(context, i * j, strings);
            }
        }
    }
}

