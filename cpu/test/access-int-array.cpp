// cpu/test/access-int-array.cpp                                      -*-C++-*-
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

#include "cpu/tube/processor.hpp"
#include "cpu/tube/timer.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <vector>

// ----------------------------------------------------------------------------

namespace competitor
{
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
}

// ----------------------------------------------------------------------------

namespace test
{
    void measure(char const* name, int (*function)(int*, int)) {
        int array1[1024];
        int array2[1024];
        for (long i(0); i != 1024; ++i) {
            array1[i] = i;
            array2[1023 - i] = i;
        }
        long total(0);
        cpu::tube::timer timer;
        for (int i(0); i != 100000; ++i) {
            total += function(array1, 1024);
            total += function(array2, 1024);
        }
        cpu::tube::duration duration = timer.measure();
        std::cout << std::setw(40) << name << ' '
                  << std::setw(6)  << duration << ' '
                  << std::setw(10) << total << '\n';
            
    }
}

// ----------------------------------------------------------------------------

int main()
{
    std::cout << "processor=" << cpu::tube::processor() << '\n';

    test::measure("pre increment index", competitor::pre_increment_index<int>);
    test::measure("pre increment less index", competitor::pre_increment_less_index<int>);
    test::measure("post increment index", competitor::post_increment_index<int>);
    test::measure("post increment less index", competitor::post_increment_less_index<int>);
    test::measure("pre decrement index", competitor::pre_decrement_index<int>);
    test::measure("pre decrement less index", competitor::pre_decrement_less_index<int>);
    test::measure("post decrement index", competitor::post_decrement_index<int>);
    test::measure("post decrement less index", competitor::post_decrement_less_index<int>);
    test::measure("pre increment pointer", competitor::pre_increment_pointer<int>);
    test::measure("pre increment less pointer", competitor::pre_increment_less_pointer<int>);
    test::measure("post increment pointer", competitor::post_increment_pointer<int>);
    test::measure("post increment less pointer", competitor::post_increment_less_pointer<int>);
    test::measure("pre decrement pointer", competitor::pre_decrement_pointer<int>);
    test::measure("pre decrement less pointer", competitor::pre_decrement_less_pointer<int>);
    test::measure("post decrement pointer", competitor::post_decrement_pointer<int>);
    test::measure("post decrement less pointer", competitor::post_decrement_less_pointer<int>);
}
