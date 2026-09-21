set(MAIN_APP_TARGET_NAME main_app)

file(GLOB_RECURSE APP_SRC_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_executable(${MAIN_APP_TARGET_NAME}
	${APP_SRC_FILES}
)

source_group("main_app" FILES 
	${APP_SRC_FILES}
)

target_link_libraries(${MAIN_APP_TARGET_NAME} PRIVATE
	glfw
	opengl_api
	vulkan_api
)

target_include_directories(${MAIN_APP_TARGET_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${MAIN_APP_TARGET_NAME} PRIVATE
	$<$<BOOL:${WIN32}>:GLFW_EXPOSE_NATIVE_WIN32>
	$<$<BOOL:${LINIX}>:GLFW_EXPOSE_NATIVE_X11>
)


if(MSVC)
    target_compile_options(${MAIN_APP_TARGET_NAME} PRIVATE /GR-)
else()
    target_compile_options(${MAIN_APP_TARGET_NAME} PRIVATE -fno-rtti)  
endif()

target_compile_features(${MAIN_APP_TARGET_NAME} PUBLIC cxx_std_20)

add_custom_command(TARGET ${MAIN_APP_TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/res" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/res"
	VERBATIM
)

if(WIN32)
	if(MSVC)
		set(EXTERNAL_BIN_DIR "${EXTERNAL_SOURCE_DIR}/install/msvc")

		add_custom_command(TARGET ${MAIN_APP_TARGET_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/cglm/bin/cglm-0$<$<CONFIG:Debug>:d>.dll"   "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/cglm-0$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/glfw3/bin/glfw3$<$<CONFIG:Debug>:d>.dll"   "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/glfw3$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/spdlog/bin/spdlog$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/spdlog$<$<CONFIG:Debug>:d>.dll"
			VERBATIM
		)
	elseif(MINGW)
		set(EXTERNAL_BIN_DIR "${EXTERNAL_SOURCE_DIR}/install/mingw")

		add_custom_command(TARGET ${MAIN_APP_TARGET_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/cglm/bin/libcglm-0$<$<CONFIG:Debug>:d>.dll"   "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/libcglm-0$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/glfw3/bin/glfw3$<$<CONFIG:Debug>:d>.dll"      "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/glfw3$<$<CONFIG:Debug>:d>.dll"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/spdlog/bin/libspdlog$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/libspdlog$<$<CONFIG:Debug>:d>.dll"
			VERBATIM
		)
	endif()
endif()