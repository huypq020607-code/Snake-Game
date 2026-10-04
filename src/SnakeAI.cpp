#include "../include/SnakeAI.h"
Direction SnakeAI::GetNextDirection(Point head, Point fruit) {
	if (fruit.x > head.x) {
		return RIGHT;
	}
	if (fruit.x < head.x) {
		return LEFT;
	}
	if (fruit.y > head.y) {
		return DOWN;
	}
	if (fruit.y < head.y) {
		return UP;
	}
	return STOP;
}