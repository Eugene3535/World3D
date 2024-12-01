cmake_minimum_required(VERSION 3.25.1)

project(World3D LANGUAGES C CXX)

set(EXTERNAL_DIR $ENV{external})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

if(WIN32 AND MSVC)
	string(APPEND CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
	string(APPEND CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib")
	string(APPEND CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")
else()
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if(MINGW)
    add_definitions(-Wno-error)
	add_compile_options(-Wno-error=dangling-reference)
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

set(_src_files
	src/Image.cpp
	src/Texture.cpp
	src/ShaderProgram.cpp
	src/main.cpp
)

set(_hdr_files
	src/Image.hpp
	src/Texture.hpp
	src/ShaderProgram.hpp
	src/Vertex.hpp
)

set(_all_files
	${_src_files}
	${_hdr_files}
)

add_executable(${PROJECT_NAME} ${_src_files} ${_hdr_files})

add_subdirectory(${EXTERNAL_DIR}/glfw glfw)
add_subdirectory(${EXTERNAL_DIR}/glad/core glad)
add_subdirectory(${EXTERNAL_DIR}/glm glm)
#add_subdirectory(${EXTERNAL_DIR}/assimp assimp)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_link_libraries(${PROJECT_NAME} PRIVATE
	glfw 
	glad 
	glm
#	assimp
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${EXTERNAL_DIR}/stb
)

if(MSVC)
	set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_all_files})
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res VERBATIM
)

if(WIN32)
	if(MSVC)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res VERBATIM
		)
	endif()
endif()