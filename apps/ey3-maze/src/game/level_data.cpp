#include "maze/game/level_data.h"

LevelData::LevelData() : cols(0) {
}

LevelData::LevelData(const std::string& name, const std::vector<std::string>& rows)
	: name(name), rows(rows), cols(0) {
	// Levels need not be padded to a rectangle in the file; the widest row
	// sets the width and the short ones simply have empty cells.
	for (size_t i = 0; i < rows.size(); i++) {
		if ((int) rows[i].size() > cols) {
			cols = (int) rows[i].size();
		}
	}
}

const std::string& LevelData::getName() const {
	return name;
}

int LevelData::getCols() const {
	return cols;
}

int LevelData::getRows() const {
	return (int) rows.size();
}

bool LevelData::isEmpty() const {
	return cols == 0 || rows.empty();
}

char LevelData::getCell(int col, int row) const {
	if (row < 0 || row >= (int) rows.size() || col < 0 || col >= (int) rows[row].size()) {
		return ' ';
	}
	return rows[row][col];
}
