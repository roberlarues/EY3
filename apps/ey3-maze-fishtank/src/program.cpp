#include "maze/program.h"

#include "maze/game/level.h"
#include "maze/game/maze_game.h"
#include "maze/game/scene.h"

using namespace ey3;

namespace {

	// A scene is a Renderizable that draws the objects inside it, and a
	// listener that passes input on to them, so a whole layer goes into the
	// engine in one go.
	void addScene(Engine& engine, Scene& scene) {
		engine.getRenderer()->addRenderizable(&scene);
		engine.getInputHandler()->addListener(&scene);
	}

	void removeScene(Engine& engine, Scene& scene) {
		engine.getRenderer()->removeRenderizable(&scene);
		engine.getInputHandler()->removeListener(&scene);
	}
}

void runProgram(Engine& engine) {
	MazeGame game;

	// The camera opens and closes with the window, so it listens to the
	// lifecycle commands. The game only asks it for frames.
	engine.getCmdHandler()->addListener(&game.getCamera());

	// The interface lasts the whole game, so it goes in once. The level is
	// swapped for the next one every time it is cleared.
	addScene(engine, game.getHud());

	while (!engine.hasTerminated()) {
		addScene(engine, game.getLevel());

		// The game loop belongs to the app, not to the engine: pump the
		// platform's events, advance the game by the time the last frame
		// took, and draw.
		while (!engine.hasTerminated() && !game.isLevelFinished()) {
			engine.pollEvents();

			if (engine.isInForeground()) {
				game.update(engine.getDeltaTime());
				engine.getRenderer()->renderFrame();
			}
		}

		// Out of the engine before the game deletes it.
		removeScene(engine, game.getLevel());
		game.nextLevel();
	}
}
