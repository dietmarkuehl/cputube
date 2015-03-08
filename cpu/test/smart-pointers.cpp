// cpu/test/smart-pointers.cpp                                        -*-C++-*-
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <bsl_memory.h>
#include <bslma_managedptr.h>

using namespace BloombergLP;

// ----------------------------------------------------------------------------

namespace cputube
{
    template <typename T>
    class rc_ptr
    {
        struct rep {
            T value;
            int count;
            template <typename... Args>
            explicit rep(Args&&... args)
                : value(std::forward<Args>(args)...)
                , count(1)
            {
            }
            rep(rep const&) = delete;
            void operator=(rep const&) = delete;
            ~rep() {
            }
        };
        rep* rep_;
    public:
        template <typename... Args>
        explicit rc_ptr(Args&&... args)
            : rep_(new rep(std::forward<Args>(args)...)) {
        }
        rc_ptr(rc_ptr& other)
            : rep_(other.rep_) {
            ++this->rep_->count;
        }
        rc_ptr(rc_ptr const& other)
            : rep_(other.rep_) {
            ++this->rep_->count;
        }
        rc_ptr(rc_ptr&& other) 
            : rep_(other.rep_) {
            ++this->rep_->count;
        }
        rc_ptr& operator=(rc_ptr other) {
            other.swap(*this);
        }
        ~rc_ptr() {
            if (--this->rep_->count == 0) {
                delete this->rep_;
            }
        }
        void swap(rc_ptr& other) {
            std::swap(this->rep_, other.rep_);
        }

        T& operator*() const { return this->rep_->value; }
        T* operator->() const { return &this->rep_->value; }
    };
    
    template <typename T, typename... Args>
    rc_ptr<T> make_rc(Args&&... args) {
        return rc_ptr<T>(std::forward<Args>(args)...);
    }
}

// ----------------------------------------------------------------------------

namespace cputube
{
    template <typename T>
    class mv_ptr
    {
        struct rep {
            T value;
            int count;
            template <typename... Args>
            explicit rep(Args&&... args)
                : value(std::forward<Args>(args)...)
                , count(1)
            {
            }
            rep(rep const&) = delete;
            void operator=(rep const&) = delete;
            ~rep() {
            }
        };
        rep* rep_;
    public:
        template <typename... Args>
        explicit mv_ptr(Args&&... args)
            : rep_(new rep(std::forward<Args>(args)...)) {
        }
        mv_ptr(mv_ptr& other)
            : rep_(other.rep_) {
            this->rep_ && ++this->rep_->count;
        }
        mv_ptr(mv_ptr const& other)
            : rep_(other.rep_) {
            this->rep_ && ++this->rep_->count;
        }
        mv_ptr(mv_ptr&& other) 
            : rep_(other.rep_) {
            other.rep_ = 0;
        }
        mv_ptr& operator=(mv_ptr other) {
            other.swap(*this);
        }
        ~mv_ptr() {
            if (this->rep_ && --this->rep_->count == 0) {
                delete this->rep_;
            }
        }
        void swap(mv_ptr& other) {
            std::swap(this->rep_, other.rep_);
        }

        T& operator*() const { return this->rep_->value; }
        T* operator->() const { return &this->rep_->value; }
    };
    
    template <typename T, typename... Args>
    mv_ptr<T> make_mv(Args&&... args) {
        return mv_ptr<T>(std::forward<Args>(args)...);
    }
}

// ----------------------------------------------------------------------------

namespace
{
    template <typename T>
    struct raw_config
    {
        using type = T*;
        template <typename... Args>
        type make(Args&&... args) const {
            return new T(std::forward<Args>(args)...);
        }
        void kill(type ptr) const {
            delete ptr;
        }
    };

    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T>
    struct unique_config
    {
        using type = std::unique_ptr<T>;
        template <typename... Args>
        type make(Args&&... args) const {
            return make_unique<T>(std::forward<Args>(args)...);
        }
        void kill(type const&) const {
        }
    };

    template <typename T>
    struct shared_config
    {
        using type = std::shared_ptr<T>;
        template <typename... Args>
        type make(Args&&... args) const {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }
        void kill(type const&) const {
        }
    };

    template <typename T>
    struct boost_shared_config
    {
        using type = boost::shared_ptr<T>;
        template <typename... Args>
        type make(Args&&... args) const {
            return boost::make_shared<T>(std::forward<Args>(args)...);
        }
        void kill(type const&) const {
        }
    };

    template <typename T>
    struct rc_config
    {
        using type = cputube::rc_ptr<T>;
        template <typename... Args>
        type make(Args&&... args) const {
            return cputube::make_rc<T>(std::forward<Args>(args)...);
        }
        void kill(type const&) const {
        }
    };

    template <typename T>
    struct mv_config
    {
        using type = cputube::mv_ptr<T>;
        template <typename... Args>
        type make(Args&&... args) const {
            return cputube::make_mv<T>(std::forward<Args>(args)...);
        }
        void kill(type const&) const {
        }
    };

    template <typename T>
    struct bsl_shared_config
    {
        using type = bsl::shared_ptr<T>;
        template <typename... Args>
        type make(Args&&... args) const {
            //return bsl::make_shared<T>(std::forward<Args>(args)...);
            return bsl::make_shared<T>(args...);
        }
        void kill(type const&) const {
        }
    };

    template <typename T>
    struct bslma_managed_config
    {
        using type = bslma::ManagedPtr<T>;
        template <typename... Args>
        type make(Args&&... args) const {
            return bslma::ManagedPtr<T>(new T(std::forward<Args>(args)...));
        }
        void kill(type const&) const {
        }
    };

}

// ----------------------------------------------------------------------------

namespace
{
    template <int Size>
    struct array
    {
        int values[Size];
        array(int value = 0) { values[Size - 1] = value; }
        operator int()  const { return values[Size - 1]; }
    };
}

// ----------------------------------------------------------------------------

namespace
{
    struct runner
    {
        template <typename Competitor>
        std::pair<cpu::tube::duration, unsigned int>
        measure(cpu::tube::context& context,
                int                 size,
                Competitor const&   competitor) const
        {
            auto timer = context.start();

            unsigned int sum{};
            {
                std::vector<typename Competitor::type> container;
                container.reserve(size);
                for (int i(0); i != size; ++i) {
                    container.emplace_back(competitor.make(i));
                }
                for (auto&& ptr: container) {
                    sum += *ptr;
                    competitor.kill(ptr);
                }
            }
            auto time = timer.measure();
            return std::make_pair(time, sum);
        }
    };

    template <typename T>
    void run_tests(cpu::tube::context& context, std::string type)
    {
        context.run(10, 100000, type, runner(),
                    cpu::tube::make_test_case(type + "*", raw_config<T>()),
                    cpu::tube::make_test_case("std::unique_ptr<" + type + ">", unique_config<T>()),
                    cpu::tube::make_test_case("std::shared_ptr<" + type + ">", shared_config<T>()),
                    cpu::tube::make_test_case("boost::shared_ptr<" + type + ">", boost_shared_config<T>()),
                    cpu::tube::make_test_case("rc_ptr<" + type + ">", rc_config<T>()),
                    cpu::tube::make_test_case("mv_ptr<" + type + ">", mv_config<T>()),
#if !defined(__INTEL_COMPILER)
                    cpu::tube::make_test_case("bslma::ManagedPtr<" + type + ">", bslma_managed_config<T>()),
#endif
                    cpu::tube::make_test_case("bsl::shared_ptr<" + type + ">", bsl_shared_config<T>())
                    );
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    run_tests<int>(context, "int");
    run_tests<array<8>>(context, "array<8>");
    run_tests<array<16>>(context, "array<16>");
}
