cmake_minimum_required(VERSION 4.0.0)

project(World3D LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

if(NOT DEFINED ENV{External})
    message(FATAL_ERROR "Environment variable External is not set")
endif()

set(EXTERNAL_SOURCE_DIR $ENV{External})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

if(MSVC)
	string(APPEND CMAKE_RUNTIME_OUTPUT_DIRECTORY "/bin")
	string(APPEND CMAKE_LIBRARY_OUTPUT_DIRECTORY "/lib")
	string(APPEND CMAKE_ARCHIVE_OUTPUT_DIRECTORY "/lib")
else()
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build using shared libraries")
endif()

set(LIB_TYPE_FOLDER "static")

if(BUILD_SHARED_LIBS)
	set(LIB_TYPE_FOLDER "shared")
endif()

if(WIN32)
    if(MSVC)
        set(EXTERNAL_INSTALL "${EXTERNAL_SOURCE_DIR}/install/msvc")
    elseif(MINGW)
        set(EXTERNAL_INSTALL "${EXTERNAL_SOURCE_DIR}/install/mingw")
    endif()

    foreach(lib glfw3 cglm magic_enum spdlog)
        list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_INSTALL}/${LIB_TYPE_FOLDER}/${lib}")
    endforeach()
endif()

if(LINUX)
	set(EXTERNAL_INSTALL "${EXTERNAL_SOURCE_DIR}/install/linux")
	
	foreach(lib glfw3 cglm magic_enum spdlog)
        list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_INSTALL}/${LIB_TYPE_FOLDER}/${lib}")
    endforeach()
endif()

foreach(lib glfw3 cglm magic_enum spdlog)
	find_package(${lib} CONFIG REQUIRED)
endforeach()

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
include(${CMAKE_SOURCE_DIR}/cmake/compile_shaders.cmake)
compile_shaders(${CMAKE_CURRENT_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/shaders)

if (MSVC)
    add_compile_options(/utf-8)
endif()

file(GLOB_RECURSE APP_SRC_FILES CONFIGURE_DEPENDS
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${CMAKE_SOURCE_DIR}/src/*.inl
	${CMAKE_SOURCE_DIR}/src/*.hpp
)

file(GLOB SHADER_FILES
	${CMAKE_SOURCE_DIR}/src/shaders/*.vert
	${CMAKE_SOURCE_DIR}/src/shaders/*.frag
	${CMAKE_SOURCE_DIR}/src/shaders/*.geom
)

add_executable(${PROJECT_NAME}
	${APP_SRC_FILES}
)

target_sources(${PROJECT_NAME} PRIVATE ${SHADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${APP_SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
	cglm::cglm
	glfw
	$<$<BOOL:${UNIX}>:xcb>
	${Vulkan_LIBRARIES}
	spdlog::spdlog
	magic_enum::magic_enum
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${EXTERNAL_SOURCE_DIR}/stb"
	"${Vulkan_INCLUDE_DIRS}"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	$<$<BOOL:${WIN32}>:GLFW_EXPOSE_NATIVE_WIN32>
	$<$<BOOL:${WIN32}>:VK_USE_PLATFORM_WIN32_KHR>
	$<$<BOOL:${LINIX}>:GLFW_EXPOSE_NATIVE_X11>
	$<$<CONFIG:Debug>:DEBUG>
	CGLM_USE_ANONYMOUS_STRUCT
)

if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
endif()


if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /GR-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-rtti)  
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/res" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res/shaders"
	VERBATIM
)

if(WIN32 AND BUILD_SHARED_LIBS)
	if(MSVC)
		set(EXTERNAL_BIN_DIR "${EXTERNAL_SOURCE_DIR}/install/msvc")

		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/cglm/bin/cglm-0$<$<CONFIG:Debug>:d>.dll"   "$<TARGET_FILE_DIR:${PROJECT_NAME}>/cglm-0$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/glfw3/bin/glfw3$<$<CONFIG:Debug>:d>.dll"   "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/spdlog/bin/spdlog$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/spdlog$<$<CONFIG:Debug>:d>.dll"
			VERBATIM
		)
	elseif(MINGW)
		set(EXTERNAL_BIN_DIR "${EXTERNAL_SOURCE_DIR}/install/mingw")

		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/cglm/bin/libcglm-0$<$<CONFIG:Debug>:d>.dll"   "$<TARGET_FILE_DIR:${PROJECT_NAME}>/libcglm-0$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/glfw3/bin/glfw3$<$<CONFIG:Debug>:d>.dll"      "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/spdlog/bin/libspdlog$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/libspdlog$<$<CONFIG:Debug>:d>.dll"
			VERBATIM
		)
	endif()
endif()


# clang-format
# Running as: cmake --build build/<config> --target format
find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)

if(CLANG_FORMAT_EXECUTABLE)
	file(GLOB_RECURSE ALL_SOURCE_FILES
			"${CMAKE_SOURCE_DIR}/src/*.cpp"
			"${CMAKE_SOURCE_DIR}/src/*.hpp"
		)

    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${ALL_SOURCE_FILES}
        COMMENT "Running clang-format on all source files"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src
        VERBATIM
    )

else()
    message(WARNING "clang-format not found. 'format' target will not be available.")
endif()