# The sources of this app, listed once and read by both of its builds: the
# desktop one in CMakeLists.txt here, the Android one in android/. Only the
# entry point differs between them (desktop/main_desktop.cpp and
# android/src/main_android.cpp), so it is not in here. Paths are relative to
# this file.
set(APP_SOURCES
	src/program.cpp
	src/orbTest.cpp
	)

# More of OpenCV than the engine itself uses: ORB and the matcher live in
# features2d, findHomography in calib3d. Only the Android build needs them
# spelled out; on desktop they come with the ey3 package.
set(APP_LIBRARIES cv_features2d cv_calib3d cv_flann)
