// data-structures/hash_set.hpp                                       -*-C++-*-
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

#ifndef INCLUDED_DATA_STRUCTURES_HASH_SET
#define INCLUDED_DATA_STRUCTURES_HASH_SET

#include <iostream> //-dk:TODO remove
#include <algorithm>
#include <functional>
#include <memory>
#include <new>
#include <utility>
#include <cstdlib>

// ----------------------------------------------------------------------------

namespace cpu {
    namespace data_structures {
        template <typename Key,
                  typename Hash = std::hash<Key>,
                  typename Equal = std::equal_to<Key>>
        class hash_set;
    }
}

// ----------------------------------------------------------------------------

template <typename Key, typename Hash, typename Equal>
class cpu::data_structures::hash_set {
private:
    using flag = unsigned char; // high bit: empty; other bits: lower hash bits
    static bool p_is_empty(flag f) { return f & flag{0x80}; }

public:
    using size_type = std::size_t;

    class iterator {
    private:
        friend class hash_set;

        const hash_set *d_container;
        std::ptrdiff_t  d_index;

        Key& value() const { return this->d_container->d_values[this->d_index].value; }

    public:
        using value_type        = Key;
        using reference         = Key const&;
        using pointer           = Key const*;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        iterator(const hash_set* c, std::ptrdiff_t i) : d_container(c), d_index(i) {}

        Key const& operator*() const { return this->value(); }
        Key const* operator->() const { return &this->value(); }
        iterator& operator++() {
            this->d_index = this->d_container->p_find_if(this->d_index + 1, [](flag f){
                    return !p_is_empty(f); }) - this->d_container->p_begin();
            return *this;
        }
        iterator  operator++(int) { iterator rc{*this}; ++*this; return rc; }
        friend bool operator== (iterator i0, iterator i1) { return i0.d_index == i1.d_index; }
        friend bool operator!= (iterator i0, iterator i1) { return !(i0 == i1); }
    };
    using const_iterator = iterator;

private:
    union element {
        Key value;
        element() {}
        template <typename... T>
        element(T&&... a): value(std::forward<T>(a)...) {}
        ~element() {}
    };

    std::ptrdiff_t             d_size{};
    std::size_t                d_mask;
    std::unique_ptr<flag[]>    d_flags{new flag[this->d_mask + 1u]};
    std::unique_ptr<element[]> d_values{new element[this->d_mask + 1u]};
    Hash                       d_hash;
    Equal                      d_equal;

    flag* p_begin() const { return this->d_flags.get(); }
    flag* p_end() const   { return this->d_flags.get() + this->d_mask + 1; }
    template <typename Pred>
    flag* p_find_if(std::ptrdiff_t pos, Pred pred) const {
        return std::find_if(this->p_begin() + pos, this->p_end(), pred);
    }

    flag* p_find(std::size_t hash, Key const& key) const {
        auto const pred{[x = flag(hash & 0x7f)](flag f){
                return f == x || p_is_empty(f);
            }};
        auto init{this->p_begin() + ((hash >> 7) & this->d_mask)};
        for (auto it{init}, end{this->p_end()};
             end != (it = std::find_if(it, end, pred)); ++it) {
            if (p_is_empty(*it)
                || this->d_equal(key, this->d_values[it - this->p_begin()].value)) {
                return it;
            }
        }
        for (auto it{this->p_begin()}, end{init};
             end != (it = std::find_if(it, end, pred)); ++it) {
            if (p_is_empty(*it)
                || this->d_equal(key, this->d_values[it - this->p_begin()].value)) {
                return it;
            }
        }
        // since the buckets can never be full this point isn't reached!
        return this->p_end();
    }
    void p_resize() {
        hash_set tmp(2 * (this->d_mask + 1), this->d_hash, this->d_equal);
        for (auto& key: *this) {
            tmp.p_insert(std::move(key));
        }
        this->swap(tmp);
    }
    template <typename K>
    std::pair<iterator, bool> p_insert(K&& key) {
        if (this->d_size == std::ptrdiff_t(this->d_mask - (this->d_mask >> 3))) {
            this->p_resize();
        }
        auto hash{this->d_hash(key) >> 3};
        auto it{this->p_find(hash, key)};
        auto index{it - this->p_begin()};

        bool empty{p_is_empty(*it)};
        if (empty) {
            ++this->d_size;
            *it = flag(hash & 0x7f);
            new(&this->d_values[index].value) Key{std::forward<K>(key)};
        }
        return std::make_pair(iterator{this, index}, empty);
    }
    void p_destroy() {
        for (auto it{this->begin()}, end{this->end()}; it != end; ++it) {
            it.value().~Key();
        }
    }

public:
    hash_set(): hash_set{16u} {}
    explicit hash_set(std::size_t capacity, // must be a power of 2
                      const Hash& hash = Hash{},
                      const Equal& equal = Equal{})
        : d_mask{capacity - 1u}, d_hash{hash}, d_equal{equal} {
            std::fill(this->d_flags.get(), this->d_flags.get() + this->d_mask + 1, flag(0x80));
    }
    template <typename InIt>
    explicit hash_set(InIt it, InIt end)
        : hash_set{16u} {
        for (; it != end; ++it) {
            this->emplace(*it);
        }
    }
    ~hash_set() { this->p_destroy(); }
    bool           empty() const { return 0u == this->d_size; }
    size_type      size() const { return this->d_size; }
    const_iterator begin() const {
        return const_iterator{this, this->p_find_if(0, [](flag f){
                    return !p_is_empty(f); }) - this->p_begin()};
    }
    const_iterator end() const { return const_iterator{this, std::ptrdiff_t(this->d_mask) + 1}; }
    const_iterator find(const Key& key) const {
        auto hash{this->d_hash(key) >> 3};
        auto it{this->p_find(hash, key)};
        return p_is_empty(*it)? this->end(): const_iterator{this, it - this->p_begin()};
    }

    void swap(hash_set& other) {
        using std::swap;
        swap(this->d_size,   other.d_size);
        swap(this->d_mask,   other.d_mask);
        swap(this->d_flags,  other.d_flags);
        swap(this->d_values, other.d_values);
    }
    template <typename... T>
    std::pair<iterator, bool> emplace(T&&... a) {
        return this->p_insert(Key(std::forward<T>(a)...));
    }
};

// ----------------------------------------------------------------------------

#endif
