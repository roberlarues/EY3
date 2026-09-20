#include "maze/game/grid.h"

#include <math.h>

#include <ey3.h>

Grid::Grid(int cols, int rows)
	: cols(cols), rows(rows), tileSize(TILE_ART_SIZE), originX(0.0f), originY(0.0f),
	  screenWidth(0.0f), screenHeight(0.0f) {
}

void Grid::fit(float screenWidth, float screenHeight) {
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	int scale = (int) fminf(screenWidth / (cols * TILE_ART_SIZE), screenHeight / (rows * TILE_ART_SIZE));
	if (scale < 1) {
		scale = 1;
	}

	tileSize = (float) (TILE_ART_SIZE * scale);
	originX = (screenWidth - cols * tileSize) / 2.0f;
	originY = (screenHeight - rows * tileSize) / 2.0f;
	LOGI("Grid: %dx%d cells, tile %d px (x%d)", cols, rows, (int) tileSize, scale);
}

int Grid::getCols() const {
	return cols;
}

int Grid::getRows() const {
	return rows;
}

bool Grid::contains(int col, int row) const {
	return col >= 0 && row >= 0 && col < cols && row < rows;
}

float Grid::getTileSize() const {
	return tileSize;
}

float Grid::getScreenWidth() const {
	return screenWidth;
}

float Grid::getScreenHeight() const {
	return screenHeight;
}

Rect Grid::cellBounds(float col, float row) const {
	return {originX + col * tileSize, originY + row * tileSize, tileSize, tileSize};
}
