/* ***** ***** */

#ifndef FORCEINLINE_H
#define FORCEINLINE_H

/* ***** ***** */

#if defined(__GNUC__)
    #define FORCEINLINE inline __attribute__((__always_inline__))
#elif defined(__CLANG__)
    #if __has_attribute(__always_inline__)
        #define FORCEINLINE inline __attribute__((__always_inline__))
    #else
        #define FORCEINLINE inline
    #endif
#elif defined(_MSC_VER)
    #define FORCEINLINE __forceinline
#else
    #define FORCEINLINE inline
#endif

/* ***** ***** */

#endif // FORCEINLINE_H