#ifndef PROGRAM_H
#define PROGRAM_H

#include <ey3.h>

// Platform-independent app logic, called from both desktop/main_desktop.cpp
// and android/src/main_android.cpp -- see apps/ey3-triangle in the main EY3
// repo for the same pattern.
void runProgram(ey3::Engine& engine);

#endif // PROGRAM_H
