#ifndef EXPORT_HPP
#define EXPORT_HPP

// #define BEGIN_NAMESPACE_RL namespace rl {
// #define END_NAMESPACE_RL }

// #if defined(_WIN32)
//     #ifndef NOMINMAX
//         #define NOMINMAX
//     #endif
// #endif

#if !defined(RENDERLIB_STATIC)
    #if defined(_WIN32)
        #define RL_API_EXPORT __declspec(dllexport)
        #define RL_API_IMPORT __declspec(dllimport)

        // For Visual C++ compilers, we also need to turn off this annoying C4251 warning
        #ifdef _MSC_VER
            #pragma warning(disable: 4251)
        #endif

    #elif defined(__unix__)
        #define RL_API_EXPORT __attribute__ ((__visibility__ ("default")))
        #define RL_API_IMPORT __attribute__ ((__visibility__ ("default")))
    #else
        // Unsupported system
        #error This operating system is not supported by render library
    #endif

    #if defined(RENDERLIB_API_EXPORTS)
        #define RL_API RL_API_EXPORT
    #else
        #define RL_API RL_API_IMPORT
    #endif

#else
    #define RL_API
    #define RL_API
#endif

#endif // !EXPORT_HPP