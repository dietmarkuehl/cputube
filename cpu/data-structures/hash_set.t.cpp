// data-structures/hash_set.t.cpp                                     -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2018 Dietmar Kuehl http://www.dietmar-kuehl.de         
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

#include "hash_set.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cstdlib>

namespace DS = cpu::data_structures;

// ----------------------------------------------------------------------------

static std::pair<char const*, bool(*)()> tests[] = {
    { "initial state", []{
            DS::hash_set<std::string> container;
            return container.empty()
                && 0u == container.size()
                && container.begin() == container.end()
                ;
        }
    },
    {
        "find on empty container yields end()", []{
            DS::hash_set<std::string> container;
            return container.find("foo") == container.end()
                ;
        }
    },
    {
        "emplace a single element", []{
            DS::hash_set<std::string> container;
            std::string               key{"foo"};
            auto rc = container.emplace(key);
            return !container.empty()
                && 1u == container.size()
                && 1  == std::distance(container.begin(), container.end())
                && rc.second 
                && container.begin() == rc.first 
                && key == *rc.first
                && container.find(key) == rc.first
                && container.find("bar") == container.end()
                ;
        }
    },
    {
        "emplace single element again", []{
            DS::hash_set<std::string> container;
            std::string               key{"foo"};
            auto rc1 = container.emplace(key);
            auto rc2 = container.emplace(key);
            return 1u == container.size() 
                && 1 == std::distance(container.begin(), container.end())
                && !rc2.second
                && rc1.first == rc2.first
                ;
        }
    },
    {
        "emplace a few elements", []{
            std::string keys[] = { "abc", "def", "ghi", "jkl", "mno", "pqr" };
            bool        success{true};

            DS::hash_set<std::string> container;
            for (auto const& key: keys) {
                success = success && container.emplace(key).second;
            }
            for (auto const& key: keys) {
                success = success && !container.emplace(key).second;
            }
            std::vector<std::string> content(container.begin(), container.end());
            std::sort(content.begin(), content.end());
            return success
                && std::equal(std::begin(keys), std::end(keys), content.begin(), content.end())
                ;
        }
    },
    {
        "emplace a lots of elements", []{
            char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            std::vector<std::string> keys;
            for (char c0: chars) {
                for (char c1: chars) {
                    if (c0 && c1) {
                        keys.push_back(std::string("base") + c0 + c1);
                    }
                }
            }
            bool        success{true};

            DS::hash_set<std::string> container;
            for (auto const& key: keys) {
                success = success && container.emplace(key).second;
            }
            for (auto const& key: keys) {
                success = success && !container.emplace(key).second;
            }
            std::vector<std::string> content(container.begin(), container.end());
            std::sort(keys.begin(), keys.end());
            std::sort(content.begin(), content.end());
            return success
                && std::equal(std::begin(keys), std::end(keys), content.begin(), content.end())
                ;
        }
    }
};

// ----------------------------------------------------------------------------

static bool run_test(std::pair<char const*, bool(*)()> test) {
    static char const* const fail{"\x1b[31mFAIL\x1b[0m: "};
    bool rc{false};
    try {
        rc = test.second();
        std::cout << (rc? "PASS: ": fail) << test.first << "\n";
    }
    catch (std::exception const& ex) {
        std::cout << "ERROR: " << test.first << " caught exception: "
                  << ex.what() << "\n";
    }
    catch (...) {
        std::cout << "ERROR: " << test.first << " caught unknown exception\n";
    }
    return rc;
}

// ----------------------------------------------------------------------------

int main()
{
    int rc = EXIT_SUCCESS;
    for (auto test: tests) {
        if (!run_test(test)) {
            rc = EXIT_FAILURE;
        }
    }
    return rc;
}
