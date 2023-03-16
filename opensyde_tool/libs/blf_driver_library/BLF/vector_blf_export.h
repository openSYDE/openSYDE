
#ifndef VECTOR_BLF_EXPORT_H
#define VECTOR_BLF_EXPORT_H

#ifdef VECTOR_BLF_STATIC_DEFINE
#  define VECTOR_BLF_EXPORT
#  define VECTOR_BLF_NO_EXPORT
#else
#  ifndef VECTOR_BLF_EXPORT
#    ifdef Vector_BLF_EXPORTS
        /* We are building this library */
#      define VECTOR_BLF_EXPORT /*__declspec(dllexport)*/
#    else
        /* We are using this library */
#      define VECTOR_BLF_EXPORT /*__declspec(dllimport)*/
#    endif
#  endif

#  ifndef VECTOR_BLF_NO_EXPORT
#    define VECTOR_BLF_NO_EXPORT 
#  endif
#endif

#ifndef VECTOR_BLF_DEPRECATED
#  define VECTOR_BLF_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef VECTOR_BLF_DEPRECATED_EXPORT
#  define VECTOR_BLF_DEPRECATED_EXPORT VECTOR_BLF_EXPORT VECTOR_BLF_DEPRECATED
#endif

#ifndef VECTOR_BLF_DEPRECATED_NO_EXPORT
#  define VECTOR_BLF_DEPRECATED_NO_EXPORT VECTOR_BLF_NO_EXPORT VECTOR_BLF_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define VECTOR_BLF_NO_DEPRECATED
#endif

#endif
