#include "cpu/tube/context.hpp"
#include <algorithm>
#include <fstream>
#include <iterator>
#include <numeric>
#include <regex>
#include <string>
#include <unordered_set>
#include <cctype>
#include <cstring>
#include <vector>

// ----------------------------------------------------------------------------

namespace
{
    struct use_regex_build
    {
        void operator()(std::string& text) const {
#ifndef INTEL
            text = std::regex_replace(text, std::regex(R"(\D)"), "");
#endif
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_regex_prebuild
    {
#ifndef INTEL
        std::regex filter; use_regex_prebuild() : filter(R"(\D)") {}
#endif
        void operator()(std::string& text) const {
#ifndef INTEL
            text = std::regex_replace(text, this->filter, "");
#endif
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_remove_if_str_find
    {
        std::string allowed;
        use_remove_if_str_find() : allowed("0123456789") {}
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(), [&](char c) {
                        return allowed.find(c) == std::string::npos;
                    }),
                text.end());
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_remove_if_find
    {
        std::string allowed;
        use_remove_if_find() : allowed("0123456789") {}
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(), [&](char c) {
                        return std::find(allowed.begin(), allowed.end(), c) == allowed.end();
                    }),
                text.end());
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_remove_if_binary_search
    {
        std::string allowed;
        use_remove_if_binary_search() : allowed("0123456789") {
            std::sort(this->allowed.begin(), this->allowed.end());
        }
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(), [&](char c) {
                        return !std::binary_search(allowed.begin(), allowed.end(), c);
                    }),
                text.end());
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_remove_if_hash
    {
        std::unordered_set<char> allowed;
        use_remove_if_hash(std::string const& allowed = "0123456789")
            : allowed(allowed.begin(), allowed.end()) {
        }
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(), [&](char c) {
                        return !allowed.count(c);
                    }),
                text.end());
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_remove_if_compare
    {
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(),
                                      [&](char c) { return !('0' <= c && c <= '9'); }),
                text.end());
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_remove_if_ctype
    {
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(),
                                      [](unsigned char c) { return !std::isdigit(c); }),
                text.end());
        }
    };
}

namespace
{
    struct use_remove_if_locale_naive
    {
        void operator()(std::string& text) const {
            std::locale locale;
            text.erase(std::remove_if(text.begin(), text.end(),
                                      [&](char c) { return !std::isdigit(c, locale); }),
                text.end());
        }
    };
}

namespace
{
    struct use_remove_if_locale
    {
        void operator()(std::string& text) const {
            std::ctype<char> const& ctype(std::use_facet<std::ctype<char>>(std::locale()));
            text.erase(std::remove_if(text.begin(), text.end(),
                                      [&](char c) { return !ctype.is(std::ctype_base::digit, c); }),
                text.end());
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_remove_if_ctype_ptr_fun
    {
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(),
                                      std::not1(std::ptr_fun(static_cast<int(*)(int)>(&std::isdigit)))),
                       text.end());
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_remove_if_table
    {
        std::vector<char> filter;
        use_remove_if_table(std::string const& allowed = "0123456789")
            : filter(std::numeric_limits<unsigned char>::max(), 1) {
            for (unsigned char c: allowed) {
                filter[c] = 0;
            }
        }
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(),
                                      [&](unsigned char c) { return filter[c]; }),
                text.end());
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_partition
    {
        use_partition() {
        }
        void operator()(std::string& text) const {
            text.erase(std::partition(text.begin(), text.end(),
                                      [&](unsigned char c) { return std::isdigit(c); }),
                text.end());
        }
    };
}

namespace
{
    struct use_stable_partition
    {
        use_stable_partition() {
        }
        void operator()(std::string& text) const {
            text.erase(std::stable_partition(text.begin(), text.end(),
                                             [&](unsigned char c) { return std::isdigit(c); }),
                text.end());
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_sort
    {
        void operator()(std::string& text) const {
            std::sort(text.begin(), text.end());
            std::string::size_type start(text.find_first_of("0123456789"));
            if (start == text.npos) {
                text.clear();
            }
            else {
                text = text.substr(start, text.find_last_of("0123456789") - start + 1);
            }
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_copy_if
    {
        void operator()(std::string& text) const {
            std::string result;
            std::copy_if(text.begin(), 
                         text.end(), 
                         std::back_inserter(result), 
                         [](char c) { return '0' <= c && c <= '9'; });
            text.swap(result);
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_recursive
    {
        static void getDigs(const char* inp, char* dig)
        {
            if (!*inp)
                return;
            if (*inp>='0' && *inp<='9')
            {
                *dig=*inp; 
                dig++;
                *dig=0;
            }
            getDigs(inp+1,dig);
        }
        void operator()(std::string& text) const {
            if (!text.empty()) {
                getDigs(text.c_str(), &text[0]);
                text.resize(std::strlen(text.c_str()));
            }
        }
    };
}

// ----------------------------------------------------------------------------

namespace test
{
    template <typename Replace>
    void measure(cpu::tube::context& context,
                 char const* name, std::string text, Replace replace)
    {
        cpu::tube::timer timer = context.start();
        replace(text);
        cpu::tube::duration duration(timer.measure());
        unsigned long sum = std::accumulate(text.begin(), text.end(), 0ul);
        context.report(name, duration, sum);
    }
}

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));

    std::ifstream in("cpu/test/input.txt");
    std::string text((std::istreambuf_iterator<char>(in)),
                      std::istreambuf_iterator<char>());

    test::measure(context, "use_remove_if_str_find", text, use_remove_if_str_find());
    test::measure(context, "use_remove_if_find", text, use_remove_if_find());
    test::measure(context, "use_remove_if_binary_search", text, use_remove_if_binary_search());
    test::measure(context, "use_remove_if_compare", text, use_remove_if_compare());
    test::measure(context, "use_remove_if_ctype", text, use_remove_if_ctype());
    test::measure(context, "use_remove_if_ctype_ptr_fun", text, use_remove_if_ctype());
    test::measure(context, "use_remove_if_hash", text, use_remove_if_hash());
    test::measure(context, "use_remove_if_table", text, use_remove_if_table());
    test::measure(context, "use_remove_if_locale_naive", text, use_remove_if_locale_naive());
    test::measure(context, "use_remove_if_locale", text, use_remove_if_locale());
    test::measure(context, "use_partition", text, use_partition());
    test::measure(context, "use_stable_partition", text, use_stable_partition());
    test::measure(context, "use_sort", text, use_sort());
    test::measure(context, "use_copy_if", text, use_copy_if());
    test::measure(context, "use_recursive", text, use_recursive());
    test::measure(context, "regex_build",    text, use_regex_build());
    test::measure(context, "regex_prebuild", text, use_regex_prebuild());
}
