set(RENDERLIB_TARGET renderlib)

set(_renderlib_src_files
	src/Image.cpp
	src/Shader.cpp
	src/ShaderProgram.cpp
	src/VertexBuffer.cpp
)

set(_renderlib_hdr_files
	src/Export.hpp
	src/Image.hpp
	src/Shader.hpp
	src/ShaderProgram.hpp
	src/VertexBuffer.hpp
)

set(_all_renderlib_files
	${_renderlib_src_files}
	${_renderlib_hdr_files}
)

if(RENDERLIB_BUILD_STATIC)
	add_library(${RENDERLIB_TARGET} STATIC ${_all_renderlib_files})
else()
	add_library(${RENDERLIB_TARGET} SHARED ${_all_renderlib_files})
endif()

if(RENDERLIB_BUILD_STATIC)
	target_compile_definitions(${RENDERLIB_TARGET} PRIVATE RENDERLIB_STATIC)
else()
	target_compile_definitions(${RENDERLIB_TARGET} PRIVATE RENDERLIB_API_EXPORTS)
endif()

target_compile_features(${RENDERLIB_TARGET} PRIVATE cxx_std_17)

target_link_libraries(${RENDERLIB_TARGET} PUBLIC
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