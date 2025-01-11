#include <cstdio>

#include <glad/glad.h>

#include "OpenGLDebugger.hpp"


static const char* glSourceToString(const GLenum source)
{
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             return "DEBUG_SOURCE_API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "DEBUG_SOURCE_WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "GL_DEBUG_SOURCE_SHADER_COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY:     return "DEBUG_SOURCE_THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:     return "DEBUG_SOURCE_APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:           return "DEBUG_SOURCE_OTHER";

        default: return "UNKNOWN_DEBUG_SOURCE";
    }
}


static const char* glTypeToString(const GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               return "DEBUG_TYPE_ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEBUG_TYPE_DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "DEBUG_TYPE_UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY:         return "DEBUG_TYPE_PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE:         return "DEBUG_TYPE_PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER:              return "DEBUG_TYPE_MARKER";
        case GL_DEBUG_TYPE_PUSH_GROUP:          return "DEBUG_TYPE_PUSH_GROUP";
        case GL_DEBUG_TYPE_POP_GROUP:           return "DEBUG_TYPE_POP_GROUP";
        case GL_DEBUG_TYPE_OTHER:               return "DEBUG_TYPE_OTHER";

        default: return "UNKNOWN_DEBUG_TYPE";
    }
}


OpenGLDebugger::OpenGLDebugger() noexcept
{
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    glDebugMessageCallback([](GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei length,
                              const GLchar* message,
                              const void* userParam)
        {
            switch (severity)
            {
                case GL_DEBUG_SEVERITY_HIGH:
                    printf("OpenGL Error: [%s: %s](%u): %s\n",
                        glSourceToString(source),
                        glTypeToString(type),
                        id,
                        message);
                break;

                case GL_DEBUG_SEVERITY_MEDIUM:
                    printf("OpenGL Warning: [%s: %s](%u): %s\n",
                        glSourceToString(source),
                        glTypeToString(type),
                        id,
                        message);
                break;

                case GL_DEBUG_SEVERITY_LOW:
                    printf("OpenGL Info: [%s: %s](%u): %s\n",
                        glSourceToString(source),
                        glTypeToString(type),
                        id,
                        message);
                break;

                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    printf("OpenGL Notificaton: [%s: %s](%u): %s\n",
                        glSourceToString(source),
                        glTypeToString(type),
                        id,
                        message);
                break;

                default:
                    printf("OpenGL Error: [%s: %s](%u): %s\n",
                        glSourceToString(source),
                        glTypeToString(type),
                        id,
                        message);
            }
        }, nullptr);
}


OpenGLDebugger::~OpenGLDebugger() noexcept = default;