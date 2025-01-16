set(RENDER_ENGINE_TARGET render_engine)

set(_component_src_files
	src/ecs/components/camera/Perspective.cpp
)

set(_component_hdr_files
	src/ecs/components/camera/Perspective.hpp
)

set(_render_engine_src_files
	src/Image.cpp
	src/gl_resources/GlResource.cpp
	src/gl_resources/Shader.cpp
	src/gl_resources/ShaderProgram.cpp
	src/gl_resources/buffers/GlBuffer.cpp
	src/gl_resources/buffers/VertexBuffer.cpp
	src/gl_resources/buffers/IndexBuffer.cpp
	src/gl_resources/VertexArray.cpp
	src/gl_resources/Texture2D.cpp
	src/OpenGLDebugger.cpp
	src/BaseWindow.cpp
	src/RenderWindow.cpp
	src/Scene.cpp
)

set(_render_engine_hdr_files
	src/Config.hpp
	src/Export.hpp
	src/Image.hpp
	src/gl_resources/GlResource.hpp
	src/gl_resources/Shader.hpp
	src/gl_resources/ShaderProgram.hpp
	src/gl_resources/buffers/GlBuffer.hpp
	src/gl_resources/buffers/VertexBuffer.hpp
	src/gl_resources/buffers/IndexBuffer.hpp
	src/gl_resources/VertexArray.hpp
	src/gl_resources/Texture2D.hpp
	src/OpenGLDebugger.hpp
	src/BaseWindow.hpp
	src/RenderWindow.hpp
	src/Scene.hpp
)

set(_all_render_engine_files
	${_render_engine_src_files}
	${_render_engine_hdr_files}
	${_component_src_files}
	${_component_hdr_files}
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