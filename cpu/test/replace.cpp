#include "cpu/tube/context.hpp"
#include <algorithm>
#include <fstream>
#include <iterator>
#include <numeric>
#include <regex>
#include <string>
#include <unordered_set>
#include <cctype>
#include <vector>

// ----------------------------------------------------------------------------

namespace
{
    struct use_regex_build
    {
        std::string allowed;
        use_regex_build(std::string const& allowed): allowed(allowed) {}
        void operator()(std::string& text) const {
            std::regex filter("[^" + allowed + "]");
            text = std::regex_replace(text, filter, "");
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_regex_prebuild
    {
        std::regex filter;
        use_regex_prebuild(std::string const& allowed)
            : filter("[^" + allowed + "]") {
        }
        void operator()(std::string& text) const {
            text = std::regex_replace(text, this->filter, "");
        }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct use_remove_if_str_find
    {
        std::string allowed;
        use_remove_if_str_find(std::string const& allowed)
            : allowed(allowed) {
        }
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
        use_remove_if_find(std::string const& allowed)
            : allowed(allowed) {
            std::sort(this->allowed.begin(), this->allowed.end());
        }
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
        use_remove_if_binary_search(std::string const& allowed)
            : allowed(allowed) {
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
        use_remove_if_hash(std::string const& allowed)
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
    struct use_remove_if_ctype
    {
        std::unordered_set<char> allowed;
        use_remove_if_ctype(std::string const& allowed)
            : allowed(allowed.begin(), allowed.end()) {
        }
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(), [&](unsigned char c) {
                        return !(std::isalnum(c) || c == '_' || c == '-');
                    }),
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
        use_remove_if_table(std::string const& allowed)
            : filter(std::numeric_limits<unsigned char>::max(), 1) {
            for (unsigned char c: allowed) {
                filter[c] = 0;
            }
        }
        void operator()(std::string& text) const {
            text.erase(std::remove_if(text.begin(), text.end(), [&](unsigned char c) {
                        return filter[c];
                    }),
                text.end());
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
    std::string allowed("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-");

    test::measure(context, "regex (build)",    text, use_regex_build(allowed));
    test::measure(context, "regex (prebuild)", text, use_regex_prebuild(allowed));
    test::measure(context, "use_remove_if_str_find", text, use_remove_if_str_find(allowed));
    test::measure(context, "use_remove_if_find", text, use_remove_if_find(allowed));
    test::measure(context, "use_remove_if_binary_search", text, use_remove_if_binary_search(allowed));
    test::measure(context, "use_remove_if_ctype", text, use_remove_if_ctype(allowed));
    test::measure(context, "use_remove_if_hash", text, use_remove_if_hash(allowed));
    test::measure(context, "use_remove_if_table", text, use_remove_if_table(allowed));
}
