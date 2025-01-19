set(RENDER_ENGINE_TARGET render_engine)

set(_component_src_files
	src/ecs/components/camera/Perspective.cpp
)

set(_component_hdr_files
	src/ecs/components/camera/Perspective.hpp
)

set(_render_engine_src_files
	src/Image.cpp
	src/opengl/resources/unique/Shader.cpp
	src/opengl/resources/unique/ShaderProgram.cpp
	src/opengl/resources/GlResource.cpp
	src/opengl/resources/GlResourceHolder.cpp
	src/opengl/resources/buffers/GlBuffer.cpp
	src/opengl/resources/buffers/VertexBuffer.cpp
	src/opengl/resources/buffers/IndexBuffer.cpp
	src/opengl/resources/buffers/UniformBuffer.cpp
	src/opengl/resources/shared/VertexArray.cpp
	src/opengl/resources/shared/Texture2D.cpp
	src/OpenGLDebugger.cpp
	src/window/BaseWindow.cpp
	src/window/RenderWindow.cpp
	src/Scene.cpp
)

set(_render_engine_hdr_files
	src/Config.hpp
	src/Export.hpp
	src/Image.hpp
	src/opengl/resources/unique/Shader.hpp
	src/opengl/resources/unique/ShaderProgram.hpp
	src/opengl/resources/GlResource.hpp
	src/opengl/resources/GlResourceHolder.hpp
	src/opengl/resources/GlResourceHolder.inl
	src/opengl/resources/buffers/GlBuffer.hpp
	src/opengl/resources/buffers/VertexBuffer.hpp
	src/opengl/resources/buffers/IndexBuffer.hpp
	src/opengl/resources/buffers/UniformBuffer.hpp
	src/opengl/resources/shared/VertexArray.hpp
	src/opengl/resources/shared/Texture2D.hpp
	src/OpenGLDebugger.hpp
	src/window/BaseWindow.hpp
	src/window/RenderWindow.hpp
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