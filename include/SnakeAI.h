#ifndef SNAKEAI_H
#define SNAKEAI_H
#include <vector>
#include "GameTypes.h"
#include <queue>
#include <string>
class SnakeAI {
private:
	struct SearchState {
		Point head;
		std::vector<Point> tail;
		// Huong di dau tien tu vi tri ban dau
		Direction firstDirection;
		// Huong di cua buoc gan nhat
		Direction currentDirection;
	};
	struct AStarNode {
    		SearchState state;
    		int g;  // So buoc da di
    		int h;  // Uoc luong khoang cach den fruit
	};
	//BFS tim duong tu head toi fruit
	Direction FindPathBFS(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		Direction currentDirection,
		int width,
		int height
	);
	Direction FindPathAStar(
    		Point head,
    		Point fruit,
    		const std::vector<Point>& tail,
    		Direction currentDirection,
    		int width,
    		int height
	);
	//kiem tra huong di co an toan khong
	bool CheckSafeMove(
		Point head,
		Direction dir,
		Point fruit,
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
		bool fruitAvailble,
		int depth
	);
	//danh gia trang thai
	int EvaluatePosition(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		int width,
		int height,
		bool fruitAvailable
	);
	//kiem tra sau khi an
	bool IsSafeAfterEating(
		Point head,
		Direction dir,
		Point fruit,
		const std::vector<Point>& tail,
		int width,
		int height
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