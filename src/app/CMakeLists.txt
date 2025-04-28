set(APP_TARGET app)

set(DUNE_SCENE_SRC_FILES
	src/scenes/dune/DuneDemo.cpp
	src/scenes/dune/DuneDemo.hpp
)

set(HEIGHTMAP_SCENE_SRC_FILES
	src/scenes/heightmap/HeightmapDemo.cpp
	src/scenes/heightmap/HeightmapDemo.hpp
)

set(PATHS_SCENE_SRC_FILES
	src/scenes/paths/PathDemo.cpp
	src/scenes/paths/PathDemo.hpp
)

set(PLATFORMER_SCENE_SRC_FILES
	src/scenes/platformer/PlatformerDemo.cpp
	src/scenes/platformer/PlatformerDemo.hpp
	src/scenes/platformer/sprites/Sprite2D.hpp
	src/scenes/platformer/sprites/SpriteHolder.cpp
	src/scenes/platformer/sprites/SpriteHolder.hpp
	src/scenes/platformer/animation/Animator.cpp
	src/scenes/platformer/animation/Animator.hpp
	src/scenes/platformer/tilemap/TileMap.cpp
	src/scenes/platformer/tilemap/TileMap.hpp
	src/scenes/platformer/entities/Entity.hpp
	src/scenes/platformer/entities/Goomba.hpp
	src/scenes/platformer/entities/Bullet.hpp
	src/scenes/platformer/entities/MovingPlatform.hpp
	src/scenes/platformer/entities/Player.hpp
)

set(ORBIT_SCENE_SRC_FILES
	src/scenes/orbit/OrbitDemo.cpp
	src/scenes/orbit/OrbitDemo.hpp
)

set(FONTS_SCENE_SRC_FILES
	src/scenes/fonts/FontDemo.cpp
	src/scenes/fonts/FontDemo.cpp
)

set(MAIN_APP_SRC_FILES
	${DUNE_SCENE_SRC_FILES}
	${HEIGHTMAP_SCENE_SRC_FILES}
	${PATHS_SCENE_SRC_FILES}
	${PLATFORMER_SCENE_SRC_FILES}
	${ORBIT_SCENE_SRC_FILES}
	${FONTS_SCENE_SRC_FILES}
	src/scenes/DemoScene.hpp
	src/scenes/DemoScene.cpp
	src/scenes/SceneManager.hpp
	src/scenes/SceneManager.cpp
	src/main.cpp
)

add_executable(${APP_TARGET}
	${MAIN_APP_SRC_FILES}
)

target_compile_features(${APP_TARGET} PUBLIC cxx_std_20)

target_link_libraries(${APP_TARGET} PRIVATE
	renderer
	sfml-system
	sfml-window
)

target_include_directories(${APP_TARGET} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
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