#ifndef ORB_DEMO_PROGRAM_H
#define ORB_DEMO_PROGRAM_H

#include <ey3.h>

/**
 * App setup shared by every platform's entry point (main.cpp, main_desktop.cpp):
 * wires up Camera/CameraView/OrbTest and runs the loop until the engine
 * terminates. Not just Engine::run(), since it polls the camera each frame.
 */
void runProgram(ey3::Engine& engine);

#endif // ORB_DEMO_PROGRAM_H
