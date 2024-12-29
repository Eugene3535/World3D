set(RENDERLIB_TARGET renderlib)

set(_renderlib_src_files
	src/Image.cpp
	src/Shader.cpp
)

set(_renderlib_hdr_files
	src/Image.hpp
	src/Shader.hpp
)

set(_all_renderer_files
	${_renderlib_src_files}
	${_renderlib_hdr_files}
)

add_library(${RENDERLIB_TARGET} SHARED ${_all_renderer_files})

target_compile_definitions(${RENDERLIB_TARGET} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_features(${RENDERLIB_TARGET} PRIVATE cxx_std_17)

target_link_libraries(${RENDERLIB_TARGET} PRIVATE
	glad 
#	glm
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