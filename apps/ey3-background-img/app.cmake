# The sources of this app, listed once and read by both of its builds: the
# desktop one in CMakeLists.txt here, the Android one in android/. Only the
# entry point differs between them (desktop/main_desktop.cpp and
# android/src/main_android.cpp), so it is not in here. Paths are relative to
# this file.
set(APP_SOURCES
	src/program.cpp
	)
