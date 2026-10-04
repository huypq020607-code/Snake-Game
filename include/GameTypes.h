#ifndef GAMETYPES_H
#define GAMETYPES_H
// ==================== DATA ====================
struct Point {
	int x;
	int y;
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