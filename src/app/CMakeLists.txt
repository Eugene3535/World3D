set(MAIN_APP_TARGET_NAME star_dust)

set(MAIN_APP_SRC_FILES
	camera/Camera.cpp
	Application.cpp
	main.cpp
)

set(MAIN_APP_HDR_FILES
	camera/Camera.hpp
	Application.hpp
)

add_executable(${MAIN_APP_TARGET_NAME} ${MAIN_APP_SRC_FILES} ${MAIN_APP_HDR_FILES})

target_include_directories(${MAIN_APP_TARGET_NAME} PRIVATE
	${Vulkan_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${MAIN_APP_TARGET_NAME} PRIVATE
	vulkan_api
	${Vulkan_LIBRARIES}
	glfw
	cglm
	$<$<BOOL:${UNIX}>:xcb>
	$<$<BOOL:${UNIX}>:m>
)

target_compile_definitions(${MAIN_APP_TARGET_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
	CGLM_USE_ANONYMOUS_STRUCT
	$<$<BOOL:${WIN32}>:VK_USE_PLATFORM_WIN32_KHR>
	$<$<BOOL:${WIN32}>:GLFW_EXPOSE_NATIVE_WIN32>
	$<$<BOOL:${UNIX}>:VK_USE_PLATFORM_XCB_KHR>
	$<$<BOOL:${UNIX}>:GLFW_EXPOSE_NATIVE_X11>
)

if(MSVC)
    target_compile_options(${MAIN_APP_TARGET_NAME} PRIVATE /GR-)
else()
    target_compile_options(${MAIN_APP_TARGET_NAME} PRIVATE -fno-rtti)  
endif()

target_compile_definitions(${MAIN_APP_TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG> CGLM_USE_ANONYMOUS_STRUCT)
target_compile_features(${MAIN_APP_TARGET_NAME} PUBLIC cxx_std_20)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MAIN_APP_SRC_FILES} ${MAIN_APP_HDR_FILES})
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${MAIN_APP_TARGET_NAME})	
	set_target_properties(${MAIN_APP_TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
endif()

add_custom_command(TARGET ${MAIN_APP_TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/res"     "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/res"
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/res/shaders"
	VERBATIM
)