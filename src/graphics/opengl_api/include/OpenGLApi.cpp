#include <span>
#include <type_traits>
#include <array>
#ifdef DEBUG
#include <cstdio>
#endif

#include <glad/glad.h>
#include <cglm/struct/affine-pre.h>
#include <cglm/struct/cam.h>

#include "files/FileProvider.hpp"
#include "program/Shader.hpp"
#include "texture/Texture2D.hpp"
#include "vao/VertexBufferLayout.hpp"
#include "geometry/Geometry3D.hpp"
#include "OpenGLApi.hpp"


static GLuint create_program(std::span<const Shader> shaders) noexcept
{
    GLint program = glCreateProgram();

    for(const auto& shader : shaders)
        glAttachShader(program, shader.getHandle());
    
    glLinkProgram(program);

    int32_t success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        GLchar infoLog[1024]{};
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
#ifdef DEBUG
        printf("SHADER PROGRAM: Link-time error:\n%s\n", infoLog);
#endif
        glDeleteProgram(program);

        return 0;
    }
    else
    {
        GLuint uniformBlockIndex = glGetUniformBlockIndex(program, "Matrices");

        if (uniformBlockIndex != GL_INVALID_INDEX)
            glUniformBlockBinding(program, uniformBlockIndex, 0);
    }

    for(const auto& shader : shaders)
        glDetachShader(program, shader.getHandle());

    return program;
}


OpenGLApi::OpenGLApi(Camera& camera) noexcept:
    GraphicsApi(camera)
{

}


OpenGLApi::~OpenGLApi()
{
    glDeleteVertexArrays(1, &m_vertexArrayObject);
    glDeleteBuffers(1, &m_vertexBufferObject);
    glDeleteBuffers(1, &m_indexBufferObject);
    glDeleteTextures(1, &m_texture);
    glDeleteProgram(m_shaderProgram);
}



bool OpenGLApi::createContext() noexcept
{
    static_assert(std::is_same_v<GLbyte, int8_t>, "GLbyte mismatch");
    static_assert(std::is_same_v<GLubyte, uint8_t>, "GLubyte mismatch");
    static_assert(std::is_same_v<GLshort, int16_t>, "GLshort mismatch");
    static_assert(std::is_same_v<GLushort, uint16_t>, "GLushort mismatch");
    static_assert(std::is_same_v<GLint, int32_t>, "GLint mismatch");
    static_assert(std::is_same_v<GLuint, uint32_t>, "GLuint mismatch");
    static_assert(std::is_same_v<GLfloat, float>, "GLfloat mismatch");
    static_assert(std::is_same_v<GLdouble, double>, "GLdouble mismatch");

    if (!gladLoadGL())
        return false;

    initializeDebug();

    std::array<Shader, 2> shaders;

    if (!shaders[0].loadFromFile(FileProvider::findPathToFile("shader.vert"), GL_VERTEX_SHADER))
        return false;

    if (!shaders[1].loadFromFile(FileProvider::findPathToFile("shader.frag"), GL_FRAGMENT_SHADER))
        return false;

    m_shaderProgram = create_program(shaders);

    if (!m_shaderProgram)
        return false;

    glUseProgram(m_shaderProgram);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
    Texture2D texture(m_texture);

    if (!texture.loadFromFile(FileProvider::findPathToFile("container.jpg")))
        return false;

    Grid3D grid = 
    {
        .cellCount = { 5, 5 },
        .isTiled = true,
        .texture =
        {
            .size = texture.getSize(),
            .isEnabled = true,
            .isRepeated = false
        }
    };

    if (!grid.create())
        return false;

    std::span<float> vertices = grid.vertices;
    std::span<uint32_t> indices = grid.indices;

    glCreateBuffers(1, &m_vertexBufferObject);
    glCreateBuffers(1, &m_indexBufferObject);

    glNamedBufferStorage(m_vertexBufferObject, static_cast<GLsizeiptr>(vertices.size_bytes()), vertices.data(), 0);
	glNamedBufferStorage(m_indexBufferObject, static_cast<GLsizeiptr>(indices.size_bytes()), indices.data(), 0);

    glCreateVertexArrays(1, &m_vertexArrayObject);

    const std::array<VertexBufferLayout::Attribute, 2> attributes 
    { 
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };

    VertexBufferLayout layout(attributes);
    layout.createVertexInputState(m_vertexArrayObject, m_vertexBufferObject);
    glVertexArrayElementBuffer(m_vertexArrayObject, m_indexBufferObject);
    m_indexCount = static_cast<uint32_t>(indices.size());

    glCreateBuffers(1, &m_uniformBuffer);
    glNamedBufferData(m_uniformBuffer, sizeof(mat4s), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uniformBuffer, 0, sizeof(mat4s));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    return true;
}


bool OpenGLApi::createView(uint64_t windowHandle) noexcept
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    return true;
}


void OpenGLApi::drawFrame() const noexcept
{
    mat4s projection = glms_perspective(glm_rad(60.f), (float)m_width / (float)m_height, 0.1f, 100.f);
    const auto modelView = m_camera.getViewMatrix();
    const mat4s modelViewProjection = glms_mat4_mul(projection, modelView);

    glNamedBufferSubData(m_uniformBuffer, 0, sizeof(mat4s), static_cast<const void*>(modelViewProjection.raw));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(m_vertexArrayObject);
    glBindTextureUnit(0, m_texture);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glBindTextureUnit(0, 0);
    glBindVertexArray(0);
}


void OpenGLApi::resize(int width, int height) noexcept
{
    glViewport(0, 0, width,  height);
    m_width = width;
    m_height = height;
}


void OpenGLApi::initializeDebug() noexcept
{
#ifdef DEBUG
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
	{
		auto source_str = [source]() -> const char*
		{
			switch (source)
			{
				case GL_DEBUG_SOURCE_API:             return "API";
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "WINDOW SYSTEM";
				case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
				case GL_DEBUG_SOURCE_THIRD_PARTY:     return "THIRD PARTY";
				case GL_DEBUG_SOURCE_APPLICATION:     return "APPLICATION";
				case GL_DEBUG_SOURCE_OTHER:           return "OTHER";
				
				default: return "UNKNOWN";
			}
		}();

		auto type_str = [type]() -> const char*
		{
			switch (type)
			{
				case GL_DEBUG_TYPE_ERROR:               return "ERROR";
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UNDEFINED_BEHAVIOR";
				case GL_DEBUG_TYPE_PORTABILITY:         return "PORTABILITY";
				case GL_DEBUG_TYPE_PERFORMANCE:         return "PERFORMANCE";
				case GL_DEBUG_TYPE_MARKER:              return "MARKER";
				case GL_DEBUG_TYPE_OTHER:               return "OTHER";

				default: return "UNKNOWN";
			}
		}();

		auto severity_str = [severity]() -> const char*
		{
			switch (severity) 
			{
				case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
				case GL_DEBUG_SEVERITY_LOW:          return "LOW";
				case GL_DEBUG_SEVERITY_MEDIUM:       return "MEDIUM";
				case GL_DEBUG_SEVERITY_HIGH:         return "HIGH";

				default: return "UNKNOWN";
			}
		}();

		printf("%s, %s, %s, %u: %s\n", source_str, type_str, severity_str, id, message);
	}, nullptr);
#endif
}