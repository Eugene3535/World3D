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

add_custom_command(TARGET ${MAIN_APP_TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/res"     "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/res"
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/res/shaders"
	VERBATIM
)

if(WIN32)
	if(MSVC)
		set(EXTERNAL_BIN_DIR "${EXTERNAL_SOURCE_DIR}/install/msvc/bin")

		add_custom_command(TARGET ${MAIN_APP_TARGET_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/cglm-0$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/cglm-0$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/glfw3$<$<CONFIG:Debug>:d>.dll"  "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/glfw3$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/spdlog$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/spdlog$<$<CONFIG:Debug>:d>.dll"
			VERBATIM
		)
	elseif(MINGW)
		set(EXTERNAL_BIN_DIR "${EXTERNAL_SOURCE_DIR}/install/mingw/bin")

		add_custom_command(TARGET ${MAIN_APP_TARGET_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/libcglm-0$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/libcglm-0$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/glfw3$<$<CONFIG:Debug>:d>.dll"     "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/glfw3$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/libspdlog$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/libspdlog$<$<CONFIG:Debug>:d>.dll"
			VERBATIM
		)
	endif()
endif()