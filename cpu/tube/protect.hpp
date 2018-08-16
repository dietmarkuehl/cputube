// cpu/tube/protect.hpp                                               -*-C++-*-
// ----------------------------------------------------------------------------

#ifndef INCLUDED_CPU_TUBE_PROTECT
#define INCLUDED_CPU_TUBE_PROTECT

// ----------------------------------------------------------------------------

namespace cpu {
    namespace tube {
        void escape(void *p) {
           asm volatile("" : : "g"(p) : "memory");
        }
        void escape(void const *p) {
           asm volatile("" : : "g"(p) : "memory");
        }

        void clobber() {
           asm volatile("" : : : "memory");
        }
    }
}

// ----------------------------------------------------------------------------

#endif
