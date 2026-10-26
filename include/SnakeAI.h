#ifndef SNAKEAI_H
#define SNAKEAI_H
#include <vector>
#include "GameTypes.h"
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstdlib>
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
		std::vector<Direction> path; // Toan bo duong di
		// Chi so nut cha trong danh sach cac nut A*
		int parentIndex;
		// Huong di tu nut cha den nut hien tai
		Direction directionFromParent;
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
	//tim duong di A*
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
	// thu bam duoi neu A* khong tim duoc duong di toi fruit an toan
	Direction FindTailFollowingDirection(
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
	//duong an toan A*
	bool IsPathSafe(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		Direction currentDirection,
		const std::vector<Direction>& path,
		int width,
		int height
	);
	// kiem tra trang thai sau khi an fruit
	bool IsSafeAfterEatingState(
		Point head,
		const std::vector<Point>& tail,
		int width,
		int height
	);
	//tim duong den vi tri duoi hien tai
	bool CanReachTail(
		Point head,
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
	// dem huong an toan
	int CountSafeDirections(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		Direction currentDirection,
		int width,
		int height
	);
	// dem huong an toan du doan
	int CountFutureOptions(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		Direction currentDirection,
		int width,
		int height
	);
	// Duong di A* da tim duoc
	std::vector<Direction> cachedPath;
	// Vi tri tiep theo trong duong di
	size_t pathIndex = 0;
	// Fruit ma duong di hien tai dang huong toi
	Point cachedFruit = { -1, -1 };
	void ClearCachedPath();
	int astarCallCount = 0;
	int cacheInvalidationCount = 0;
	// bo dem chuan doan
	int pathFoundCount = 0;
	int stateLimitCount = 0;
	int discoveryLimitCount = 0;
	//lan tim duong gan nhat co that bai hay khong
	bool lastSearchFailed = false;
	//so luot con lai truoc khi thu lai A*
	int failedSearchCooldown = 0;
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
	//A* tra ve toan bo duong di
	std::vector<Direction> FindPathAStarPath(
		Point head,
		Point fruit,
		const std::vector<Point>& tail,
		Direction currentDirection,
		int width,
		int height
	);
	//bo dem so lan chay A*
	int GetAStarCallCount() const {
		return astarCallCount;
	}
	//bo dem chan doan
	int GetPathFoundCount() const {
		return pathFoundCount;
	}
	int GetStateLimitCount() const {
		return stateLimitCount;
	}
	int GetDiscoveryLimitCount() const {
		return discoveryLimitCount;
	}
	//thong tin debug
	int GetCacheInvalidationCount() const {
		return cacheInvalidationCount;
	}
};
#endif