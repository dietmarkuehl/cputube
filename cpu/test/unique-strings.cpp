// cpu/test/unique-strings.cpp                                        -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2015 Dietmar Kuehl http://www.dietmar-kuehl.de         
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
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <string>
#include <random>
#include <set>
#include <unordered_set>
#include <vector>
#if defined(HAS_BSL)
#include <bsl_set.h>
#include <bsl_unordered_set.h>
#endif
#include <stdlib.h>

// ----------------------------------------------------------------------------

namespace std {
    template <typename Algo>
    void hashAppend(Algo& algo, std::string const& value) {
        algo(value.c_str(), value.size());
    }
}

// ----------------------------------------------------------------------------

namespace {
    struct std_algos {
        std::string name() const { return "std::sort()/std::unique()"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            std::vector<std::string> values(keys.begin(), keys.end());
            std::sort(values.begin(), values.end());
            auto end = std::unique(values.begin(), values.end());
            values.erase(end, values.end());
            return values.size();
        }
    };

    struct std_set {
        std::string name() const { return "std::set<std::string>"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            std::set<std::string> values(keys.begin(), keys.end());
            return values.size();
        }
    };

    struct std_insert_set {
        std::string name() const { return "std::set<std::string> (insert)"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            std::set<std::string> values;
            for (std::string value: keys) {
                values.insert(value);
            }
            return values.size();
        }
    };

    struct std_reverse_set {
        std::string name() const { return "std::set<std::string> (reverse)"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            std::set<std::string> values;
            for (std::string value: keys) {
                std::reverse(value.begin(), value.end());
                values.insert(value);
            }
            return values.size();
        }
    };

    struct std_unordered_set {
        std::string name() const { return "std::unordered_set<std::string>"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            std::unordered_set<std::string> values(keys.begin(), keys.end());
            return values.size();
        }
    };

    struct std_insert_unordered_set {
        std::string name() const { return "std::unordered_set<std::string> (insert)"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            std::unordered_set<std::string> values;
            for (std::string value: keys) {
                values.insert(value);
            }
            return values.size();
        }
    };

    struct std_reserve_unordered_set {
        std::string name() const { return "std::unordered_set<std::string> (reserve)"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            std::unordered_set<std::string> values;
            values.reserve(keys.size());
            for (std::string value: keys) {
                values.insert(value);
            }
            return values.size();
        }
    };

#if defined(HAS_BSL)
    struct bsl_set {
        std::string name() const { return "bsl::set<std::string>"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            bsl::set<std::string> values(keys.begin(), keys.end());
            return values.size();
        }
    };

    struct bsl_insert_set {
        std::string name() const { return "bsl::set<std::string> (insert)"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            bsl::set<std::string> values;
            for (std::string value: keys) {
                values.insert(value);
            }
            return values.size();
        }
    };

    struct bsl_reverse_set {
        std::string name() const { return "bsl::set<std::string> (reverse)"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            bsl::set<std::string> values;
            for (std::string value: keys) {
                std::reverse(value.begin(), value.end());
                values.insert(value);
            }
            return values.size();
        }
    };

    struct bsl_unordered_set {
        std::string name() const { return "bsl::unordered_set<std::string>"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            bsl::unordered_set<std::string> values(keys.begin(), keys.end());
            return values.size();
        }
    };

    struct bsl_insert_unordered_set {
        std::string name() const { return "bsl::unordered_set<std::string> (insert)"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            bsl::unordered_set<std::string> values;
            for (std::string value: keys) {
                values.insert(value);
            }
            return values.size();
        }
    };

    struct bsl_reserve_unordered_set {
        std::string name() const { return "bsl::unordered_set<std::string> (reserve)"; }
        std::size_t run(std::vector<std::string> const& keys) const {
            bsl::unordered_set<std::string> values;
            values.reserve(keys.size());
            for (std::string value: keys) {
                values.insert(value);
            }
            return values.size();
        }
    };
#endif
}

// ----------------------------------------------------------------------------

namespace {
    template <typename Algo>
    void measure(cpu::tube::context&             context,
                 std::vector<std::string> const& keys,
                 std::size_t                     basesize,
                 Algo                            algo)
    {
        auto timer = context.start();
        std::size_t size = algo.run(keys);
        auto time = timer.measure();
        std::ostringstream out;
        out << std::left << std::setw(50) << algo.name()
            << " [" << keys.size() << "/" << basesize << "]";
        context.report(out.str(), time, size);
    }
}

static void measure(cpu::tube::context&             context,
                    std::vector<std::string> const& keys,
                    std::size_t                     basesize)
{
    measure(context, keys, basesize, std_algos());
    measure(context, keys, basesize, std_set());
    measure(context, keys, basesize, std_insert_set());
    measure(context, keys, basesize, std_reverse_set());
    measure(context, keys, basesize, std_unordered_set());
    measure(context, keys, basesize, std_insert_unordered_set());
    measure(context, keys, basesize, std_reserve_unordered_set());
#if defined(HAS_BSL)
    measure(context, keys, basesize, bsl_set());
    measure(context, keys, basesize, bsl_insert_set());
    measure(context, keys, basesize, bsl_reverse_set());
    measure(context, keys, basesize, bsl_unordered_set());
    measure(context, keys, basesize, bsl_insert_unordered_set());
    measure(context, keys, basesize, bsl_reserve_unordered_set());
#endif
}

// ----------------------------------------------------------------------------

static void run_tests(cpu::tube::context& context, int size) {
    std::string const bases[] = {
        "/usr/local/include/",
        "/some/medium/sized/path/as/a/prefix/to/the/actual/interesting/names/",
        "/finally/a/rather/long/string/including/pointless/sequences/like/"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/"
        "just/to/make/the/string/longer/to/qualify/as/an/actual/long/string/"
    };

    std::mt19937 gen(4711);
    std::uniform_int_distribution<> rand(0, size * 0.8);

    for (std::string const& base: bases) {
        std::vector<std::string> keys;
        keys.reserve(size);
        int i = 0;
        std::generate_n(std::back_inserter(keys), size,
                        [i, &base, &rand, &gen]()mutable{
                            return base + std::to_string(rand(gen));
                        });
        measure(context, keys, base.size());
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
        for (int i(10); i <= 100000; i *= 10) {
            for (int j(1); j < 10; j *= 2) {
                run_tests(context, i * j);
            }
        }
    }
}
