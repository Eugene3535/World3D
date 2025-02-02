#ifndef EXPORT_HPP
#define EXPORT_HPP

#include "Config.hpp"

#if defined(_WIN32)
    #define OGL_API_EXPORT __declspec(dllexport)
    #define OGL_API_IMPORT __declspec(dllimport)

    // For Visual C++ compilers, we also need to turn off this annoying C4251 warning
    #ifdef _MSC_VER
        #pragma warning(disable: 4251)
    #endif

#elif defined(__unix__)
    #define OGL_API_EXPORT __attribute__ ((__visibility__ ("default")))
    #define OGL_API_IMPORT __attribute__ ((__visibility__ ("default")))
#else
    // Unsupported system
    #error This operating system is not supported by render library
#endif

#if defined(OPENGL_UTILS_API_EXPORTS)
    #define OGL_API OGL_API_EXPORT
#else
    #define OGL_API OGL_API_IMPORT
#endif

#endif // !EXPORT_HPP