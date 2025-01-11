set(RENDER_ENGINE_TARGET render_engine)

set(_render_engine_src_files
	src/Image.cpp
	src/Shader.cpp
	src/ShaderProgram.cpp
	src/VertexBuffer.cpp
	src/IndexBuffer.cpp
	src/VertexArray.cpp
	src/Texture2D.cpp
	src/OpenGLDebugger.cpp
	src/RenderWindow.cpp
	src/Camera.cpp
	src/Scene.cpp
)

set(_render_engine_hdr_files
	src/Config.hpp
	src/Export.hpp
	src/Image.hpp
	src/Shader.hpp
	src/ShaderProgram.hpp
	src/VertexBuffer.hpp
	src/IndexBuffer.hpp
	src/VertexArray.hpp
	src/Texture2D.hpp
	src/OpenGLDebugger.hpp
	src/RenderWindow.hpp
	src/Camera.hpp
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
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_all_files})
endif()