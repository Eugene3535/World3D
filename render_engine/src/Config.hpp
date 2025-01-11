#ifndef EXPORT_HPP
#define EXPORT_HPP

#define BEGIN_NAMESPACE_RE namespace re {
#define END_NAMESPACE_RE }

#if defined(_WIN32)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#endif

#endif // !EXPORT_HPP