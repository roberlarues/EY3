#ifndef BACKGROUND_IMG_PROGRAM_H
#define BACKGROUND_IMG_PROGRAM_H

#include <ey3.h>

/**
 * App setup shared by every platform's entry point (main.cpp, main_desktop.cpp):
 * loads the background image and runs the standard loop until the engine
 * terminates.
 */
void runProgram(ey3::Engine& engine);

#endif // BACKGROUND_IMG_PROGRAM_H
