set(RENDER_ENGINE_TARGET render_engine)

set(_render_engine_src_files
	src/utils/Transform2D.cpp
	src/Image.cpp
	src/camera/perspective/Perspective.cpp
	src/camera/orthogonal/Orthogonal.cpp
	src/opengl/context/GlContext.cpp
	src/opengl/resources/unique/Shader.cpp
	src/opengl/resources/unique/ShaderProgram.cpp
	src/opengl/resources/GlResource.cpp
	src/opengl/resources/buffers/GlBuffer.cpp
	src/opengl/resources/buffers/VertexBuffer.cpp
	src/opengl/resources/buffers/IndexBuffer.cpp
	src/opengl/resources/buffers/UniformBuffer.cpp
	src/opengl/resources/shared/VertexArrayObject.cpp
	src/opengl/resources/shared/Texture2D.cpp
	src/opengl/debug/OpenGLDebugger.cpp
	src/window/BaseWindow.cpp
	src/window/RenderWindow.cpp
	src/Scene.cpp
)

set(_render_engine_hdr_files
	src/Config.hpp
	src/Export.hpp
	src/utils/Transform2D.hpp
	src/Image.hpp
	src/camera/perspective/Perspective.hpp
	src/camera/orthogonal/Orthogonal.hpp
	src/opengl/context/GlContext.hpp
	src/opengl/context/GlContext.inl
	src/opengl/resources/unique/Shader.hpp
	src/opengl/resources/unique/ShaderProgram.hpp
	src/opengl/resources/GlResource.hpp
	src/opengl/resources/buffers/GlBuffer.hpp
	src/opengl/resources/buffers/VertexBuffer.hpp
	src/opengl/resources/buffers/IndexBuffer.hpp
	src/opengl/resources/buffers/UniformBuffer.hpp
	src/opengl/resources/shared/VertexArrayObject.hpp
	src/opengl/resources/shared/Texture2D.hpp
	src/opengl/debug/OpenGLDebugger.hpp
	src/window/BaseWindow.hpp
	src/window/RenderWindow.hpp
	src/Scene.hpp
)

set(_all_render_engine_files
	${_render_engine_src_files}
	${_render_engine_hdr_files}
)

add_library(${RENDER_ENGINE_TARGET} SHARED ${_all_render_engine_files})

target_compile_definitions(${RENDER_ENGINE_TARGET} PRIVATE RENDER_ENGINE_API_EXPORTS)

target_compile_features(${RENDER_ENGINE_TARGET} PRIVATE cxx_std_20)

target_link_libraries(${RENDER_ENGINE_TARGET}
PRIVATE
	glad 
	glfw
PUBLIC
	glm
)

target_include_directories(${RENDER_ENGINE_TARGET} 
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
PRIVATE
	${EXTERNAL_DIR}/stb
)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_all_render_engine_files})
endif()