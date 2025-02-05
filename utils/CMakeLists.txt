set(OPENGL_UTILS_TARGET opengl_utils)

set(_opengl_utils_src_files
	src/graphics/Transform2D.cpp
	src/files/Image.cpp
	src/camera/perspective/Perspective.cpp
	src/camera/orthogonal/Orthogonal.cpp
	src/opengl/context/GlContext.cpp
	src/opengl/resources/shaders/Shader.cpp
	src/opengl/resources/shaders/ShaderProgram.cpp
	src/opengl/resources/GlResource.cpp
	src/opengl/resources/buffers/GlBuffer.cpp
	src/opengl/resources/buffers/VertexBuffer.cpp
	src/opengl/resources/buffers/IndexBuffer.cpp
	src/opengl/resources/buffers/UniformBuffer.cpp
	src/opengl/resources/graphics/VertexArrayObject.cpp
	src/opengl/resources/graphics/Texture2D.cpp
	src/opengl/debug/OpenGLDebugger.cpp
	src/window/BaseWindow.cpp
	src/window/RenderWindow.cpp
	src/Scene.cpp
)

set(_opengl_utils_hdr_files
	src/Config.hpp
	src/Export.hpp
	src/graphics/Transform2D.hpp
	src/files/Image.hpp
	src/camera/perspective/Perspective.hpp
	src/camera/orthogonal/Orthogonal.hpp
	src/opengl/context/GlContext.hpp
	src/opengl/context/GlContext.inl
	src/opengl/resources/shaders/Shader.hpp
	src/opengl/resources/shaders/ShaderProgram.hpp
	src/opengl/resources/GlResource.hpp
	src/opengl/resources/buffers/GlBuffer.hpp
	src/opengl/resources/buffers/VertexBuffer.hpp
	src/opengl/resources/buffers/IndexBuffer.hpp
	src/opengl/resources/buffers/UniformBuffer.hpp
	src/opengl/resources/graphics/VertexArrayObject.hpp
	src/opengl/resources/graphics/Texture2D.hpp
	src/opengl/debug/OpenGLDebugger.hpp
	src/window/Input.hpp
	src/window/BaseWindow.hpp
	src/window/RenderWindow.hpp
	src/Scene.hpp
)

set(_all_opengl_utils_files
	${_opengl_utils_src_files}
	${_opengl_utils_hdr_files}
)

add_library(${OPENGL_UTILS_TARGET} SHARED ${_all_opengl_utils_files})

target_compile_definitions(${OPENGL_UTILS_TARGET} PRIVATE OPENGL_UTILS_API_EXPORTS)

target_compile_features(${OPENGL_UTILS_TARGET} PRIVATE cxx_std_20)

target_link_libraries(${OPENGL_UTILS_TARGET} PRIVATE
	glad 
	glfw
)

target_include_directories(${OPENGL_UTILS_TARGET} 
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${EXTERNAL_SOURCE_DIR}/glm
PRIVATE
	${EXTERNAL_SOURCE_DIR}/stb
)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_all_opengl_utils_files})
endif()