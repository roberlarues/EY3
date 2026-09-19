#include "maze/game/maze_game.h"

#include "maze/game/level_data.h"
#include "maze/objects/exit.h"
#include "maze/objects/floor.h"
#include "maze/objects/player.h"
#include "maze/objects/wall.h"
#include "maze/ui/touch_direction_pad.h"

// What the characters of a level file mean (see assets/levels/index.txt).
static const char CELL_WALL = '#';
static const char CELL_FLOOR = '.';
static const char CELL_EXIT = 'X';
static const char CELL_START = '@';

// How long the winning ball stays on the exit before the next level.
static const float WIN_PAUSE = 1.5f;

MazeGame::MazeGame()
	: hud(sprites), controls(nullptr), level(nullptr), player(nullptr), exit(nullptr),
	  levelIndex(0), levelFinished(false), winTimer(0.0f) {

	levels.load(&assetLoader);

	controls = new TouchDirectionPad(&hud);
	hud.add(controls);

	buildLevel();
}

MazeGame::~MazeGame() {
	delete level;
}

void MazeGame::buildLevel() {
	const LevelData& data = levels.get(levelIndex);

	level = new Level(sprites, data.getCols(), data.getRows());
	player = nullptr;
	exit = nullptr;
	levelFinished = false;
	winTimer = 0.0f;

	for (int row = 0; row < data.getRows(); row++) {
		for (int col = 0; col < data.getCols(); col++) {
			switch (data.getCell(col, row)) {
				case CELL_WALL:
					level->add(new Wall(level, col, row));
					break;
				case CELL_EXIT:
					exit = new Exit(level, col, row);
					level->add(exit);
					break;
				case CELL_START:
					// The ball starts on an ordinary floor cell.
					level->add(new Floor(level, col, row));
					player = new Player(level, col, row, controls);
					break;
				case CELL_FLOOR:
					level->add(new Floor(level, col, row));
					break;
				default:
					break;   // anything else leaves the cell empty
			}
		}
	}

	// Added last so it is built on top of the cells it walks over.
	if (player != nullptr) {
		level->add(player);
	}

	if (player == nullptr || exit == nullptr) {
		LOGE("Level %s has no '%c' start or no '%c' exit; it cannot be played",
		     data.getName().c_str(), CELL_START, CELL_EXIT);
	} else {
		LOGI("Playing level %d/%d: %s", levelIndex + 1, levels.size(), data.getName().c_str());
	}
}

Level& MazeGame::getLevel() {
	return *level;
}

Scene& MazeGame::getHud() {
	return hud;
}

void MazeGame::update(float deltaTime) {
	hud.update(deltaTime);
	level->update(deltaTime);

	if (levelFinished || player == nullptr || exit == nullptr) {
		return;
	}

	if (winTimer > 0.0f) {
		winTimer -= deltaTime;
		if (winTimer <= 0.0f) {
			levelFinished = true;
		}
		return;
	}

	if (player->getCol() == exit->getCol() && player->getRow() == exit->getRow()) {
		LOGI("Level solved!");
		player->celebrate();
		winTimer = WIN_PAUSE;
	}
}

bool MazeGame::isLevelFinished() const {
	return levelFinished;
}

void MazeGame::nextLevel() {
	delete level;
	level = nullptr;

	levelIndex = levels.size() > 0 ? (levelIndex + 1) % levels.size() : 0;
	buildLevel();
}
