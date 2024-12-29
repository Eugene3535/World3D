set(EXAMPLE_TARGET example)

set(_example_src_files
	src/Texture.cpp
	src/ShaderProgram.cpp
	src/Vertex.cpp
	src/heightmap_demo.cpp
	src/path_demo.cpp
	src/dune_demo.cpp
	src/main.cpp
)

set(_example_hdr_files
	src/Texture.hpp
	src/ShaderProgram.hpp
	src/Vertex.hpp
)

add_executable(${EXAMPLE_TARGET} ${_example_src_files} ${_example_hdr_files})

target_compile_definitions(${EXAMPLE_TARGET} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_features(${EXAMPLE_TARGET} PUBLIC cxx_std_17)

target_link_libraries(${EXAMPLE_TARGET} PRIVATE
	glfw 
	glad 
	glm
	renderlib
)

target_include_directories(${EXAMPLE_TARGET} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_all_files})
endif()

add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>/res VERBATIM
)

if(MSVC)
	add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res VERBATIM
	)
endif()