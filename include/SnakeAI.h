#ifndef SNAKEAI_H
#define SNAKEAI_H
#include <vector>
#include "GameTypes.h"
class SnakeAI {
private:
	Direction FindPathBFS(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		int width,
		int height
	);
	bool CheckSafeMove(
		Point head,
		Direction dir,
		const std::vector<Point>& tail,
		int width,
		int height
	);
public:
	Direction GetNextDirection(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		Direction currentDirection,
		int width,
		int height
	);
};
#endif