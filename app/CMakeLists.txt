set(APP_TARGET app)

set(_src_files
	src/scenes/heightmap/Heightmap.cpp
	src/scenes/paths/path_demo.cpp
	src/scenes/dune/dune_demo.cpp
	src/main.cpp
)

source_group("" FILES ${_src_files})
source_group("heightmap" FILES src/scenes/heightmap/Heightmap.cpp)
source_group("paths" FILES src/scenes/paths/path_demo.cpp)
source_group("dune" FILES src/scenes/dune/dune_demo.cpp)

add_executable(${APP_TARGET}
	${_src_files}
)

target_compile_definitions(${APP_TARGET} PUBLIC "SFML_STATIC")

target_compile_features(${APP_TARGET} PUBLIC cxx_std_20)

target_link_libraries(${APP_TARGET} PRIVATE
	opengl_utils
	sfml-window
)

add_custom_command(TARGET ${APP_TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${APP_TARGET}>/res VERBATIM
)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
	add_custom_command(TARGET ${APP_TARGET} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res VERBATIM
	)
endif()