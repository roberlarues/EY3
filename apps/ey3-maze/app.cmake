# The sources of this app, listed once and read by both of its builds: the
# desktop one in CMakeLists.txt here, the Android one in android/. Only the
# entry point differs between them (desktop/main_desktop.cpp and
# android/src/main_android.cpp), so it is not in here. Paths are relative to
# this file.
set(APP_SOURCES
	src/program.cpp
	src/game/direction.cpp
	src/game/grid.cpp
	src/game/level.cpp
	src/game/level_data.cpp
	src/game/level_library.cpp
	src/game/maze_game.cpp
	src/game/scene.cpp
	src/graphics/sprite.cpp
	src/graphics/sprite_library.cpp
	src/graphics/sprite_renderer.cpp
	src/graphics/sprite_sheet.cpp
	src/objects/exit.cpp
	src/objects/floor.cpp
	src/objects/game_object.cpp
	src/objects/grid_object.cpp
	src/objects/player.cpp
	src/objects/wall.cpp
	src/ui/touch_direction_pad.cpp
	)
