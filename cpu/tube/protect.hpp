// cpu/tube/protect.hpp                                               -*-C++-*-
// ----------------------------------------------------------------------------

#ifndef INCLUDED_CPU_TUBE_PROTECT
#define INCLUDED_CPU_TUBE_PROTECT

// ----------------------------------------------------------------------------

namespace cpu {
    namespace tube {
#if !defined(_MSC_VER)
        inline void escape(void const *p);
        inline void escape(void *p);
#endif

        template <typename T>
        void prevent_optimize_away(T&& value);
    }
}

// ----------------------------------------------------------------------------

#if defined(_MSC_VER)
#  pragma optimize("", off)

template <typename T>
void prevent_optimize_away(T&& value) {
    value = value;
}

#  pragma optimize("", on)
#else

inline void cpu::tube::escape(void const *p) {
    asm volatile("" : : "g"(p) : "memory");
}
inline void cpu::tube::escape(void *p) {
    asm volatile("" : : "g"(p) : "memory");
}

template <typename T>
void cpu::tube::prevent_optimize_away(T&& value) {
    cpu::tube::escape(&value);
}
#endif

// ----------------------------------------------------------------------------

#endif
