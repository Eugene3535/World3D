set(SHINY_TARGET shiny)

set(SHINY_SRC_FILES
	graphics/Color.cpp
	graphics/Transform2D.cpp
	files/FileProvider.cpp
	files/Font.cpp
	files/Shader.cpp
	files/StbImage.cpp
	camera/perspective/PerspectiveCamera.cpp
	camera/orthogonal/OrthogonalCamera.cpp
	camera/orbit/OrbitCamera.cpp
	resources/holder/GlResourceHolder.cpp
	resources/shaders/ShaderProgram.cpp
	resources/buffers/GlBuffer.cpp
	resources/buffers/VertexBufferLayout.cpp
	resources/buffers/VertexArrayObject.cpp
	resources/buffers/Texture.cpp
)

set(SHINY_HDR_FILES
	Export.hpp
	graphics/Color.hpp
	graphics/Glyph.hpp
	graphics/Transform2D.hpp
	files/FileProvider.hpp
	files/Font.hpp
	files/Shader.hpp
	files/StbImage.hpp
	camera/perspective/PerspectiveCamera.hpp
	camera/orthogonal/OrthogonalCamera.hpp
	camera/orbit/OrbitCamera.hpp
	resources/holder/GlResourceHolder.hpp
	resources/holder/GlResourceHolder.inl
	resources/shaders/ShaderProgram.hpp
	resources/buffers/GlBuffer.hpp
	resources/buffers/VertexBufferLayout.hpp
	resources/buffers/VertexArrayObject.hpp
	resources/buffers/Texture.hpp
	scenes/DemoScene.hpp
	scenes/DemoScene.cpp
	scenes/SceneManager.hpp
	scenes/SceneManager.inl
	scenes/SceneManager.cpp
)

set(ALL_SHINY_FILES
	${SHINY_SRC_FILES}
	${SHINY_HDR_FILES}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_SHINY_FILES})

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

add_library(${SHINY_TARGET} SHARED ${ALL_SHINY_FILES})

include(GenerateExportHeader)

generate_export_header(${SHINY_TARGET}
    BASE_NAME SHINY
    EXPORT_MACRO_NAME SHINY_API
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Export.hpp
    STATIC_DEFINE SHINY_STATIC_DEFINE
)

target_sources(${SHINY_TARGET} PRIVATE ${SHADER_FILES})

target_compile_features(${SHINY_TARGET} PRIVATE cxx_std_20)

target_link_libraries(${SHINY_TARGET} PUBLIC
	glad
	glm
	sfml-system
	sfml-window
	freetype
	utf8cpp
)

target_include_directories(${SHINY_TARGET} 
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
PRIVATE
	${EXTERNAL_SOURCE_DIR}/stb
)