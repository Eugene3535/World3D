set(MAIN_APP_TARGET_NAME main_app)

add_executable(${MAIN_APP_TARGET_NAME}
	MainWindow.cpp
	MainWindow.hpp
	main.cpp
)

source_group("main_app" FILES 
	MainWindow.cpp
	MainWindow.hpp
	main.cpp
)

target_link_libraries(${MAIN_APP_TARGET_NAME} PRIVATE
	glfw
	vulkan_api
)

target_compile_definitions(${MAIN_APP_TARGET_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
	$<$<BOOL:${WIN32}>:GLFW_EXPOSE_NATIVE_WIN32>
	$<$<BOOL:${UNIX}>:GLFW_EXPOSE_NATIVE_X11>
)


if(MSVC)
    target_compile_options(${MAIN_APP_TARGET_NAME} PRIVATE /GR-)
else()
    target_compile_options(${MAIN_APP_TARGET_NAME} PRIVATE -fno-rtti)  
endif()

target_compile_features(${MAIN_APP_TARGET_NAME} PUBLIC cxx_std_20)

# if(CMAKE_GENERATOR MATCHES "Visual Studio")
# 	set_target_properties(${MAIN_APP_TARGET_NAME} PROPERTIES 
# 		VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>"
# 		VS_DEBUGGER_COMMAND           "$<TARGET_FILE:${MAIN_APP_TARGET_NAME}>"
# 		VS_DEBUGGER_ENVIRONMENT       "PATH=%PATH%;${CMAKE_PREFIX_PATH}/bin"
# 	)
# endif()

add_custom_command(TARGET ${MAIN_APP_TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/res"     "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/res"
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/res/shaders"
	VERBATIM
)