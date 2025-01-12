#ifndef CONFIG_HPP
#define CONFIG_HPP

#define BEGIN_NAMESPACE_RE namespace re {
#define END_NAMESPACE_RE }

#if defined(_WIN32)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#endif

#define GLM_FORCE_SSE42
#define GLM_FORCE_ALIGNED

#endif // !CONFIG_HPP