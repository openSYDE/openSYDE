
#ifndef VECTOR_DBC_EXPORT_H
#define VECTOR_DBC_EXPORT_H

#ifdef VECTOR_DBC_STATIC_DEFINE
#  define VECTOR_DBC_EXPORT
#  define VECTOR_DBC_NO_EXPORT
#else
#  ifndef VECTOR_DBC_EXPORT
#    ifdef Vector_DBC_EXPORTS
        /* We are building this library */
#      define VECTOR_DBC_EXPORT /*__declspec(dllexport)*/
#    else
        /* We are using this library */
#      define VECTOR_DBC_EXPORT /*__declspec(dllimport)*/
#    endif
#  endif

#  ifndef VECTOR_DBC_NO_EXPORT
#    define VECTOR_DBC_NO_EXPORT 
#  endif
#endif

#ifndef VECTOR_DBC_DEPRECATED
#  define VECTOR_DBC_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef VECTOR_DBC_DEPRECATED_EXPORT
#  define VECTOR_DBC_DEPRECATED_EXPORT VECTOR_DBC_EXPORT VECTOR_DBC_DEPRECATED
#endif

#ifndef VECTOR_DBC_DEPRECATED_NO_EXPORT
#  define VECTOR_DBC_DEPRECATED_NO_EXPORT VECTOR_DBC_NO_EXPORT VECTOR_DBC_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef VECTOR_DBC_NO_DEPRECATED
#    define VECTOR_DBC_NO_DEPRECATED
#  endif
#endif

#endif /* VECTOR_DBC_EXPORT_H */
