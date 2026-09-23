#pragma once

#include <memory>

#include "OpenGLApiExport.hpp"
#include "GraphicsApi.hpp"


class OGL_API OpenGLApi final:
    public GraphicsApi
{
public:
    OpenGLApi(Camera& camera) noexcept;
    ~OpenGLApi();

    bool createContext()                   noexcept;
    bool createView(uint64_t windowHandle) noexcept;
    void drawFrame()                 const noexcept;
    void resize(int width, int height)     noexcept;

private:
    void initializeDebug() noexcept;
    
    uint32_t m_uniformBuffer = 0;
    uint32_t m_shaderProgram = 0;
    uint32_t m_vertexBufferObject = 0;
    uint32_t m_indexBufferObject = 0;
    uint32_t m_vertexArrayObject = 0;
    uint32_t m_texture = 0;
    uint32_t m_indexCount = 0;

    int32_t m_width = 0;
    int32_t m_height = 0;
};
