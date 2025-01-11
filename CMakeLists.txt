cmake_minimum_required(VERSION 3.25.1)

project(World3D LANGUAGES C CXX)

set(EXTERNAL_DIR $ENV{External})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

if(WIN32 AND MSVC)
	string(APPEND CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
	string(APPEND CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib")
	string(APPEND CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")
else()
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

if(MINGW)
    add_definitions(-Wno-error)
	add_compile_options(-Wno-error=dangling-reference)
endif()

add_subdirectory(${EXTERNAL_DIR}/glfw glfw)
add_subdirectory(${EXTERNAL_DIR}/glad/core glad)
add_subdirectory(${EXTERNAL_DIR}/glm glm)

add_subdirectory(${PROJECT_SOURCE_DIR}/render_engine)
add_subdirectory(${PROJECT_SOURCE_DIR}/scenes)
add_subdirectory(${PROJECT_SOURCE_DIR}/app)

if(MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT examples)
endif()