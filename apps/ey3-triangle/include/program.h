#ifndef TRIANGLE_PROGRAM_H
#define TRIANGLE_PROGRAM_H

#include <ey3.h>

/**
 * App setup shared by every platform's entry point (main.cpp, main_desktop.cpp):
 * wires up Triangle and runs the standard loop until the engine terminates.
 */
void runProgram(ey3::Engine& engine);

#endif // TRIANGLE_PROGRAM_H
