#ifndef SNAKEAI_H
#define SNAKEAI_H
#include <vector>
#include "GameTypes.h"
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstdlib>
// luu thong tin dinh danh trang thai
struct StateKey {
	Point head = { -1, -1 };
	std::vector<Point> tail;
	Direction currentDirection = STOP;
	bool operator==(const StateKey& other) const {
		if (head.x != other.head.x ||
			head.y != other.head.y ||
			currentDirection != other.currentDirection ||
			tail.size() != other.tail.size()) {
			return false;
		}
		for (size_t i = 0; i < tail.size(); i++) {
			if (tail[i].x != other.tail[i].x ||
				tail[i].y != other.tail[i].y) {
				return false;
			}
		}
		return true;
	}
};
// tao ma bam de tra cuu nhanh
struct StateHash {
	size_t operator()(const StateKey& state) const {
		size_t seed = 0;
		auto hashCombine = [&seed](int value) {
			seed ^= std::hash<int>{}(value)
				+0x9e3779b9
				+ (seed << 6)
				+ (seed >> 2);
			};
		hashCombine(state.head.x);
		hashCombine(state.head.y);
		hashCombine(static_cast<int>(
			state.currentDirection));
		for (const Point& part : state.tail) {
			hashCombine(part.x);
			hashCombine(part.y);
		}
		return seed;
	}
};
class SnakeAI {
private:
	struct SearchState {
		Point head = { -1, -1 };
		std::vector<Point> tail;
		// Huong di dau tien tu vi tri ban dau
		Direction firstDirection = STOP;
		// Huong di cua buoc gan nhat
		Direction currentDirection = STOP;
	};
	struct AStarNode {
		SearchState state;
		int g = 0;  // So buoc da di
		int h = 0;  // Uoc luong khoang cach den fruit
		std::vector<Direction> path; // Toan bo duong di
		// Chi so nut cha trong danh sach cac nut A*
		int parentIndex = -1;
		// Huong di tu nut cha den nut hien tai
		Direction directionFromParent = STOP;
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
	int bothLimitCount = 0;
	int noPathCount = 0;
	long long keyGenerationCount = 0;
	long long totalAStarTimeUs = 0;
	long long maxAStarTimeUs = 0;
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
	double GetAStarSuccessRate() const {
		if (astarCallCount == 0) {
			return 0.0;
		}
		return 100.0 * pathFoundCount / astarCallCount;
	}
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
	int GetBothLimitCount() const {
		return bothLimitCount;
	}
	int GetNoPathCount() const {
		return noPathCount;
	}
	long long GetKeyGenerationCount() const {
		return keyGenerationCount;
	}
	long long GetTotalAStarTimeUs() const {
		return totalAStarTimeUs;
	}
	long long GetMaxAStarTimeUs() const {
		return maxAStarTimeUs;
	}
	//thong tin debug
	int GetCacheInvalidationCount() const {
		return cacheInvalidationCount;
	}
};
#endif