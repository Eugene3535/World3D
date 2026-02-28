set(MAIN_APP_TARGET_NAME star_dust)

add_executable(${MAIN_APP_TARGET_NAME} main.cpp)

target_link_libraries(${MAIN_APP_TARGET_NAME} PRIVATE
	vulkan_api
)

target_compile_definitions(${MAIN_APP_TARGET_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

if(MSVC)
    target_compile_options(${MAIN_APP_TARGET_NAME} PRIVATE /GR-)
else()
    target_compile_options(${MAIN_APP_TARGET_NAME} PRIVATE -fno-rtti)  
endif()

target_compile_features(${MAIN_APP_TARGET_NAME} PUBLIC cxx_std_20)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES main.cpp)
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${MAIN_APP_TARGET_NAME})	
	set_target_properties(${MAIN_APP_TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
endif()

add_custom_command(TARGET ${MAIN_APP_TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/res"     "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/res"
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:${MAIN_APP_TARGET_NAME}>/res/shaders"
	VERBATIM
)