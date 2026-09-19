#include "maze/game/direction.h"

int colOffset(Direction direction) {
	if (direction == Direction::LEFT) return -1;
	if (direction == Direction::RIGHT) return 1;
	return 0;
}

int rowOffset(Direction direction) {
	if (direction == Direction::UP) return -1;
	if (direction == Direction::DOWN) return 1;
	return 0;
}
