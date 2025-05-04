
#ifndef SHINY_API_H
#define SHINY_API_H

#ifdef SHINY_STATIC_DEFINE
#  define SHINY_API
#  define SHINY_NO_EXPORT
#else
#  ifndef SHINY_API
#    ifdef shiny_EXPORTS
        /* We are building this library */
#      define SHINY_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define SHINY_API __declspec(dllimport)
#    endif
#  endif

#  ifndef SHINY_NO_EXPORT
#    define SHINY_NO_EXPORT 
#  endif
#endif

#ifndef SHINY_DEPRECATED
#  define SHINY_DEPRECATED __declspec(deprecated)
#endif

#ifndef SHINY_DEPRECATED_EXPORT
#  define SHINY_DEPRECATED_EXPORT SHINY_API SHINY_DEPRECATED
#endif

#ifndef SHINY_DEPRECATED_NO_EXPORT
#  define SHINY_DEPRECATED_NO_EXPORT SHINY_NO_EXPORT SHINY_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef SHINY_NO_DEPRECATED
#    define SHINY_NO_DEPRECATED
#  endif
#endif

#endif /* SHINY_API_H */
