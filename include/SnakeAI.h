#ifndef SNAKEAI_H
#define SNAKEAI_H
#include <vector>
#include "GameTypes.h"
#include <cstddef>
#include <cstdint>
class SnakeAI {
private:
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
			for (std::size_t i = 0; i < tail.size(); i++) {
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
		std::size_t operator()(const StateKey& state) const {
			std::uint64_t seed = 0x9e3779b97f4a7c15ULL;
			auto mix = [](std::uint64_t x) {
				x += 0x9e3779b97f4a7c15ULL;
				x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
				x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
				return x ^ (x >> 31);
				};
			auto packPoint = [](const Point& p) -> std::uint64_t {
				return (static_cast<std::uint64_t>(
					static_cast<std::uint32_t>(p.x)) << 32)
					| static_cast<std::uint32_t>(p.y);
				};
			seed = mix(seed ^ packPoint(state.head));
			seed = mix(
				seed ^ static_cast<std::uint64_t>(state.currentDirection));
			for (const Point& part : state.tail) {
				seed = mix(seed ^ packPoint(part));
			}
		return static_cast<size_t>(seed);
		}
	};
	struct SearchState {
		Point head = { -1, -1 };
		std::vector<Point> tail;
		// Huong di cua buoc gan nhat
		Direction currentDirection = STOP;
	};
	struct AStarNode {
		SearchState state;
		int g = 0;  // So buoc da di
		// Chi so nut cha trong danh sach cac nut A*
		int parentIndex = -1;
		// Huong di tu nut cha den nut hien tai
		Direction directionFromParent = STOP;
	};
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
		bool fruitAvailable,
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
	std::size_t pathIndex = 0;
	// Fruit ma duong di hien tai dang huong toi
	Point cachedFruit = { -1, -1 };
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
	long long totalKeyGenerationTimeNs = 0;
	long long maxKeyGenerationTimeNs = 0;
	long long totalSimulateMoveTimeNs = 0;
	long long totalCheckValidTimeNs = 0;
	long long totalBestGTimeNs = 0;
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
	long long GetTotalSimulateMoveTimeNs() const {
		return totalSimulateMoveTimeNs;
	}
	long long GetTotalCheckValidTimeNs() const {
		return totalCheckValidTimeNs;
	}
	long long GetTotalBestGTimeNs() const {
		return totalBestGTimeNs;
	}
	long long GetTotalKeyGenerationTimeNs() const {
		return totalKeyGenerationTimeNs;
	}
	long long GetMaxKeyGenerationTimeNs() const {
		return maxKeyGenerationTimeNs;
	}
	//thong tin debug
	int GetCacheInvalidationCount() const {
		return cacheInvalidationCount;
	}
};
#endif