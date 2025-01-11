set(APP_TARGET app)

add_executable(${APP_TARGET} main.cpp)

target_compile_features(${APP_TARGET} PUBLIC cxx_std_20)

target_link_libraries(${APP_TARGET} PRIVATE
	render_engine
	heightmap
)

target_compile_definitions(${APP_TARGET} PRIVATE RENDER_ENGINE_API_EXPORTS)

add_custom_command(TARGET ${APP_TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${APP_TARGET}>/res VERBATIM
)