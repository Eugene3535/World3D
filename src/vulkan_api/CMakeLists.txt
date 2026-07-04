set(VULKAN_API_TARGET_NAME vulkan_api)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
include(${CMAKE_SOURCE_DIR}/src/cmake/compile_shaders.cmake)
compile_shaders(${CMAKE_CURRENT_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/shaders)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

set(SHADER_FILES
	src/shaders/vertex_shader.vert
	src/shaders/fragment_shader.frag
)

source_group("shaders" FILES ${SHADER_FILES})

if(BUILD_SHARED_LIBS)
	add_library(${VULKAN_API_TARGET_NAME} SHARED ${SRC_FILES} ${SHADER_FILES})
else()
	add_library(${VULKAN_API_TARGET_NAME} STATIC ${SRC_FILES} ${SHADER_FILES})
endif()

include(GenerateExportHeader)

generate_export_header(${VULKAN_API_TARGET_NAME}
    BASE_NAME VULKAN_API
    EXPORT_MACRO_NAME VK_API
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/Export.hpp
    STATIC_DEFINE VK_API_STATIC_DEFINE
)

target_include_directories(${VULKAN_API_TARGET_NAME}
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
	${Vulkan_INCLUDE_DIRS}
	${EXTERNAL_SOURCE_DIR}/stb
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${VULKAN_API_TARGET_NAME} PRIVATE
	$<$<BOOL:${UNIX}>:xcb>
	${Vulkan_LIBRARIES}
	cglm
	spdlog
	magic_enum::magic_enum
)

target_compile_definitions(${VULKAN_API_TARGET_NAME} PRIVATE
	MAX_FRAMES_IN_FLIGHT=2
    $<$<CONFIG:Debug>:DEBUG>
	CGLM_USE_ANONYMOUS_STRUCT
	$<$<BOOL:${WIN32}>:VK_USE_PLATFORM_WIN32_KHR>
	$<$<BOOL:${UNIX}>:VK_USE_PLATFORM_XCB_KHR>
)

if(MSVC)
    target_compile_options(${VULKAN_API_TARGET_NAME} PRIVATE /GR-)
else()
    target_compile_options(${VULKAN_API_TARGET_NAME} PRIVATE -fno-rtti)  
endif()

target_compile_features(${VULKAN_API_TARGET_NAME} PUBLIC cxx_std_20)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES} ${HDR_FILES} ${SHADER_FILES})