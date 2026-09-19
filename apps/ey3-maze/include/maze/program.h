#ifndef MAZE_PROGRAM_H
#define MAZE_PROGRAM_H

#include <ey3.h>

/**
 * App setup shared by every platform's entry point (main.cpp, main_desktop.cpp):
 * wires up MazeGame and runs the game loop until the engine terminates.
 */
void runProgram(ey3::Engine& engine);

#endif // MAZE_PROGRAM_H
