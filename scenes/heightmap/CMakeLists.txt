set(HEIGHTMAP_TARGET heightmap)

if(MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(_heightmap_src_files
	src/Heightmap.cpp
)

add_library(${HEIGHTMAP_TARGET} SHARED ${_heightmap_src_files})

target_compile_features(${HEIGHTMAP_TARGET} PUBLIC cxx_std_20)

target_link_libraries(${HEIGHTMAP_TARGET} PRIVATE
	opengl_utils
	glad
)

target_include_directories(${HEIGHTMAP_TARGET} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
)