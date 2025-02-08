#ifndef CONFIG_HPP
#define CONFIG_HPP

#if defined(_WIN32)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#endif

#define GLM_FORCE_SSE42
#define GLM_FORCE_ALIGNED

#endif // !CONFIG_HPP