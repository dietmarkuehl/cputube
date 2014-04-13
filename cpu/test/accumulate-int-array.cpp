// cpu/test/accumulate-int-array.cpp                                  -*-C++-*-
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

#include "cpu/tube/context.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <numeric>
#include <vector>

// ----------------------------------------------------------------------------

namespace competitor
{
    template <typename T>
    T accumulate(T* array, int size) {
        return std::accumulate(array, array + size, T(0));
    }

    template <typename T>
    struct range
    {
        T* d_begin;
        T* d_end;
        range(T* begin, T* end): d_begin(begin), d_end(end) {}
    };
    template <typename T>
    T* begin(range<T> const& r) {
        return r.d_begin;
    }
    template <typename T>
    T* end(range<T> const& r) {
        return r.d_end;
    }
    
    template <typename T>
    T range_for(T* array, int size) {
        T result(0);
        for (auto&& value: range<T>(array, array + size)) {
            result += value;
        }
        return result;
    }

    template <typename T>
    T pre_increment_index(T* array, int size) {
        T result(0);
        for (int index(0); index != size; ++index) {
            result += array[index];
        }
        return result;
    }

    template <typename T>
    T pre_increment_less_index(T* array, int size) {
        T result(0);
        for (int index(0); index < size; ++index) {
            result += array[index];
        }
        return result;
    }

    template <typename T>
    T post_increment_index(T* array, int size) {
        T result(0);
        for (int index(0); index != size; index++) {
            result += array[index];
        }
        return result;
    }

    template <typename T>
    T post_increment_less_index(T* array, int size) {
        T result(0);
        for (int index(0); index < size; index++) {
            result += array[index];
        }
        return result;
    }

    template <typename T>
    T pre_decrement_index(T* array, int size) {
        T result(0);
        for (int index(size); index; ) {
            result += array[--index];
        }
        return result;
    }

    template <typename T>
    T pre_decrement_less_index(T* array, int size) {
        T result(0);
        for (int index(size); 0 < index; ) {
            result += array[--index];
        }
        return result;
    }

    template <typename T>
    T post_decrement_index(T* array, int size) {
        T result(0);
        for (int index(size); index--; ) {
            result += array[index];
        }
        return result;
    }

    template <typename T>
    T post_decrement_less_index(T* array, int size) {
        T result(0);
        for (int index(size); 0 < index--; ) {
            result += array[index];
        }
        return result;
    }

    template <typename T>
    T pre_increment_pointer(T* array, int size) {
        T result(0);
        for (T* end(array + size); array != end; ++array) {
            result += *array;
        }
        return result;
    }

    template <typename T>
    T pre_increment_less_pointer(T* array, int size) {
        T result(0);
        for (T* end(array + size); array < end; ++array) {
            result += *array;
        }
        return result;
    }

    template <typename T>
    T post_increment_pointer(T* array, int size) {
        T result(0);
        for (T* end(array + size); array != end; array++) {
            result += *array;
        }
        return result;
    }

    template <typename T>
    T post_increment_less_pointer(T* array, int size) {
        T result(0);
        for (T* end(array + size); array < end; array++) {
            result += *array;
        }
        return result;
    }

    template <typename T>
    T pre_decrement_pointer(T* array, int size) {
        T result(0);
        if (size != 0) {
            for (T* end(array + size); end != array; ) {
                result += *--end;
            }
        }
        return result;
    }

    template <typename T>
    T pre_decrement_less_pointer(T* array, int size) {
        T result(0);
        if (size != 0) {
            for (T* end(array + size); array < end; ) {
                result += *--end;
            }
        }
        return result;
    }

    template <typename T>
    T post_decrement_pointer(T* array, int size) {
        T result(0);
        if (size != 0) {
            for (T* end(array + size); end-- != array; ) {
                result += *end;
            }
        }
        return result;
    }

    template <typename T>
    T post_decrement_less_pointer(T* array, int size) {
        T result(0);
        if (size != 0) {
            for (T* end(array + size); array < end--; ) {
                result += *end;
            }
        }
        return result;
    }

    template <typename T>
    T unrolled_pointer(T* array, int size) {
        T result(0);
        T* end(array + size);
        for (; 4 <= end - array; array += 4) {
            result += array[0];
            result += array[1];
            result += array[2];
            result += array[3];
        }
        for (; array != end; ++array) {
            result += *array;
        }
        return result;
    }

    template <typename T>
    T unrolled_pointer2(T* array, int size) {
        T result(0);
        T* end(array + size);
        for (; 4 <= end - array; array += 4) {
            result += array[0] + array[1] + array[2] + array[3];
        }
        for (; array != end; ++array) {
            result += *array;
        }
        return result;
    }

    template <typename T>
    T unrolled_index(T* array, int size) {
        T result(0);
        int index(0);
        for (; 4 <= size - index; index += 4) {
            result += array[index + 0];
            result += array[index + 1];
            result += array[index + 2];
            result += array[index + 3];
        }
        for (; index != size; ++index) {
            result += array[index];
        }
        return result;
    }

    template <typename T>
    T unrolled_index2(T* array, int size) {
        T result(0);
        int index(0);
        for (; 4 <= size - index; index += 4) {
            result += array[index + 0] + array[index + 1]
                    + array[index + 2] + array[index + 3];
        }
        for (; index != size; ++index) {
            result += array[index];
        }
        return result;
    }
    
    template <typename T>
    T unrolled_index3(T* array, int size) {
        T r1(0),r2(0),r3(0),r4(0);
        int index(0);
        for (; 4 <= size - index; index += 4) {
            r1 += array[index + 0];
            r2 += array[index + 1];
            r3 += array[index + 2];
            r4 += array[index + 3];
        }
        for (; index != size; ++index) {
            r1 += array[index];
        }
        return r1+r2+r3+r4;
    }    
}

// ----------------------------------------------------------------------------

namespace test
{
    template <typename T>
    void measure(cpu::tube::context& context,
                 char const* name, T (*function)(T*, int)) {
        T array1[1024];
        T array2[1024];
        for (long i(0); i != 1024; ++i) {
            array1[i] = i;
            array2[1023 - i] = i;
        }
        long total(0);
        auto timer = context.start();
        for (T i(0); i != 1000000; ++i) {
            total += function(array1, 1024);
            total += function(array2, 1024);
        }
        context.report(name, timer, total);
    }


    template <typename T>
    void measure(cpu::tube::context& context)
    {
        test::measure(context, "accumulate", competitor::accumulate<T>);
        test::measure(context, "range for", competitor::range_for<T>);
        test::measure(context, "pre increment index", competitor::pre_increment_index<T>);
        test::measure(context, "pre increment less index", competitor::pre_increment_less_index<T>);
        test::measure(context, "post increment index", competitor::post_increment_index<T>);
        test::measure(context, "post increment less index", competitor::post_increment_less_index<T>);
        test::measure(context, "pre decrement index", competitor::pre_decrement_index<T>);
        test::measure(context, "pre decrement less index", competitor::pre_decrement_less_index<T>);
        test::measure(context, "post decrement index", competitor::post_decrement_index<T>);
        test::measure(context, "post decrement less index", competitor::post_decrement_less_index<T>);
        test::measure(context, "pre increment pointer", competitor::pre_increment_pointer<T>);
        test::measure(context, "pre increment less pointer", competitor::pre_increment_less_pointer<T>);
        test::measure(context, "post increment pointer", competitor::post_increment_pointer<T>);
        test::measure(context, "post increment less pointer", competitor::post_increment_less_pointer<T>);
        test::measure(context, "pre decrement pointer", competitor::pre_decrement_pointer<T>);
        test::measure(context, "pre decrement less pointer", competitor::pre_decrement_less_pointer<T>);
        test::measure(context, "post decrement pointer", competitor::post_decrement_pointer<T>);
        test::measure(context, "post decrement less pointer", competitor::post_decrement_less_pointer<T>);
        test::measure(context, "unrolled pointer", competitor::unrolled_pointer<T>);
        test::measure(context, "unrolled index", competitor::unrolled_index<T>);
        test::measure(context, "unrolled pointer2", competitor::unrolled_pointer2<T>);
        test::measure(context, "unrolled index2", competitor::unrolled_index2<T>);
        test::measure(context, "unrolled index3", competitor::unrolled_index3<T>);
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    cpu::tube::context context(CPUTUBE_CONTEXT_ARGS(ac, av));
    std::cout << "unsigned int\n";
    test::measure<unsigned int>(context);
    std::cout << "float\n";
    test::measure<float>(context);
    std::cout << "double\n";
    test::measure<double>(context);
}
