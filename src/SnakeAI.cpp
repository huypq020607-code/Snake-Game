#include "../include/SnakeAI.h"
#include <vector>
#include <queue>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <utility>
#include <unordered_map>
#include <unordered_set>
//dinh nghia ham 
bool IsOppositeDirection(Direction current, Direction next);
// kiem tra point co nam tren tail khong
bool CheckTail(
	Point p,
	const std::vector<Point>& tail
) {
	for (const Point& part : tail) {
		if (p.x == part.x && p.y == part.y) {
			return true;
		}
	}
	return false;
}
//kiem tra co the di vao khong
bool CheckValid(
	Point p,
	const std::vector<Point>& tail,
	int width,
	int height
) {
	//kiem tra tuong
	if (p.x < 0 || p.x >= width ||
		p.y < 0 || p.y >= height) {
		return false;
	}
	// Kiem tra than ran
	return !CheckTail(p, tail);
}
//A* tra ve toan bo duong di
std::vector<Direction> SnakeAI::FindPathAStarPath(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
) {
	astarCallCount++;
	auto startTime = std::chrono::steady_clock::now();
	// 1. Ghi nhan thoi gian A*
	auto recordAStarTime = [this, &startTime]() {
		auto endTime = std::chrono::steady_clock::now();
		long long elapsed =
			std::chrono::duration_cast<std::chrono::microseconds>(
				endTime - startTime
			).count();
		totalAStarTimeUs += elapsed;
		maxAStarTimeUs = (std::max)(maxAStarTimeUs, elapsed);
	};
	// 2. Ghi nhan thoi gian thao tac bestG
	auto recordBestGTime = [this](
		const std::chrono::steady_clock::time_point& start
		) {
			auto end = std::chrono::steady_clock::now();
			totalBestGTimeNs +=
				std::chrono::duration_cast<std::chrono::nanoseconds>(
					end - start
				).count();
		};
	// 3. Kiem tra dau vao
	if (width <= 0 || height <= 0 ||
		head.x < 0 || head.x >= width ||
		head.y < 0 || head.y >= height ||
		fruit.x < 0 || fruit.x >= width ||
		fruit.y < 0 || fruit.y >= height) {
		recordAStarTime();
		return {};
	}
	// 4. Heuristic Manhattan
	auto heuristic = [&fruit](Point point) {
		return std::abs(point.x - fruit.x)
			+ std::abs(point.y - fruit.y);
		};
	// 5. Tao khoa duy nhat cho trang thai
	auto makeKey = [this](const SearchState& state) {
		auto start = std::chrono::steady_clock::now();
		keyGenerationCount++;
		StateKey key;
		key.head = state.head;
		key.tail = state.tail;
		key.currentDirection = state.currentDirection;
		key.fruit = state.fruit;
		auto end = std::chrono::steady_clock::now();
		long long elapsed =
			std::chrono::duration_cast<std::chrono::nanoseconds>(
				end - start
			).count();
		totalKeyGenerationTimeNs += elapsed;
		maxKeyGenerationTimeNs =
			(std::max)(maxKeyGenerationTimeNs, elapsed);

		return key;
		};
	// 6. Cau truc phan tu trong hang doi uu tien
	struct OpenEntry {
		int nodeIndex;
		int f;
		int h;
	};
	struct Compare {
		bool operator()(
			const OpenEntry& a,
			const OpenEntry& b
			) const {
			if (a.f != b.f) {
				return a.f > b.f;
			}
			return a.h > b.h;
		}
	};
	// 7. Khoi tao cac cau truc du lieu
	std::priority_queue<
		OpenEntry,
		std::vector<OpenEntry>,
		Compare
	> open;
	std::vector<AStarNode> nodes;
	const int MAX_STATES = 1200;
	const int MAX_DISCOVERED = 2400;
	nodes.reserve(1 + MAX_STATES * 4);
	std::unordered_map<StateKey, int, StateHash> bestG;
	// Toi uu bang bam
	bestG.max_load_factor(0.7f);
	bestG.reserve(MAX_DISCOVERED);
	// 8. Khoi tao trang thai ban dau
	SearchState start;
	start.head = head;
	start.tail = tail;
	start.currentDirection = currentDirection;
	start.fruit = fruit;
	AStarNode startNode;
	startNode.state = start;
	startNode.g = 0;
	startNode.parentIndex = -1;
	startNode.directionFromParent = STOP;
	nodes.push_back(std::move(startNode));
	int startH = heuristic(head);
	open.push({ 0, startH, startH });
	bestG.emplace(makeKey(start), 0);
	Direction directions[4] = {
		UP, DOWN, LEFT, RIGHT
	};
	int statesChecked = 0;
	int statesDiscovered = 1;
	bool discoveryLimitHit = false;
	// 9. Bat dau tim kiem A*
	while (!open.empty() && statesChecked < MAX_STATES) {
		OpenEntry entry = open.top();
		open.pop();
		int nodeIndex = entry.nodeIndex;
		// Bo qua trang thai khong con la duong di tot nhat
		const AStarNode& savedNode = nodes[nodeIndex];
		StateKey currentKey = makeKey(savedNode.state);
		auto found = bestG.find(currentKey);
		if (found == bestG.end() ||
			savedNode.g != found->second) {
			continue;
		}
		statesChecked++;
		// Sao chep trang thai hien tai
		const SearchState& current = savedNode.state;
		int currentG = savedNode.g;
		// 10. Kiem tra da den dich
		if (current.head.x == fruit.x &&
			current.head.y == fruit.y) {
			std::vector<Direction> path;
			int index = nodeIndex;
			// Truy nguoc duong di thong qua parentIndex
			while (nodes[index].parentIndex != -1) {
				path.push_back(
					nodes[index].directionFromParent
				);
				index = nodes[index].parentIndex;
			}
			// Dao nguoc duong di
			std::reverse(path.begin(), path.end());
			pathFoundCount++;
			recordAStarTime();
			return path;
		}
		// 11. Mo rong cac huong di
		for (Direction dir : directions) {
			// Khong cho ran quay nguoc truc tiep
			if (current.currentDirection != STOP &&
				IsOppositeDirection(
					current.currentDirection,
					dir)) {
				continue;
			}
			Point newHead;
			bool ateFruit;
			// Tao trang thai tiep theo
			SearchState next;
			// Mo phong di chuyen
			auto simulateStart = std::chrono::steady_clock::now();
			SimulateMove(
				current.head,
				dir,
				current.fruit,
				current.tail,
				newHead,
				next.tail,
				ateFruit
			);
			auto simulateEnd = std::chrono::steady_clock::now();
			totalSimulateMoveTimeNs +=
				std::chrono::duration_cast<std::chrono::nanoseconds>(
					simulateEnd - simulateStart
				).count();
			// Kiem tra tinh hop le
			auto validStart = std::chrono::steady_clock::now();
			bool valid = CheckValid(
				newHead,
				next.tail,
				width,
				height
			);
			auto validEnd = std::chrono::steady_clock::now();
			totalCheckValidTimeNs +=
				std::chrono::duration_cast<std::chrono::nanoseconds>(
					validEnd - validStart
				).count();
			if (!valid) {
				continue;
			}
			next.head = newHead;
			next.currentDirection = dir;
			next.fruit = current.fruit;
			// Tinh chi phi duong di
			int nextG = currentG + 1;
			// Tao khoa trang thai
			StateKey key = makeKey(next);
			// Kiem tra trang thai da duoc phat hien chua
			auto bestGStart = std::chrono::steady_clock::now();
			auto nextFound = bestG.find(key);
			if (nextFound != bestG.end()) {
				// Da co duong di tot hon hoac bang
				if (nextG >= nextFound->second) {
					recordBestGTime(bestGStart);
					continue;
				}
				// Cap nhat chi phi tot hon
				nextFound->second = nextG;
			}
			else {
				// Kiem tra gioi han trang thai moi
				if (statesDiscovered >= MAX_DISCOVERED) {
					discoveryLimitHit = true;
					recordBestGTime(bestGStart);
					continue;
				}
				statesDiscovered++;
				bestG.emplace(std::move(key), nextG);
			}
			recordBestGTime(bestGStart);
			// Tinh heuristic
			int nextH = heuristic(newHead);
			// Tao node moi va lien ket node cha
			int nextIndex = static_cast<int>(nodes.size());
			AStarNode nextNode;
			nextNode.state = std::move(next);
			nextNode.g = nextG;
			nextNode.parentIndex = nodeIndex;
			nextNode.directionFromParent = dir;
			nodes.push_back(std::move(nextNode));
			// Dua node vao hang doi uu tien
			open.push({
				nextIndex,
				nextG + nextH,
				nextH
				});
		}
	}
	// 12. Cap nhat thong ke gioi han tim kiem
	bool stateLimitHit =
    		(statesChecked >= MAX_STATES && !open.empty());
	if (stateLimitHit) {
		stateLimitCount++;
	}
	if (discoveryLimitHit) {
		discoveryLimitCount++;
	}
	if (stateLimitHit && discoveryLimitHit) {
		bothLimitCount++;
	}
	// Dem moi lan A* khong tim duoc duong di
	noPathCount++;
	// 13. Ghi nhan truong hop khong tim thay duong di
	recordAStarTime();
	return {};
}
// tim duong den vi tri duoi hien tai
bool SnakeAI::CanReachTail(
	Point head,
	const std::vector<Point>& tail,
	int width,
	int height
) {
	if (width <= 0 || height <= 0 || tail.empty()) {
		return false;
	}
	// O cuoi cung cua tail la vi tri duoi
	Point target = tail.back();
	// Kiem tra toa do dau va duoi
	if (head.x < 0 || head.x >= width ||
		head.y < 0 || head.y >= height ||
		target.x < 0 || target.x >= width ||
		target.y < 0 || target.y >= height) {
		return false;
	}
	std::vector<unsigned char> blocked(width * height, 0);
	std::vector<unsigned char> visited(width * height, 0);
	// Danh dau than ran, tru o duoi vi duoi co the di chuyen
	for (int i = 0; i < static_cast<int>(tail.size()); i++) {
		if (i == static_cast<int>(tail.size()) - 1) {
			continue;
		}
		const Point& part = tail[i];
		if (part.x >= 0 && part.x < width &&
			part.y >= 0 && part.y < height) {
			blocked[part.y * width + part.x] = 1;
		}
	}
	std::vector<Point> q;
	q.reserve(width * height);
	q.push_back(head);
	visited[head.y * width + head.x] = 1;
	int front = 0;
	while (front < static_cast<int>(q.size())) {
		Point current = q[front++];

		if (current.x == target.x &&
			current.y == target.y) {
			return true;
		}
		Point next[4] = {
			{current.x + 1, current.y},
			{current.x - 1, current.y},
			{current.x, current.y + 1},
			{current.x, current.y - 1}
		};
		for (int i = 0; i < 4; i++) {
			Point p = next[i];
			if (p.x < 0 || p.x >= width ||
				p.y < 0 || p.y >= height) {
				continue;
			}
			int index = p.y * width + p.x;
			if (blocked[index] || visited[index]) {
				continue;
			}
			visited[index] = 1;
			q.push_back(p);
		}
	}
	return false;
}
//kiem tra kha nang song khi di theo duoi
bool SnakeAI::CanSurviveByFollowingTail(
    const Point& head,
    const std::vector<Point>& tail,
    Direction currentDirection,
    const Point& fruit,
    int width,
    int height
) {
    // 1. Khoi tao trang thai ban dau
    SearchState start;
    start.head = head;
    start.tail = tail;
    start.currentDirection = currentDirection;
    start.fruit = fruit;
    start.ateInitialFruit = false;
    std::queue<SearchState> q;
    std::unordered_set<StateKey, StateHash> visited;
    q.push(start);
    StateKey startKey;
    startKey.head = start.head;
    startKey.tail = start.tail;
    startKey.currentDirection = start.currentDirection;
    startKey.fruit = start.fruit;
    startKey.ateInitialFruit = start.ateInitialFruit;
    visited.insert(startKey);
    // 2. Cac huong di chuyen
    Direction directions[] = {
        UP, DOWN, LEFT, RIGHT
    };
    // 3. Gioi han tim kiem
    const size_t MAX_STATES = 1200;
    size_t exploredStates = 0;
    // 4. BFS
    while (!q.empty()) {
        SearchState current = q.front();
        q.pop();
        exploredStates++;
        if (exploredStates > MAX_STATES) {
            return false;
        }
        // 5. Thu cac huong di chuyen
        for (Direction dir : directions) {
            // Khong cho quay nguoc 180 do
            if (
                (current.currentDirection == UP && dir == DOWN) ||
                (current.currentDirection == DOWN && dir == UP) ||
                (current.currentDirection == LEFT && dir == RIGHT) ||
                (current.currentDirection == RIGHT && dir == LEFT)
            ) {
                continue;
            }
            // 6. Mo phong buoc di tiep theo
            Point newHead;
            std::vector<Point> newTail;
            bool ateFruit = false;
	    // Mo phong di chuyen den vi tri tiep theo
            SimulateMove(
                current.head,
                dir,
                current.fruit,
                current.tail,
                newHead,
                newTail,
                ateFruit
            );
            // 7. Kiem tra va cham sau khi mo phong
            if (!CheckValid(newHead, newTail, width, height)) {
                continue;
            }
            // 8. Tao trang thai moi
            SearchState nextState;
            nextState.head = newHead;
            nextState.tail = newTail;
            nextState.currentDirection = dir;
	    nextState.ateInitialFruit = current.ateInitialFruit;
	    // Xu ly khi an fruit
	    if (ateFruit) {
    		// Neu da an fruit A truoc do thi an fruit B
    		if (current.ateInitialFruit) {
        		return true;
   		}
    		 // Danh dau da an A
    		nextState.ateInitialFruit = true;
    		// Lay danh sach cac vi tri co the sinh B
    		const int MAX_CANDIDATES = 24;
    		std::vector<Point> candidates = GenerateFruitCandidates(
        		nextState.head,
        		nextState.tail,
        		width,
        		height,
        		MAX_CANDIDATES
    		);
    		// Khong con o trong de sinh B
    		if (candidates.empty()) {
        		return true;
    		}
    		// Kiem tra tung vi tri B
    		const size_t MAX_BFS_STATES = 1200;
		bool allReachable = true;
    		for (const Point& candidate : candidates) {
        		SearchResult result = CanReachFruit(
    				nextState,
    				candidate,
   				width,
    				height,
    				MAX_BFS_STATES
			);
			if (result != SearchResult::REACHABLE) {
    				allReachable = false;
    				break;
			}
    		}
	    	// Tat ca vi tri B duoc lay mau deu co duong di
	    	if (allReachable) {
        		return true;
    		}
		// Khong dung duoc duong nay den A.
    		// Tiep tuc BFS de thu duong khac den A.
    		continue;
	    }
	    else {
	    	nextState.fruit = current.fruit;
	    }
            // 9. Tao khoa trang thai
            StateKey nextKey;
            nextKey.head = nextState.head;
            nextKey.tail = nextState.tail;
            nextKey.currentDirection = nextState.currentDirection;
	    nextKey.fruit = nextState.fruit;
	    nextKey.ateInitialFruit = nextState.ateInitialFruit;
            // Bo qua trang thai da tham
            if (visited.find(nextKey) != visited.end()) {
                continue;
            }
            visited.insert(nextKey);
            // 10. Dua trang thai moi vao hang doi
            q.push(nextState);
        }
    }
    // Khong tim thay duong an ca fruit A va fruit B
    return false;
}
//tao fruit moi
bool SnakeAI::GenerateNewFruit(
    const Point& head,
    const std::vector<Point>& tail,
    int width,
    int height,
    Point& newFruit
) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Point p = {x, y};
            // Không đặt thức ăn tại đầu rắn
            if (p.x == head.x && p.y == head.y)
                continue;
            // Không đặt thức ăn trên thân rắn
            if (CheckTail(p, tail))
                continue;
            newFruit = p;
            return true;
        }
    }
    return false;
}
// cac o trong de sinh fruit
std::vector<Point> SnakeAI::GenerateFruitCandidates(
    const Point& head,
    const std::vector<Point>& tail,
    int width,
    int height,
    int maxCandidates
) {
    std::vector<Point> candidates;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Point p = {x, y};
            // Bo qua vi tri dau ran
            if (p.x == head.x && p.y == head.y) {
                continue;
            }
            // Bo qua cac vi tri than ran
            if (CheckTail(p, tail)) {
                continue;
            }
            candidates.push_back(p);
        }
    }
    // Neu so vi tri khong vuot qua gioi han
    if ((int)candidates.size() <= maxCandidates) {
        return candidates;
    }
    // Lay mau cac vi tri cach deu trong danh sach
    std::vector<Point> sampled;
    for (int i = 0; i < maxCandidates; i++) {
        int index = i * (int)candidates.size() / maxCandidates;
        sampled.push_back(candidates[index]);
    }
    return sampled;
}
//sau khi an fruit A kiem tra xem co the an fruit B duoc khong
SearchResult SnakeAI::CanReachFruit(
    const SearchState& start,
    const Point& targetFruit,
    int width,
    int height,
    size_t maxStates
) {
    std::queue<SearchState> q;
    std::unordered_set<StateKey, StateHash> visited;
    SearchState initial = start;
    initial.fruit = targetFruit;
    q.push(initial);
    StateKey startKey;
    startKey.head = initial.head;
    startKey.tail = initial.tail;
    startKey.currentDirection = initial.currentDirection;
    startKey.fruit = initial.fruit;
    startKey.ateInitialFruit = initial.ateInitialFruit;
    visited.insert(startKey);
    Direction directions[] = {UP, DOWN, LEFT, RIGHT};
    size_t exploredStates = 0;
    while (!q.empty()) {
        SearchState current = q.front();
        q.pop();
        exploredStates++;
        if (exploredStates > maxStates) {
            return SearchResult::UNKNOWN;
        }
        for (Direction dir : directions) {
            if (
                (current.currentDirection == UP && dir == DOWN) ||
                (current.currentDirection == DOWN && dir == UP) ||
                (current.currentDirection == LEFT && dir == RIGHT) ||
                (current.currentDirection == RIGHT && dir == LEFT)
            ) {
                continue;
            }
            Point newHead;
            std::vector<Point> newTail;
            bool ateFruit = false;
            SimulateMove(
                current.head,
                dir,
                targetFruit,
                current.tail,
                newHead,
                newTail,
                ateFruit
            );
            if (!CheckValid(newHead, newTail, width, height)) {
                continue;
            }
            // Da an duoc B
            if (ateFruit) {
                return SearchResult::REACHABLE;
            }
            SearchState nextState;
            nextState.head = newHead;
            nextState.tail = newTail;
            nextState.currentDirection = dir;
            nextState.fruit = targetFruit;
            nextState.ateInitialFruit = true;
            StateKey nextKey;
            nextKey.head = nextState.head;
            nextKey.tail = nextState.tail;
            nextKey.currentDirection = nextState.currentDirection;
            nextKey.fruit = nextState.fruit;
            nextKey.ateInitialFruit = nextState.ateInitialFruit;
            if (visited.find(nextKey) != visited.end()) {
                continue;
            }
            visited.insert(nextKey);
            q.push(nextState);
        }
    }
    return SearchResult::UNREACHABLE;
}
//kiem tra hai huong co nguoc nhau khong
bool IsOppositeDirection(
	Direction current,
	Direction next
) {
	if (current == LEFT && next == RIGHT) {
		return true;
	}
	if (current == RIGHT && next == LEFT) {
		return true;
	}
	if (current == UP && next == DOWN) {
		return true;
	}
	if (current == DOWN && next == UP) {
		return true;
	}
	return false;
}
//tinh khoang cach Manhattan
int Distance(Point a, Point b) {
	return abs(a.x - b.x) + abs(a.y - b.y);
}
//kiem tra khi an fruit con bao nhieu o co the di toi
int SnakeAI::CountReachableCells(
	Point start,
	const std::vector<Point>& tail,
	int width,
	int height
) {
	if (width <= 0 || height <= 0) {
		return 0;
	}
	if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) {
		return 0;
	}
	int totalCells = width * height;
	// Danh dau cac o bi than ran chiem
	std::vector<unsigned char> blocked(totalCells, 0);
	for (const Point& part : tail) {
		if (part.x >= 0 && part.x < width &&
			part.y >= 0 && part.y < height) {
			int index = part.y * width + part.x;
			blocked[index] = 1;
		}
	}
	// Mang danh dau cac o da tham
	std::vector<unsigned char> visited(totalCells, 0);
	// Dung vector lam hang doi BFS
	// Tranh cap phat bo nho nhieu lan khi push vao queue
	std::vector<Point> q;
	q.reserve(totalCells);
	q.push_back(start);
	visited[start.y * width + start.x] = 1;
	int front = 0;
	int count = 0;
	while (front < static_cast<int>(q.size())) {
		Point current = q[front++];
		count++;
		Point next[4] = {
			{current.x + 1, current.y},
			{current.x - 1, current.y},
			{current.x, current.y + 1},
			{current.x, current.y - 1}
		};
		for (int i = 0; i < 4; i++) {
			Point p = next[i];
			if (p.x < 0 || p.x >= width ||
				p.y < 0 || p.y >= height) {
				continue;
			}
			int index = p.y * width + p.x;
			if (blocked[index] || visited[index]) {
				continue;
			}
			visited[index] = 1;
			q.push_back(p);
		}
	}
	return count;
}
// dem huong di an toan
int SnakeAI::CountSafeDirections(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
) {
	Direction directions[4] = { UP, DOWN, LEFT, RIGHT };
	int count = 0;
	for (int i = 0; i < 4; i++) {
		Direction dir = directions[i];
		if (IsOppositeDirection(currentDirection, dir)) {
			continue;
		}
		Point newHead;
		std::vector<Point> newTail;
		bool ateFruit;
		SimulateMove(head, dir, fruit, tail,
			newHead, newTail, ateFruit);
		if (CheckValid(newHead, newTail, width, height)) {
			count++;
		}
	}
	return count;
}
// dem huoan an toan du doan
int SnakeAI::CountFutureOptions(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
) {
	Direction directions[4] = { UP, DOWN, LEFT, RIGHT };
	int viableDirections = 0;
	for (int i = 0; i < 4; i++) {
		Direction dir = directions[i];
		if (IsOppositeDirection(currentDirection, dir)) {
			continue;
		}
		Point newHead;
		std::vector<Point> newTail;
		bool ateFruit;
		SimulateMove(head, dir, fruit, tail,
			newHead, newTail, ateFruit);
		if (!CheckValid(newHead, newTail, width, height)) {
			continue;
		}
		int options = CountSafeDirections(
			newHead, fruit, newTail,
			dir, width, height);
		if (options > 0) {
			viableDirections++;
		}
	}
	return viableDirections;
}
// Chon huong di theo duoi ran
Direction SnakeAI::FindTailFollowingDirection(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
) {
	Direction directions[4] = { UP, DOWN, LEFT, RIGHT };
	Direction bestDirection = STOP;
	int bestScore = -1000000;
	for (int i = 0; i < 4; i++) {
		Direction dir = directions[i];
		// Khong quay nguoc dau
		if (IsOppositeDirection(currentDirection, dir)) {
			continue;
		}
		// Mo phong chuyen dong cua ran
		Point newHead;
		std::vector<Point> newTail;
		bool ateFruit;
		SimulateMove(head, dir, fruit,
			tail, newHead, newTail, ateFruit);
		// Bo qua nuoc di gay va cham
		if (!CheckValid(newHead, newTail, width, height)) {
			continue;
		}
		// Danh gia khong gian co the di toi
		int reachable = CountReachableCells(
			newHead, newTail, width, height);
		// Kiem tra co the di den duoi ran hay khong
		if (!CanReachTail(
			newHead, newTail, width, height)) {
			continue;
		}
		// Khong uu tien khu vuc qua chat
		if (reachable <= 1) {
			continue;
		}
		// Diem co ban: uu tien khu vuc rong
		int safeDirections = CountSafeDirections(
			newHead,fruit,newTail,
			dir,width,height);
		int futureOptions = CountFutureOptions(
			newHead, fruit, newTail,
			dir, width, height);
		int score = reachable * 10
			+ safeDirections * 15
			+ futureOptions * 10;
		// Han che cac vi tri chi con mot huong di an toan
		if (safeDirections <= 1) {
			score -= 40;
		}
		if (futureOptions == 0) {
			score -= 50;
		}
		// Thuong neu dau ran tien gan duoi ran
		if (!newTail.empty()) {
			Point tailEnd = newTail.back();
			int distanceToTail = abs(newHead.x - tailEnd.x)
				+ abs(newHead.y - tailEnd.y);
			score -= distanceToTail * 2;
		}
		// Phat neu khong gian kha dung qua nho
		if (reachable <= 5) {
			score -= 100;
		}
		// Chon nuoc di co diem cao nhat
		if (score > bestScore) {
			bestScore = score;
			bestDirection = dir;
		}
	}
	return bestDirection;
}
//tim duong an toan gan fruit nhat
Direction SnakeAI::FindSafeDirection(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
) {
	Direction directions[4] = { UP,DOWN,LEFT,RIGHT };
	Direction bestDirection = STOP;
	int bestScore = -1000000;
	int bestReachable = -1;
	int oldDistance = Distance(head, fruit);
	for (int i = 0;i < 4;i++) {
		Direction dir = directions[i];
		//khong duoc quay nguoc dau
		if (IsOppositeDirection(currentDirection, dir)) {
			continue;
		}
		//huong nay co an toan khong
		if (!IsSafeAfterMove(head, dir, fruit, tail, width, height)) {
			continue;
		}
		//mo phong nuoc di
		Point newHead;
		std::vector<Point> newTail;
		bool ateFruit;
		SimulateMove(head, dir, fruit, tail, newHead, newTail, ateFruit);
		//tinh diem cua tuong lai
		int futureScore = EvaluateFuture(newHead, fruit, newTail, dir, width, height, !ateFruit, 2);
		// Dem so o co the tiep can
		int reachable = CountReachableCells(newHead, newTail, width, height);
		// Thuong neu tien gan trai cay
		int fruitProgress = 0;
		if (!ateFruit) {
			int newDistance = Distance(newHead, fruit);
			fruitProgress = oldDistance - newDistance;
		}
		// Tong diem: song sot + tien gan trai cay
		int score = futureScore + fruitProgress * 3;
		// uu tien diem cao hon neu bang thi chon vung rong hon
		if (score > bestScore ||
			(score == bestScore && reachable > bestReachable)) {
			bestScore = score;
			bestReachable = reachable;
			bestDirection = dir;
		}
	}
	return bestDirection;
}
//kiem tra an toan sau khi di chuyen
bool SnakeAI::IsSafeAfterMove(
	Point head,
	Direction dir,
	Point fruit,
	const std::vector<Point>& tail,
	int width,
	int height
) {
	Point newHead;
	std::vector<Point> newTail;
	bool ateFruit;
	// Mo phong nuoc di tiep theo
	SimulateMove(head, dir, fruit, tail,
		newHead, newTail, ateFruit);
	// Chi kiem tra tuong va than ran
	return CheckValid(newHead, newTail, width, height);
}
//kiem tra sau khi an
bool SnakeAI::IsSafeAfterEating(
	Point head,
	Direction dir,
	Point fruit,
	const std::vector<Point>& tail,
	int width,
	int height
) {
	Point newHead;
	std::vector<Point> newTail;
	bool ateFruit;
	// Mo phong nuoc di
	SimulateMove(head, dir, fruit, tail, newHead, newTail, ateFruit);
	// chi kiem tra nuoc di an trai cay
	if (!ateFruit) {
		return true;
	}
	//kiem tra dau moi va than moi sau khi an
	if (!CheckValid(newHead, newTail, width, height)) {
		return false;
	}
	// xac minh trang thai sau khi an
	return IsSafeAfterEatingState(
		newHead, newTail, width, height);
}
// mo phong duong an toan A*
bool SnakeAI::IsPathSafe(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	const std::vector<Direction>& path,
	int width,
	int height
) {
	// Kiem tra kich thuoc ban do
	if (width <= 0 || height <= 0) {
		return false;
	}
	// Khong co duong di thi khong chap nhan
	if (path.empty()) {
		return false;
	}
	// Sao chep trang thai ban dau
	Point simulatedHead = head;
	std::vector<Point> simulatedTail = tail;
	Direction simulatedDirection = currentDirection;
	Point targetFruit = fruit;
	for (Direction dir : path) {
		// Khong cho phep dung hoac quay nguoc dau
		if (dir == STOP ||
			IsOppositeDirection(simulatedDirection, dir)) {
			return false;
		}
		Point newHead;
		std::vector<Point> newTail;
		bool ateFruit = false;
		// Mo phong mot buoc
		SimulateMove(simulatedHead, dir, targetFruit,
			simulatedTail, newHead, newTail, ateFruit);
		// Kiem tra tuong va than ran sau buoc di
		if (!CheckValid(newHead, newTail, width, height)) {
			return false;
		}
		// Cap nhat trang thai sau buoc di
		simulatedHead = newHead;
		simulatedTail = std::move(newTail);
		simulatedDirection = dir;
		// Neu an fruit kiem tra trang thai sau khi an
		if (ateFruit) {
			targetFruit = { -1, -1 };
			if (!IsSafeAfterEatingState(
				simulatedHead,simulatedTail,
				width, height)) {
				return false;
			}
		}
	}
	// Duong di hop le den cuoi
	return true;
}
// Kiem tra trang thai ran sau khi an fruit
bool SnakeAI::IsSafeAfterEatingState(
	Point head,
	const std::vector<Point>& tail,
	int width,
	int height
) {
	// 1. Kiem tra dau vao
	if (width <= 0 || height <= 0) {
		return false;
	}
	// 2. Kiem tra dau ran va than ran
	if (!CheckValid(head, tail, width, height)) {
		return false;
	}
	// 3. Uoc luong khong gian co the di toi
	int reachable = CountReachableCells(
		head, tail, width, height);
	// 4. Can co du khong gian de di chuyen
	if (reachable <= 2) {
		return false;
	}
	// 5. Kiem tra kha nang tiep can duoi ran
	return CanReachTail(head, tail, width, height);
}
//mo phong duong di
void SnakeAI::SimulateMove(
	Point head,
	Direction dir,
	Point fruit,
	const std::vector<Point>& tail,
	Point& newHead,
	std::vector<Point>& newTail,
	bool& ateFruit
) {
	// 1. Tinh vi tri dau moi
	newHead = head;
	switch (dir) {
	case UP:    newHead.y--; break;
	case DOWN:  newHead.y++; break;
	case LEFT:  newHead.x--; break;
	case RIGHT: newHead.x++; break;
	default: break;
	}
	// 2. Kiem tra an fruit
	ateFruit = (newHead.x == fruit.x &&
		newHead.y == fruit.y);
	// 3. Tao than moi
	newTail.clear();
	newTail.push_back(head);
	if (ateFruit) {
		// An fruit: giu toan bo than cu
		newTail.insert(newTail.end(),
			tail.begin(),tail.end());
	}
	else if (!tail.empty()) {
		// Khong an fruit: bo phan duoi cuoi
		newTail.insert(newTail.end(),
			tail.begin(),tail.end() - 1);
	}
}
//diem so du doan cua trang thai tuong lai
int SnakeAI::EvaluateFuture(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height,
	bool fruitAvailable,
	int depth
) {
	// Da du do sau du doan
	if (depth == 0) {
		return EvaluatePosition(
			head, fruit, tail, width, height, fruitAvailable
		);
	}
	Direction directions[4] = { UP, DOWN, LEFT, RIGHT };
	int bestScore = -1000000;
	// Neu fruit da bi an, khong su dung toa do fruit cu
	Point targetFruit = fruit;
	if (!fruitAvailable) {
		targetFruit = { -1, -1 };
	}
	for (int i = 0; i < 4; i++) {
		Direction dir = directions[i];
		// Khong cho phep quay nguoc dau
		if (IsOppositeDirection(currentDirection, dir)) {
			continue;
		}
		// Mo phong nuoc di mot lan
		Point newHead;
		std::vector<Point> newTail;
		bool ateFruit;
		SimulateMove(
			head, dir, targetFruit, tail,
			newHead, newTail, ateFruit
		);
		// Kiem tra tuong va than ran sau khi mo phong
		if (!CheckValid(newHead, newTail, width, height)) {
			continue;
		}
		// Danh gia trang thai tiep theo
		int futureScore = EvaluateFuture(
			newHead,
			fruit,
			newTail,
			dir,
			width,
			height,
			fruitAvailable && !ateFruit,
			depth - 1
		);
		// Thuong diem neu an duoc fruit
		if (ateFruit) {
			futureScore += 20;
		}
		if (futureScore > bestScore) {
			bestScore = futureScore;
		}
	}
	// Khong co nuoc di hop le
	if (bestScore == -1000000) {
		return -100000;
	}
	return bestScore;
}
//danh gia trang thai
int SnakeAI::EvaluatePosition(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	int width,
	int height,
	bool fruitAvailable
) {
	int reachable = CountReachableCells(head, tail, width, height);
	//khong gian qua hep
	if (reachable <= 2) {
		return -10000 + reachable;
	}
	int score = reachable * 10;
	// Phat them neu khong gian di chuyen qua nho
	if (reachable <= 5) {
		score -= 100;
	}
	else if (reachable <= 10) {
		score -= 30;
	}
	//chi danh gia khoang cach neu fruit con ton tai
	if (fruitAvailable) {
		score -= Distance(head, fruit);
	}
	return score;
}
// Ham AI chinh
Direction SnakeAI::GetNextDirection(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
) {
	// Buoc 1: Neu moi doi vi tri, xoa cache cu
	if (cachedFruit.x != fruit.x ||
		cachedFruit.y != fruit.y) {
		// Chi xoa duong di khong con an toan
		// Khong reset trang thai tim duong that bai
		cachedPath.clear();
		pathIndex = 0;
		cachedFruit = fruit;
		cacheInvalidationCount++;
	}
	// Buoc 2: Neu lan truoc tim duong that bai,
	// cho AI di an toan them mot so luot
	if (lastSearchFailed && failedSearchCooldown > 0) {
		failedSearchCooldown--;
		Direction tailDirection = FindTailFollowingDirection(
			head, fruit, tail, currentDirection, width, height);
		if (tailDirection != STOP) {
			return tailDirection;
		}
		return FindSafeDirection(
			head, fruit, tail, currentDirection, width, height);
	}
	// Buoc 3: Neu khong con duong trong cache,
	// hoac cache da duoc su dung het, tim duong moi
	if (pathIndex >= cachedPath.size()) {
		cachedPath = FindPathAStarPath(
			head, fruit, tail, currentDirection, width, height);
		pathIndex = 0;
		cachedFruit = fruit;
		// Buoc 4: Kiem tra toan bo duong di A*
		if (!cachedPath.empty() &&
			!IsPathSafe(head, fruit, tail, currentDirection,
				cachedPath, width, height)) {
			cachedPath.clear();
			cacheInvalidationCount++;
		}
		// Neu khong co duong di hop le
		if (cachedPath.empty()) {
			lastSearchFailed = true;
			failedSearchCooldown = 3;
			return FindSafeDirection(head, fruit, tail,
				currentDirection, width, height);
		}
		// Tim thay duong di moi
		lastSearchFailed = false;
		failedSearchCooldown = 0;
	}
	// Buoc 5: Lay huong di tiep theo trong cache
	if (pathIndex < cachedPath.size()) {
		Direction result = cachedPath[pathIndex];
		if (result != STOP &&
			!IsOppositeDirection(currentDirection, result)) {
			if (IsSafeAfterMove(
				head, result, fruit, tail, width, height
			)) {
				if (IsSafeAfterEating(
					head, result, fruit, tail, width, height
				)) {
					pathIndex++;
					return result;
				}
			}
		}
		// Duong di trong cache khong con an toan
		cachedPath.clear();
		pathIndex = 0;
		cachedFruit = fruit;
		cacheInvalidationCount++;
		lastSearchFailed = true;
		failedSearchCooldown = 3;
	}
	// Buoc 6: Neu khong dung duoc cache,chon huong di an toan
	// A* khong tim duoc duong an toan thu bam theo duoi ran truoc
	Direction tailDirection = FindTailFollowingDirection(head, fruit, tail,
		currentDirection, width, height);
	if (tailDirection != STOP) {
		return tailDirection;
	}
	// Phuong an du phong cuoi cung
	return FindSafeDirection(head, fruit, tail,
		currentDirection, width, height);
}