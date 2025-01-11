set(RENDERLIB_TARGET renderlib)

set(_renderlib_src_files
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

set(_renderlib_hdr_files
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

set(_all_renderlib_files
	${_renderlib_src_files}
	${_renderlib_hdr_files}
)

add_library(${RENDERLIB_TARGET} SHARED ${_all_renderlib_files})

target_compile_definitions(${RENDERLIB_TARGET} PRIVATE RENDERLIB_API_EXPORTS)

target_compile_features(${RENDERLIB_TARGET} PRIVATE cxx_std_20)

target_link_libraries(${RENDERLIB_TARGET}
PRIVATE
	glad 
	glfw
PUBLIC
	glm
)

target_include_directories(${RENDERLIB_TARGET} 
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
PRIVATE
	${EXTERNAL_DIR}/stb
)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_all_files})
endif()