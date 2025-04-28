
#ifndef RENDERER_API_H
#define RENDERER_API_H

#ifdef RENDERER_STATIC_DEFINE
#  define RENDERER_API
#  define RENDERER_NO_EXPORT
#else
#  ifndef RENDERER_API
#    ifdef renderer_EXPORTS
        /* We are building this library */
#      define RENDERER_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define RENDERER_API __declspec(dllimport)
#    endif
#  endif

#  ifndef RENDERER_NO_EXPORT
#    define RENDERER_NO_EXPORT 
#  endif
#endif

#ifndef RENDERER_DEPRECATED
#  define RENDERER_DEPRECATED __declspec(deprecated)
#endif

#ifndef RENDERER_DEPRECATED_EXPORT
#  define RENDERER_DEPRECATED_EXPORT RENDERER_API RENDERER_DEPRECATED
#endif

#ifndef RENDERER_DEPRECATED_NO_EXPORT
#  define RENDERER_DEPRECATED_NO_EXPORT RENDERER_NO_EXPORT RENDERER_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef RENDERER_NO_DEPRECATED
#    define RENDERER_NO_DEPRECATED
#  endif
#endif

#endif /* RENDERER_API_H */
