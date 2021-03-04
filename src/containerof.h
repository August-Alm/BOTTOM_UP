/* ***** ***** */

#ifndef CONTAINEROF_H
#define CONTAINEROF_H

/* ***** ***** */

#if defined(__GNUC__) || defined(__CLANG__)
    #define OFFSETOF(st, f) (__builtin_offsetof(st, f))
#else
    #define OFFSETOF(st, f) ((size_t)((char *)&((st *)0)->f - (char*)0))
#endif

#define CONTAINEROF(ptr, st, f) \
    ((st*)((char*)(1 ? (ptr) : &((st*)0)->f) - OFFSETOF(st, f)))

/* ***** ***** */

#endif // CONTAINEROF_H
