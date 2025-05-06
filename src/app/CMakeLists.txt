set(APP_TARGET app)

set(DUNE_SCENE_SRC_FILES
	scenes/dune/DuneDemo.cpp
	scenes/dune/DuneDemo.hpp
)

set(HEIGHTMAP_SCENE_SRC_FILES
	scenes/heightmap/HeightmapDemo.cpp
	scenes/heightmap/HeightmapDemo.hpp
)

set(PATHS_SCENE_SRC_FILES
	scenes/paths/PathDemo.cpp
	scenes/paths/PathDemo.hpp
)

set(PLATFORMER_SCENE_SRC_FILES
	scenes/platformer/PlatformerDemo.cpp
	scenes/platformer/PlatformerDemo.hpp
	scenes/platformer/sprites/Sprite2D.hpp
	scenes/platformer/sprites/SpriteHolder.cpp
	scenes/platformer/sprites/SpriteHolder.hpp
	scenes/platformer/animation/Animator.cpp
	scenes/platformer/animation/Animator.hpp
	scenes/platformer/tilemap/TileMap.cpp
	scenes/platformer/tilemap/TileMap.hpp
	scenes/platformer/entities/Entity.hpp
	scenes/platformer/entities/Goomba.hpp
	scenes/platformer/entities/Bullet.hpp
	scenes/platformer/entities/MovingPlatform.hpp
	scenes/platformer/entities/Player.hpp
)

set(ORBIT_SCENE_SRC_FILES
	scenes/orbit/OrbitDemo.cpp
	scenes/orbit/OrbitDemo.hpp
)

set(FONTS_SCENE_SRC_FILES
	scenes/fonts/FontDemo.cpp
	scenes/fonts/FontDemo.cpp
)

set(MAIN_APP_SRC_FILES
	# ${DUNE_SCENE_SRC_FILES}
	# ${HEIGHTMAP_SCENE_SRC_FILES}
	${PATHS_SCENE_SRC_FILES}
	# ${PLATFORMER_SCENE_SRC_FILES}
	${ORBIT_SCENE_SRC_FILES}
	# ${FONTS_SCENE_SRC_FILES}
	main.cpp
)

add_executable(${APP_TARGET}
	${MAIN_APP_SRC_FILES}
)

target_compile_features(${APP_TARGET} PUBLIC cxx_std_20)

target_link_libraries(${APP_TARGET} PRIVATE
	shiny
	sfml-system
	sfml-window
)

target_include_directories(${APP_TARGET} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${EXTERNAL_SOURCE_DIR}/rapidxml
	${EXTERNAL_SOURCE_DIR}/stb
)

add_custom_command(TARGET ${APP_TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${APP_TARGET}>/res VERBATIM
)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
	add_custom_command(TARGET ${APP_TARGET} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res VERBATIM
	)
endif()

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${MAIN_APP_SRC_FILES})
endif()