
#ifndef VK_API_H
#define VK_API_H

#ifdef VK_API_STATIC_DEFINE
#  define VK_API
#  define VULKAN_API_NO_EXPORT
#else
#  ifndef VK_API
#    ifdef vulkan_api_EXPORTS
        /* We are building this library */
#      define VK_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define VK_API __declspec(dllimport)
#    endif
#  endif

#  ifndef VULKAN_API_NO_EXPORT
#    define VULKAN_API_NO_EXPORT 
#  endif
#endif

#ifndef VULKAN_API_DEPRECATED
#  define VULKAN_API_DEPRECATED __declspec(deprecated)
#endif

#ifndef VULKAN_API_DEPRECATED_EXPORT
#  define VULKAN_API_DEPRECATED_EXPORT VK_API VULKAN_API_DEPRECATED
#endif

#ifndef VULKAN_API_DEPRECATED_NO_EXPORT
#  define VULKAN_API_DEPRECATED_NO_EXPORT VULKAN_API_NO_EXPORT VULKAN_API_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef VULKAN_API_NO_DEPRECATED
#    define VULKAN_API_NO_DEPRECATED
#  endif
#endif

#endif /* VK_API_H */
