// cpu/tube/heap_fragment.hpp                                         -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2014 Thaddaeus Frogley         
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

#ifndef INCLUDED_HEAP_FRAGMENT_TUBE
#define INCLUDED_HEAP_FRAGMENT_TUBE

#include <vector>

// ----------------------------------------------------------------------------
// Creating this object leaves the heap in a fragmented state, with N allocations
//  of 1...S bytes still allocated, remaining allocations are freed on destruction
// the goal is to approximate the memory conditions of a live application
// so cache coherency of heap allocated data structure access can be tested 
// under closer to real world conditions

class heap_fragmenter
{
	public:
	heap_fragmenter( int n, int s );
	~heap_fragmenter();
	
	private:
	std::vector< char* > allocated;
};

#endif // INCLUDED_HEAP_FRAGMENT_TUBE
