#ifndef GAMETYPES_H
#define GAMETYPES_H
// ==================== DATA ====================
struct Point {
	int x = -1;
	int y = -1;
};
// ==================== ENUM ====================
enum Direction {
	STOP = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN
};
enum ControlMode {
	HUMAN_MODE = 0,
	AI_MODE
};
#endif