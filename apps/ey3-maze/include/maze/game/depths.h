#ifndef MAZE_DEPTHS_H
#define MAZE_DEPTHS_H

/**
 * The z-order of a level, in one place so it can be read as a whole.
 *
 * These are NDC depths (-1 nearest, 1 farthest): the engine's Renderer keeps
 * GL_DEPTH_TEST on, so sprites drawn over each other need distinct values.
 */
const float DEPTH_FLOOR = 0.6f;
const float DEPTH_EXIT = 0.55f;
const float DEPTH_WALL = 0.5f;
const float DEPTH_PLAYER = 0.2f;
const float DEPTH_OVERLAY = 0.0f;

#endif // MAZE_DEPTHS_H
