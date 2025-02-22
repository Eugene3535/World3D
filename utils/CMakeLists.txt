set(OPENGL_UTILS_TARGET opengl_utils)

set(_opengl_utils_src_files
	src/graphics/Transform2D.cpp
	src/files/Image.cpp
	src/files/FileProvider.cpp
	src/camera/perspective/Perspective.cpp
	src/camera/orthogonal/Orthogonal.cpp
	src/opengl/holder/GlResourceHolder.cpp
	src/opengl/resources/shaders/Shader.cpp
	src/opengl/resources/shaders/ShaderProgram.cpp
	src/opengl/resources/buffers/GlBuffer.cpp
	src/opengl/resources/buffers/VertexBufferLayout.cpp
	src/opengl/resources/graphics/VertexArrayObject.cpp
	src/opengl/resources/graphics/Texture2D.cpp
	src/opengl/debug/OpenGLDebugger.cpp
)

set(_opengl_utils_hdr_files
	src/Config.hpp
	src/graphics/Transform2D.hpp
	src/files/Image.hpp
	src/files/FileProvider.hpp
	src/camera/perspective/Perspective.hpp
	src/camera/orthogonal/Orthogonal.hpp
	src/opengl/holder/GlResourceHolder.hpp
	src/opengl/holder/GlResourceHolder.inl
	src/opengl/resources/shaders/Shader.hpp
	src/opengl/resources/shaders/ShaderProgram.hpp
	src/opengl/resources/buffers/GlBuffer.hpp
	src/opengl/resources/buffers/VertexBufferLayout.hpp
	src/opengl/resources/graphics/VertexArrayObject.hpp
	src/opengl/resources/graphics/Texture2D.hpp
	src/opengl/debug/OpenGLDebugger.hpp
)

set(SHADER_FILES
	${PROJECT_SOURCE_DIR}/res/shaders/dune/dune.vert
	${PROJECT_SOURCE_DIR}/res/shaders/dune/dune.frag
	${PROJECT_SOURCE_DIR}/res/shaders/paths/ground.vert
	${PROJECT_SOURCE_DIR}/res/shaders/paths/ground.frag
	${PROJECT_SOURCE_DIR}/res/shaders/heightmap/heightmap.vert
	${PROJECT_SOURCE_DIR}/res/shaders/heightmap/heightmap.frag
	${PROJECT_SOURCE_DIR}/res/shaders/heightmap/circle.vert
	${PROJECT_SOURCE_DIR}/res/shaders/heightmap/circle.frag
	${PROJECT_SOURCE_DIR}/res/shaders/platformer/tilemap.vert
	${PROJECT_SOURCE_DIR}/res/shaders/platformer/tilemap.frag
)

source_group("shaders" FILES 
	${SHADER_FILES}
)

set(_all_opengl_utils_files
	${_opengl_utils_src_files}
	${_opengl_utils_hdr_files}
)

add_library(${OPENGL_UTILS_TARGET} STATIC ${_all_opengl_utils_files})

target_sources(${OPENGL_UTILS_TARGET} PRIVATE ${SHADER_FILES})

target_compile_definitions(${OPENGL_UTILS_TARGET} PRIVATE OPENGL_UTILS_API_EXPORTS)

target_compile_features(${OPENGL_UTILS_TARGET} PRIVATE cxx_std_20)

target_link_libraries(${OPENGL_UTILS_TARGET} PUBLIC
	glad
	glm
)

target_include_directories(${OPENGL_UTILS_TARGET} 
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
PRIVATE
	${EXTERNAL_SOURCE_DIR}/stb
)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${_all_opengl_utils_files})
endif()