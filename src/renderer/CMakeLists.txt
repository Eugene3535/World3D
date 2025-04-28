set(RENDERER_TARGET renderer)

set(RENDERER_SRC_FILES
	src/graphics/Transform2D.cpp
	src/graphics/Color.cpp
	src/files/StbImage.cpp
	src/files/Font.cpp
	src/files/FileProvider.cpp
	src/camera/perspective/PerspectiveCamera.cpp
	src/camera/orthogonal/OrthogonalCamera.cpp
	src/camera/orbit/OrbitCamera.cpp
	src/opengl/holder/GlResourceHolder.cpp
	src/opengl/resources/shaders/Shader.cpp
	src/opengl/resources/shaders/ShaderProgram.cpp
	src/opengl/resources/buffers/GlBuffer.cpp
	src/opengl/resources/buffers/VertexBufferLayout.cpp
	src/opengl/resources/buffers/VertexArrayObject.cpp
	src/opengl/resources/buffers/Texture.cpp
)

set(RENDERER_HDR_FILES
	src/Config.hpp
	src/graphics/Transform2D.hpp
	src/graphics/Color.hpp
	src/graphics/Glyph.hpp
	src/files/StbImage.hpp
	src/files/Font.hpp
	src/files/FileProvider.hpp
	src/camera/perspective/PerspectiveCamera.hpp
	src/camera/orthogonal/OrthogonalCamera.hpp
	src/camera/orbit/OrbitCamera.hpp
	src/opengl/holder/GlResourceHolder.hpp
	src/opengl/holder/GlResourceHolder.inl
	src/opengl/resources/shaders/Shader.hpp
	src/opengl/resources/shaders/ShaderProgram.hpp
	src/opengl/resources/buffers/GlBuffer.hpp
	src/opengl/resources/buffers/VertexBufferLayout.hpp
	src/opengl/resources/buffers/VertexArrayObject.hpp
	src/opengl/resources/buffers/Texture.hpp
)

set(ALL_RENDERER_FILES
	${RENDERER_SRC_FILES}
	${RENDERER_HDR_FILES}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${ALL_RENDERER_FILES})

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
	${PROJECT_SOURCE_DIR}/res/shaders/fonts/text.vert
	${PROJECT_SOURCE_DIR}/res/shaders/fonts/text.frag
)

source_group("shaders" FILES ${SHADER_FILES})

add_library(${RENDERER_TARGET} SHARED ${ALL_RENDERER_FILES})

include(GenerateExportHeader)

generate_export_header(${RENDERER_TARGET}
    BASE_NAME RENDERER
    EXPORT_MACRO_NAME RENDERER_API
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/src/Export.hpp
    STATIC_DEFINE RENDERER_STATIC_DEFINE
)

target_sources(${RENDERER_TARGET} PRIVATE ${SHADER_FILES})

target_compile_features(${RENDERER_TARGET} PRIVATE cxx_std_20)

target_link_libraries(${RENDERER_TARGET} PUBLIC
	glad
	glm
	sfml-system
	sfml-window
	freetype
)

target_include_directories(${RENDERER_TARGET} 
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
PRIVATE
	${EXTERNAL_SOURCE_DIR}/stb
)

# Image.cpp must be compiled with the -fno-strict-aliasing
# when gcc is used; otherwise saving PNGs may crash in stb_image_write
if(CMAKE_COMPILER_IS_GNUCXX)
    set_source_files_properties(src/files/Image.cpp PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
endif()