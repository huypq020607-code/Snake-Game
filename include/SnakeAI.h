#ifndef SNAKEAI_H
#define SNAKEAI_H
#include "GameTypes.h"
class SnakeAI {
public:
	Direction GetNextDirection(
		Point head,
		Point fruit
	);
};
#endif