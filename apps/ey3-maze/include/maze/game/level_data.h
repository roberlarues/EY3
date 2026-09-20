#ifndef MAZE_LEVEL_DATA_H
#define MAZE_LEVEL_DATA_H

#include <string>
#include <vector>

/**
 * A level as it comes out of assets/levels/: a rectangle of characters plus
 * the name of the file it was read from.
 *
 * It is plain data and knows nothing about the game -- what each character
 * means is decided by whoever builds the objects from it (MazeGame here), so
 * another game can reuse this with a legend of its own.
 */
class LevelData {
	private:
		std::string name;
		std::vector<std::string> rows;
		int cols;

	public:
		LevelData();
		LevelData(const std::string& name, const std::vector<std::string>& rows);

		const std::string& getName() const;
		int getCols() const;
		int getRows() const;
		bool isEmpty() const;

		/** The character at a cell, or ' ' outside the map. */
		char getCell(int col, int row) const;
};

#endif // MAZE_LEVEL_DATA_H
