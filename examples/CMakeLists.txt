set(EXAMPLE_TARGET example)

set(_example_src_files
	src/Texture.cpp
	src/Vertex.cpp
	#src/heightmap_demo.cpp
	#src/path_demo.cpp
	src/dune_demo.cpp
	src/main.cpp
)

set(_example_hdr_files
	src/Texture.hpp
	src/Vertex.hpp
)

add_executable(${EXAMPLE_TARGET} ${_example_src_files} ${_example_hdr_files})

target_compile_features(${EXAMPLE_TARGET} PUBLIC cxx_std_17)

target_compile_definitions(${EXAMPLE_TARGET} PRIVATE RENDERLIB_STATIC)

target_link_libraries(${EXAMPLE_TARGET} PRIVATE
	renderlib
	glfw 
	glm
)

target_include_directories(${EXAMPLE_TARGET} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_all_files})
	set_target_properties(${EXAMPLE_TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>)
endif()

add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>/res VERBATIM
)