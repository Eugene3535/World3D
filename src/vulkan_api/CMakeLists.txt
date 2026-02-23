set(VULKAN_API_TARGET_NAME vulkan_api)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
include(${CMAKE_SOURCE_DIR}/src/cmake/compile_shaders.cmake)
compile_shaders(${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders)

set(SRC_FILES
	utils/Tools.cpp
	context/Context.cpp
	presentation/MainView.cpp
	pipeline/stages/shader/Shader.cpp
	pipeline/stages/shader/VertexInputState.cpp
	pipeline/stages/uniform/DescriptorSetLayout.cpp
	pipeline/descriptors/DescriptorPool.cpp
	pipeline/GraphicsPipeline.cpp
	command_pool/CommandBufferPool.cpp
	sync/SyncManager.cpp
	texture/Texture2D.cpp
	buffers/BufferHolder.cpp
	render/Renderer.cpp
)

set(HDR_FILES
	Export.hpp
	utils/Tools.hpp
	context/Context.hpp
	presentation/MainView.hpp
	pipeline/stages/shader/Shader.hpp
	pipeline/stages/shader/VertexInputState.hpp
	pipeline/stages/uniform/DescriptorSetLayout.hpp
	pipeline/descriptors/DescriptorPool.hpp
	pipeline/GraphicsPipeline.hpp
	command_pool/CommandBufferPool.hpp
	sync/SyncManager.hpp
	texture/Texture2D.hpp
	buffers/BufferHolder.hpp
	render/Renderer.hpp
)

set(SHADER_FILES
	shaders/vertex_shader.vert
	shaders/fragment_shader.frag
)

source_group("shaders" FILES ${SHADER_FILES})

if(BUILD_SHARED_LIBS)
	add_library(${VULKAN_API_TARGET_NAME} SHARED ${SRC_FILES} ${HDR_FILES} ${SHADER_FILES})
else()
	add_library(${VULKAN_API_TARGET_NAME} STATIC ${SRC_FILES} ${HDR_FILES} ${SHADER_FILES})
endif()

include(GenerateExportHeader)

generate_export_header(${VULKAN_API_TARGET_NAME}
    BASE_NAME VULKAN_API
    EXPORT_MACRO_NAME VK_API
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Export.hpp
    STATIC_DEFINE VK_API_STATIC_DEFINE
)

target_include_directories(${VULKAN_API_TARGET_NAME} PUBLIC
	${Vulkan_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/src
	${EXTERNAL_SOURCE_DIR}/stb
)

target_link_libraries(${VULKAN_API_TARGET_NAME} PUBLIC
	${Vulkan_LIBRARIES}
	glfw
	cglm
	$<$<BOOL:${UNIX}>:xcb>
	$<$<BOOL:${UNIX}>:m>
)

target_compile_definitions(${VULKAN_API_TARGET_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
	CGLM_USE_ANONYMOUS_STRUCT
	$<$<BOOL:${WIN32}>:VK_USE_PLATFORM_WIN32_KHR>
	$<$<BOOL:${WIN32}>:GLFW_EXPOSE_NATIVE_WIN32>
	$<$<BOOL:${UNIX}>:VK_USE_PLATFORM_XCB_KHR>
	$<$<BOOL:${UNIX}>:GLFW_EXPOSE_NATIVE_X11>
)

if(MSVC)
    target_compile_options(${VULKAN_API_TARGET_NAME} PRIVATE /GR-)
else()
    target_compile_options(${VULKAN_API_TARGET_NAME} PRIVATE -fno-rtti)  
endif()

target_compile_definitions(${VULKAN_API_TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG> CGLM_USE_ANONYMOUS_STRUCT)
target_compile_features(${VULKAN_API_TARGET_NAME} PUBLIC cxx_std_20)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES} ${HDR_FILES} ${SHADER_FILES})
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${VULKAN_API_TARGET_NAME})	
	set_target_properties(${VULKAN_API_TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
endif()