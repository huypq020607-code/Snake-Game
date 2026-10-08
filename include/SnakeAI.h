#ifndef SNAKEAI_H
#define SNAKEAI_H
#include <vector>
#include "GameTypes.h"
class SnakeAI {
private:
	//BFS tim duong tu head toi fruit
	Direction FindPathBFS(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		int width,
		int height
	);
	//kiem tra huong di co an toan khong
	bool CheckSafeMove(
		Point head,
		Direction dir,
		const std::vector<Point>& tail,
		int width,
		int height
	);
	//neu an fruit thi con bao nhieu o co the di toi
	int CountReachableCells(
		Point start,
		const std::vector<Point>& tail,
		int width,
		int height
	);
	//neu khong tim duoc duong den fruit thi tim duong an toan va gan fruit nhat
	Direction FindSafeDirection(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		Direction currentDirection,
		int width,
		int height
	);
	//kiem tra sau khi an co bi nhot khong
	bool IsSafeAfterMove(
		Point head,
		Direction dir,
		Point fruit,
		const std::vector<Point>& tail,
		int width,
		int height
	);
	//mo phong buoc di
	void SimulateMove(
		Point head,
		Direction dir,
		Point fruit,
		const std::vector<Point>& tail,
		Point& newHead,
		std::vector<Point>& newTail,
		bool& ateFruit
	);
	//danh gia tuong lai
	int EvaluateFuture(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		Direction currentDirection,
		int width,
		int height,
		int depth
	);
public:
	//ham chinh cua AI
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