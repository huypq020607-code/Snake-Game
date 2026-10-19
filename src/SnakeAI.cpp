#include "../include/SnakeAI.h"
#include <vector>
#include <queue>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include <chrono>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
bool IsOppositeDirection(Direction current, Direction next);
//xoa duong di da luu
void SnakeAI::ClearCachedPath() {
	cachedPath.clear();
	pathIndex = 0;
	cachedFruit = { -1, -1 };
	lastSearchFailed = false;
	failedSearchCooldown = 0;
	cacheInvalidationCount++;
}
// kiem tra point co nam tren tail khong
bool CheckTail(
	Point p,
	const std::vector<Point>& tail
) {
	for (int i = 0; i < tail.size(); i++) {
		if (p.x == tail[i].x && p.y == tail[i].y) {
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
//kiem tra huong di chuyen co an toan khong
bool SnakeAI::CheckSafeMove(
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
	// Kiem tra dau moi va than ran moi
	return CheckValid(newHead, newTail, width, height);
}
// BFS tim duong tu head->fruit
Direction SnakeAI::FindPathBFS(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
) {
	// Kiem tra toa do dau vao
	if (width <= 0 || height <= 0) {
		return STOP;
	}
	if (head.x < 0 || head.x >= width ||
		head.y < 0 || head.y >= height) {
		return STOP;
	}
	if (fruit.x < 0 || fruit.x >= width ||
		fruit.y < 0 || fruit.y >= height) {
		return STOP;
	}
	std::queue<SearchState> q;
	std::unordered_set<std::string> visited;
	// Tao khoa trang thai bang cac gia tri int dang nhi phan
	auto makeKey = [](const SearchState& state) {
		std::string key;
		// Moi int chiem sizeof(int) byte
		key.reserve(
			sizeof(int) * (3 + state.tail.size() * 2)
		);
		auto appendInt = [&key](int value) {
			key.append(
				reinterpret_cast<const char*>(&value),
				sizeof(value)
			);
			};
		appendInt(state.head.x);
		appendInt(state.head.y);
		appendInt(static_cast<int>(state.currentDirection));
		for (const Point& part : state.tail) {
			appendInt(part.x);
			appendInt(part.y);
		}
		return key;
		};
	// Trang thai ban dau
	SearchState start;
	start.head = head;
	start.tail = tail;
	start.firstDirection = STOP;
	start.currentDirection = currentDirection;
	q.push(std::move(start));
	visited.insert(makeKey(q.front()));
	Direction directions[4] = {
		UP, DOWN, LEFT, RIGHT
	};
	const int MAX_STATES = 5000;
	int statesChecked = 0;
	while (!q.empty() && statesChecked < MAX_STATES) {
		// Di chuyen trang thai ra khoi queue, tranh sao chep vector
		SearchState current = std::move(q.front());
		q.pop();
		statesChecked++;
		// Da den fruit
		if (current.head.x == fruit.x &&
			current.head.y == fruit.y) {
			return current.firstDirection;
		}
		for (int i = 0; i < 4; i++) {
			Direction dir = directions[i];
			// Khong duoc quay dau
			if (current.currentDirection != STOP &&
				IsOppositeDirection(
					current.currentDirection, dir
				)) {
				continue;
			}
			Point newHead;
			std::vector<Point> newTail;
			bool ateFruit;
			// Mo phong than ran sau mot nuoc di
			SimulateMove(
				current.head,
				dir,
				fruit,
				current.tail,
				newHead,
				newTail,
				ateFruit
			);
			// Kiem tra tuong va than ran
			if (!CheckValid(
				newHead, newTail, width, height
			)) {
				continue;
			}
			SearchState nextState;
			nextState.head = newHead;
			nextState.tail = std::move(newTail);
			// Luu huong di dau tien
			if (current.firstDirection == STOP) {
				nextState.firstDirection = dir;
			}
			else {
				nextState.firstDirection =
					current.firstDirection;
			}
			nextState.currentDirection = dir;
			// Them trang thai neu chua duoc tham
			std::string key = makeKey(nextState);
			if (!visited.insert(std::move(key)).second) {
				continue;
			}
			q.push(std::move(nextState));
		}
	}
	return STOP;
}
Direction SnakeAI::FindPathAStar(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
) {
	if (width <= 0 || height <= 0 ||
		head.x < 0 || head.x >= width ||
		head.y < 0 || head.y >= height ||
		fruit.x < 0 || fruit.x >= width ||
		fruit.y < 0 || fruit.y >= height) {
		return STOP;
	}
	// Uoc luong khoang cach Manhattan
	auto heuristic = [&fruit](Point p) {
		return std::abs(p.x - fruit.x)
			+ std::abs(p.y - fruit.y);
		};
	// Tao khoa trang thai nhi phan
	auto makeKey = [](const SearchState& state) {
		std::string key;
		key.reserve(
			sizeof(int) * (3 + state.tail.size() * 2)
		);
		auto appendInt = [&key](int value) {
			key.append(
				reinterpret_cast<const char*>(&value),
				sizeof(value)
			);
			};
		appendInt(state.head.x);
		appendInt(state.head.y);
		appendInt(static_cast<int>(state.currentDirection));
		for (const Point& part : state.tail) {
			appendInt(part.x);
			appendInt(part.y);
		}
		return key;
		};
	// Priority queue: uu tien node co f = g + h nho nhat
	struct Compare {
		bool operator()(
			const AStarNode& a,
			const AStarNode& b
			) const {
			return a.g + a.h > b.g + b.h;
		}
	};
	std::priority_queue<
		AStarNode,
		std::vector<AStarNode>,
		Compare
	> open;
	std::unordered_set<std::string> visited;
	SearchState start;
	start.head = head;
	start.tail = tail;
	start.firstDirection = STOP;
	start.currentDirection = currentDirection;
	open.push({ start, 0, heuristic(head) });
	visited.insert(makeKey(start));
	Direction directions[4] = {
		UP, DOWN, LEFT, RIGHT
	};
	const int MAX_STATES = 500;
	const int MAX_DISCOVERED = 1000;
	int statesChecked = 0;
	int statesDiscovered = 1;
	while (!open.empty() && statesChecked < MAX_STATES) {
		AStarNode node = open.top();
		open.pop();
		SearchState current = std::move(node.state);
		statesChecked++;
		if (current.head.x == fruit.x &&
			current.head.y == fruit.y) {
			return current.firstDirection;
		}
		for (Direction dir : directions) {
			if (current.currentDirection != STOP &&
				IsOppositeDirection(
					current.currentDirection, dir
				)) {
				continue;
			}
			Point newHead;
			std::vector<Point> newTail;
			bool ateFruit;
			SimulateMove(
				current.head,
				dir,
				fruit,
				current.tail,
				newHead,
				newTail,
				ateFruit
			);
			if (!CheckValid(newHead, newTail, width, height)) {
				continue;
			}
			SearchState next;
			next.head = newHead;
			next.tail = std::move(newTail);
			next.currentDirection = dir;
			next.firstDirection =
				(current.firstDirection == STOP)
				? dir
				: current.firstDirection;
			std::string key = makeKey(next);
			if (visited.find(key) != visited.end()) {
				continue;
			}
			// Gioi han tong so trang thai da kham pha
			if (statesDiscovered >= MAX_DISCOVERED) {
				continue;
			}
			visited.insert(std::move(key));
			int nextG = node.g + 1;
			int nextH = heuristic(newHead);

			open.push({
				std::move(next),
			 nextG,
				nextH
				});
			statesDiscovered++;
		}
	}
	return STOP;
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
	// Kiem tra dau vao
	if (width <= 0 || height <= 0 ||
		head.x < 0 || head.x >= width ||
		head.y < 0 || head.y >= height ||
		fruit.x < 0 || fruit.x >= width ||
		fruit.y < 0 || fruit.y >= height) {
		return {};
	}
	// Uoc luong khoang cach Manhattan
	auto heuristic = [&fruit](Point p) {
		return abs(p.x - fruit.x)
			+ abs(p.y - fruit.y);
		};
	// Tao khoa trang thai
	auto makeKey = [](const SearchState& state) {
		std::string key;
		key.reserve(
			sizeof(int) * (3 + state.tail.size() * 2)
		);
		auto appendInt = [&key](int value) {
			key.append(
				reinterpret_cast<const char*>(&value),
				sizeof(value)
			);
			};
		appendInt(state.head.x);
		appendInt(state.head.y);
		appendInt(
			static_cast<int>(state.currentDirection)
		);
		for (const Point& part : state.tail) {
			appendInt(part.x);
			appendInt(part.y);
		}
		return key;
		};
	// Node co f = g + h nho hon duoc uu tien
	struct Compare {
		bool operator()(
			const AStarNode& a,
			const AStarNode& b
			) const {
			return a.g + a.h > b.g + b.h;
		}
	};
	std::priority_queue<
		AStarNode,
		std::vector<AStarNode>,
		Compare
	> open;
	std::unordered_map<std::string, int> bestG;
	// Trang thai ban dau
	SearchState start;
	start.head = head;
	start.tail = tail;
	start.firstDirection = STOP;
	start.currentDirection = currentDirection;
	// Ban dau chua di buoc nao nen path rong
	open.push({ start, 0, heuristic(head), {} });
	bestG[makeKey(start)] = 0;
	Direction directions[4] = {
		UP, DOWN, LEFT, RIGHT
	};
	const int MAX_STATES = 500;
	const int MAX_DISCOVERED = 1000;
	int statesChecked = 0;
	int statesDiscovered = 1;
	while (!open.empty() && statesChecked < MAX_STATES) {
		AStarNode node = open.top();
		open.pop();
		// Kiem tra node co con la duong tot nhat khong
		std::string currentKey = makeKey(node.state);
		auto it = bestG.find(currentKey);
		if (it == bestG.end() || node.g != it->second) {
			continue;
		}
		statesChecked++;
		SearchState current = node.state;
		// Da den fruit
		if (current.head.x == fruit.x &&
			current.head.y == fruit.y) {
			return node.path;
		}
		for (Direction dir : directions) {
			// Khong cho ran quay nguoc truc tiep
			if (current.currentDirection != STOP &&
				IsOppositeDirection(current.currentDirection, dir)) {
				continue;
			}
			Point newHead;
			std::vector<Point> newTail;
			bool ateFruit;
			SimulateMove(
				current.head,
				dir,
				fruit,
				current.tail,
				newHead,
				newTail,
				ateFruit
			);
			if (!CheckValid(newHead, newTail, width, height)) {
				continue;
			}
			SearchState next;
			next.head = newHead;
			next.tail = std::move(newTail);
			next.currentDirection = dir;
			next.firstDirection =
				(current.firstDirection == STOP)
				? dir
				: current.firstDirection;
			std::string key = makeKey(next);
			int nextG = node.g + 1;
			auto found = bestG.find(key);
			// Da co duong den trang thai nay tot hon
			if (found != bestG.end() &&
				nextG >= found->second) {
				continue;
			}
			// Chi gioi han khi phat hien trang thai moi
			if (found == bestG.end() &&
				statesDiscovered >= MAX_DISCOVERED) {
				continue;
			}
			// Cap nhat chi phi tot nhat
			if (found == bestG.end()) {
				statesDiscovered++;
			}
			bestG[key] = nextG;
			int nextH = heuristic(newHead);
			std::vector<Direction> nextPath = node.path;
			nextPath.push_back(dir);
			open.push({
				std::move(next),
				nextG,
				nextH,
				std::move(nextPath)
				});
		}
	}
	// Khong tim thay duong di
	return {};
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
	if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) {
		return 0;
	}
	std::queue<Point> q;
	std::vector<std::vector<bool>> visited(
		height, std::vector<bool>(width, false));
	q.push(start);
	visited[start.y][start.x] = true;
	int count = 0;
	while (!q.empty()) {
		Point current = q.front();
		q.pop();
		count++;
		Point next[4] = {
			{current.x + 1,current.y},
			{current.x - 1,current.y},
			{current.x,current.y + 1},
			{current.x,current.y - 1}
		};
		for (int i = 0;i < 4;i++) {
			Point p = next[i];
			if (!CheckValid(p, tail, width, height)) {
				continue;
			}
			if (visited[p.y][p.x]) {
				continue;
			}
			visited[p.y][p.x] = true;
			q.push(p);
		}
	}
	return count;
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
	for (int i = 0;i < 4;i++) {
		Direction dir = directions[i];
		//khong duoc quay nguoc dau
		if (IsOppositeDirection(currentDirection, dir)) {
			continue;
		}
		//huong nay co an toan khong
		if (!CheckSafeMove(head, dir, fruit, tail, width, height)) {
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
		// uu tien diem cao hon neu bang thi uu tien o rong hon
		if (futureScore > bestScore ||
			(futureScore == bestScore && reachable > bestReachable)) {
			bestScore = futureScore;
			bestReachable = reachable;
			bestDirection = dir;
		}
	}
	return bestDirection;
}
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
	//mo phong nuoc di
	SimulateMove(head, dir, fruit, tail, newHead, newTail, ateFruit);
	//kiem tra tuong va than ran
	if (!CheckValid(newHead, newTail, width, height)) {
		return false;
	}
	//kiem tra sau khi di con khong gian di chuyen khong
	int reachable = CountReachableCells(newHead, newTail, width, height);
	return reachable > 1;
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
	SimulateMove(head, dir, fruit, tail, newHead, newTail, ateFruit);
	//chi kiem tra nuoc di an trai cay
	if (!ateFruit) {
		return true;
	}
	//kiem tra dau moi va than moi sau khi an
	if (!CheckValid(newHead, newTail, width, height)) {
		return false;
	}
	//uoc luong khong gian co the di toi
	int reachable = CountReachableCells(newHead, newTail, width, height);
	return reachable > 2;
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
	//tinh vi tri dau moi
	newHead = head;
	switch (dir) {
	case UP:
		newHead.y--;
		break;
	case DOWN:
		newHead.y++;
		break;
	case LEFT:
		newHead.x--;
		break;
	case RIGHT:
		newHead.x++;
		break;
	default:
		break;
	}
	//kiem tra co an fruit khong
	ateFruit = (newHead.x == fruit.x && newHead.y == fruit.y);
	//xoa tail cu
	newTail.clear();
	//dau cu tro thanh phan than dau tien
	newTail.push_back(head);
	//di chuyen toan than
	if (ateFruit) {
		//an fruit giu lai toan bo than cu
		for (const auto& part : tail) {
			newTail.push_back(part);
		}
	}
	else {
		//khong an bo phan duoi cuoi
		for (int i = 0;i < (int)tail.size() - 1;i++) {
			newTail.push_back(tail[i]);
		}
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
	// da nhin du so buoc
	if (depth == 0) {
		return EvaluatePosition(head, fruit, tail, width, height, fruitAvailable);
	}
	Direction directions[4] = { UP,DOWN,LEFT,RIGHT };
	int bestScore = -1000000;
	for (int i = 0;i < 4;i++) {
		Direction dir = directions[i];
		//khong quay dau
		if (IsOppositeDirection(currentDirection, dir)) {
			continue;
		}
		//neu fruit da bi an khong dung lai toa do cu
		Point targetFruit = fruit;
		if (!fruitAvailable) {
			targetFruit = { -1,-1 };
		}
		//nuoc di hien tai co an toan khong
		if (!CheckSafeMove(head, dir, targetFruit, tail, width, height)) {
			continue;
		}
		//mo phong
		Point newHead;
		std::vector<Point> newTail;
		bool ateFruit;
		SimulateMove(head, dir, targetFruit, tail, newHead, newTail, ateFruit);
		//ra ngoai ban do
		if (newHead.x < 0 || newHead.x >= width || newHead.y < 0 || newHead.y >= height) {
			continue;
		}
		// Kiem tra dau moi co nam trong ban do va co va cham voi than ran moi hay khong
		if (!CheckValid(newHead, newTail, width, height)) {
			continue;
		}
		//Neu an fruit thi cac buoc sau khong danh gia theo vi tri fruit cu nua
		int futureScore = EvaluateFuture(newHead, fruit, newTail, dir, width, height, fruitAvailable && !ateFruit, depth - 1);
		if (ateFruit) {
			futureScore += 20;
		}
		if (futureScore > bestScore) {
			bestScore = futureScore;
		}
	}
	// Khong tim duoc nuoc di hop le
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
		ClearCachedPath();
		cachedFruit = fruit;
	}
	// Buoc 2: Neu lan truoc tim duong that bai,
	// cho AI di an toan them mot so luot
	if (lastSearchFailed && failedSearchCooldown > 0) {
		failedSearchCooldown--;
		return FindSafeDirection(
			head, fruit, tail, currentDirection, width, height
		);
	}
	// Buoc 3: Neu khong con duong trong cache,
	// hoac cache da duoc su dung het, tim duong moi
	if (pathIndex >= cachedPath.size()) {
		cachedPath = FindPathAStarPath(
			head, fruit, tail, currentDirection, width, height
		);
		pathIndex = 0;
		cachedFruit = fruit;
		// Buoc 4: Neu A* khong tim thay duong
		if (cachedPath.empty()) {
			lastSearchFailed = true;
			failedSearchCooldown = 3;
			return FindSafeDirection(
				head, fruit, tail, currentDirection, width, height
			);
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
		ClearCachedPath();
		cachedFruit = fruit;
	}
	// Buoc 6: Neu khong dung duoc cache,
	// chon huong di an toan
	return FindSafeDirection(
		head, fruit, tail, currentDirection, width, height
	);
}