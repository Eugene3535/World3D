#ifndef EXPORT_HPP
#define EXPORT_HPP

#include "Config.hpp"

#if defined(_WIN32)
    #define RE_API_EXPORT __declspec(dllexport)
    #define RE_API_IMPORT __declspec(dllimport)

    // For Visual C++ compilers, we also need to turn off this annoying C4251 warning
    #ifdef _MSC_VER
        #pragma warning(disable: 4251)
    #endif

#elif defined(__unix__)
    #define RE_API_EXPORT __attribute__ ((__visibility__ ("default")))
    #define RE_API_IMPORT __attribute__ ((__visibility__ ("default")))
#else
    // Unsupported system
    #error This operating system is not supported by render library
#endif

#if defined(RENDER_ENGINE_API_EXPORTS)
    #define RE_API RE_API_EXPORT
#else
    #define RE_API RE_API_IMPORT
#endif

#endif // !EXPORT_HPP