set(OPENGL_API_TARGET_NAME opengl_api)

find_package(glad CONFIG REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

file(GLOB SHADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.vert
	${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.frag
	${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.geom
)

if(BUILD_SHARED_LIBS)
	add_library(${OPENGL_API_TARGET_NAME} SHARED ${SRC_FILES})
else()
	add_library(${OPENGL_API_TARGET_NAME} STATIC ${SRC_FILES})
endif()

include(GenerateExportHeader)

generate_export_header(${OPENGL_API_TARGET_NAME}
    BASE_NAME OPENGL_API
    EXPORT_MACRO_NAME OGL_API
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/OpenGLApiExport.hpp
    STATIC_DEFINE OGL_API_STATIC_DEFINE
)

target_sources(${OPENGL_API_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/OpenGLApiExport.hpp)
target_sources(${OPENGL_API_TARGET_NAME} PRIVATE ${SHADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SRC_FILES})

target_link_libraries(${OPENGL_API_TARGET_NAME} 
PUBLIC
	graphics_api
PRIVATE
	glad::glad
)

target_include_directories(${OPENGL_API_TARGET_NAME}
PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_BINARY_DIR}"
PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(MSVC)
    target_compile_options(${OPENGL_API_TARGET_NAME} PRIVATE /GR-)
else()
    target_compile_options(${OPENGL_API_TARGET_NAME} PRIVATE -fno-rtti)  
endif()

target_compile_features(${OPENGL_API_TARGET_NAME} PUBLIC cxx_std_20)

add_custom_command(TARGET ${OPENGL_API_TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders" "$<TARGET_FILE_DIR:${OPENGL_API_TARGET_NAME}>/res/shaders/opengl"
	VERBATIM
)

if(WIN32)
	if(MSVC)
		set(EXTERNAL_BIN_DIR "${EXTERNAL_SOURCE_DIR}/install/msvc")

		add_custom_command(TARGET ${OPENGL_API_TARGET_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/glad/bin/glad$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${OPENGL_API_TARGET_NAME}>/glad$<$<CONFIG:Debug>:d>.dll"
			VERBATIM
		)
	elseif(MINGW)
		set(EXTERNAL_BIN_DIR "${EXTERNAL_SOURCE_DIR}/install/mingw")

		add_custom_command(TARGET ${OPENGL_API_TARGET_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERNAL_BIN_DIR}/glad/bin/libglad$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:${OPENGL_API_TARGET_NAME}>/libglad$<$<CONFIG:Debug>:d>.dll"
			VERBATIM
		)
	endif()
endif()