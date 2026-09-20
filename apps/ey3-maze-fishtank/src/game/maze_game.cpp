#include "maze/game/maze_game.h"

#include <math.h>
#include <stdlib.h>

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
static const char CELL_EMPTY = ' ';

// How long the winning ball stays on the exit before the next level.
static const float WIN_PAUSE = 1.5f;

// Resolution asked of the front camera. Small is good: the head is a big
// blob, and every pixel costs latency, which is what breaks the illusion.
static const int CAMERA_WIDTH = 640;
static const int CAMERA_HEIGHT = 480;

// How far above the top edge of the screen the front camera sits. The rest
// of the offset is half the screen, which the device tells us, so this is
// the only part left to guess -- and a centimetre out here only shifts the
// viewpoint by a centimetre.
static const float LENS_ABOVE_SCREEN_CM = 0.5f;

// How long to wait for the camera before deciding there is none and moving
// the viewpoint by ourselves instead.
static const float CAMERA_GRACE_SECONDS = 2.0f;

// The demo orbit used when there is no camera: how far the viewpoint wanders
// and how far away it sits, in centimetres.
static const float DEMO_REACH_CM = 13.0f;
static const float DEMO_DISTANCE_CM = 50.0f;   // the distance the maze is sized for

// The face detector's model, in this app's assets.
static const char* const FACE_MODEL_ASSET = "models/face_detection_yunet.onnx";

// Set EY3_FISHTANK_NO_CAMERA to run the demo orbit and leave the camera
// alone -- handy on a machine with no webcam, or when you just want to see
// the effect without being filmed.
static const char* const NO_CAMERA_ENV = "EY3_FISHTANK_NO_CAMERA";

MazeGame::MazeGame()
	: hud(sprites, meshes), controls(nullptr), level(nullptr), player(nullptr), exit(nullptr),
	  levelIndex(0), levelFinished(false), winTimer(0.0f),
	  cameraEnabled(getenv(NO_CAMERA_ENV) == nullptr), cameraGivenUp(false), calibrated(false),
	  timeWithoutCamera(0.0f), demoTime(0.0f) {

	levels.load(&assetLoader);

	tracker.init(&assetLoader, FACE_MODEL_ASSET);
	if (!cameraEnabled) {
		LOGI("%s is set: the camera stays off and the viewpoint moves on its own",
		     NO_CAMERA_ENV);
	}
	cameraGivenUp = !cameraEnabled;

	controls = new TouchDirectionPad(&hud);
	hud.add(controls);

	buildLevel();
}

MazeGame::~MazeGame() {
	delete level;
}

void MazeGame::buildLevel() {
	const LevelData& data = levels.get(levelIndex);

	level = new Level(sprites, meshes, data.getCols(), data.getRows());
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
				case CELL_EMPTY:
					break;   // a hole in the level, on purpose
				default:
					// The levels are text files edited by hand, so say so
					// rather than quietly drawing nothing.
					LOGW("Level %s: unknown character '%c' at column %d, row %d",
					     data.getName().c_str(), data.getCell(col, row), col, row);
					break;
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

Camera& MazeGame::getCamera() {
	return camera;
}

void MazeGame::calibrateTracker() {
	// Once, as soon as both halves are known: the screen's size comes from
	// the window and the lens data from the camera, so nothing here is a
	// number typed in by hand except how far the lens sits above the glass.
	if (calibrated || hud.getScreenHeightCm() <= 0.0f) {
		return;
	}
	calibrated = true;

	float lensY = hud.getScreenHeightCm() / 2.0f + LENS_ABOVE_SCREEN_CM;
	tracker.setCameraOffset(0.0f, lensY);

	float fieldOfView = camera.getFieldOfView();
	if (fieldOfView > 0.0f) {
		tracker.setFieldOfView(fieldOfView);
		LOGI("Fishtank: camera sees %.1f degrees, lens %.1f cm above the screen centre",
		     fieldOfView, lensY);
	} else {
		LOGW("Fishtank: the camera does not report its field of view; "
		     "keeping the default, lens %.1f cm above the screen centre", lensY);
	}

}

void MazeGame::updateViewpoint(float deltaTime) {
	view.setScreenSize(hud.getScreenWidthCm(), hud.getScreenHeightCm());

	if (cameraEnabled && !cameraGivenUp) {
		// On Android the permission is granted asynchronously, so open() is
		// retried until it takes; on desktop it opens the webcam on the first
		// call. Either way it is a cheap no-op once open.
		if (!camera.isOpened()) {
			camera.open(CameraFacing::FRONT, CAMERA_WIDTH, CAMERA_HEIGHT);
		}

		if (camera.isLoaded()) {
			calibrateTracker();   // the camera can be asked about itself now

			if (camera.getFrame(frame)) {
				tracker.update(frame, deltaTime);
			}
			view.setEyePosition(tracker.getEyePosition());
			return;
		}

		timeWithoutCamera += deltaTime;
		if (timeWithoutCamera < CAMERA_GRACE_SECONDS) {
			return;   // still waiting: keep the resting viewpoint
		}
		cameraGivenUp = true;
		LOGW("No camera after %.0f s; moving the viewpoint on its own instead",
		     CAMERA_GRACE_SECONDS);
	}

	// No camera: walk the viewpoint around slowly, so the effect is still
	// visible (and so the app can be checked on a machine without one).
	demoTime += deltaTime;
	view.setEyePosition(Vec3{
		DEMO_REACH_CM * sinf(demoTime * 0.70f),
		DEMO_REACH_CM * 0.55f * sinf(demoTime * 0.47f + 1.0f),
		DEMO_DISTANCE_CM + 4.0f * sinf(demoTime * 0.31f)});
}

void MazeGame::update(float deltaTime) {
	updateViewpoint(deltaTime);
	meshes.setViewProjection(view.getViewProjection());

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
