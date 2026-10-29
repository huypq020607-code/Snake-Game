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
#include <utility>
//dinh nghia ham 
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
	auto makeKey = [this](const SearchState& state) {
		keyGenerationCount++;
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
	auto startTime = std::chrono::steady_clock::now();
	// 1. Kiem tra dau vao
	if (width <= 0 || height <= 0 ||
		head.x < 0 || head.x >= width ||
		head.y < 0 || head.y >= height ||
		fruit.x < 0 || fruit.x >= width ||
		fruit.y < 0 || fruit.y >= height) {
		return {};
	}
	// 2. Heuristic Manhattan
	auto heuristic = [&fruit](Point p) {
		return abs(p.x - fruit.x)
			+ abs(p.y - fruit.y);
		};
	// 3. Tao khoa duy nhat cho trang thai
	auto makeKey = [this](const SearchState& state) {
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
	// 4. Nut trong hang doi chi can luu chi so va f
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
	std::priority_queue<
		OpenEntry,
		std::vector<OpenEntry>,
		Compare
	> open;
	// Luu cac nut A* tai day.
	// ParentIndex tro den nut cha trong vector nay.
	std::vector<AStarNode> nodes;
	const int MAX_STATES = 1200;
	const int MAX_DISCOVERED = 2400;
	// Moi lan mo rong co toi da 4 huong.
	// Du tru bo nho, khong thay doi gioi han tim kiem.
	nodes.reserve(
		1 + MAX_STATES * 4
	);
	std::unordered_map<std::string, int> bestG;
	// 5. Khoi tao trang thai ban dau
	SearchState start;
	start.head = head;
	start.tail = tail;
	start.firstDirection = STOP;
	start.currentDirection = currentDirection;
	AStarNode startNode;
	startNode.state = start;
	startNode.g = 0;
	startNode.h = heuristic(head);
	startNode.parentIndex = -1;
	startNode.directionFromParent = STOP;
	nodes.push_back(std::move(startNode));
	int startH = heuristic(head);
	open.push({ 0,startH,startH });
	bestG[makeKey(start)] = 0;
	Direction directions[4] = {
		UP, DOWN, LEFT, RIGHT
	};
	int statesChecked = 0;
	int statesDiscovered = 1;
	bool discoveryLimitHit = false;
	// 6. Bat dau tim kiem
	while (!open.empty() &&
		statesChecked < MAX_STATES) {
		OpenEntry entry = open.top();
		open.pop();
		int nodeIndex = entry.nodeIndex;
		// Lay nut hien tai tu danh sach
		const AStarNode& savedNode = nodes[nodeIndex];
		// Bo qua nut khong con la duong tot nhat
		std::string currentKey =
			makeKey(savedNode.state);
		auto it = bestG.find(currentKey);
		if (it == bestG.end() ||
			savedNode.g != it->second) {
			continue;
		}
		statesChecked++;
		// Sao chep trang thai hien tai,
		// tranh bi anh huong khi them nut vao vector.
		SearchState current = savedNode.state;
		int currentG = savedNode.g;
		// 7. Neu da den qua, khoi phuc duong di
		if (current.head.x == fruit.x &&
			current.head.y == fruit.y) {
			std::vector<Direction> path;
			int index = nodeIndex;
			// Truy nguoc tu dich ve diem xuat phat
			while (nodes[index].parentIndex != -1) {
				path.push_back(
					nodes[index].directionFromParent);
				index = nodes[index].parentIndex;
			}
			// Dao nguoc de duong di theo thu tu xuat phat -> dich
			std::reverse(path.begin(), path.end());
			pathFoundCount++;
			return path;
		}
		// 8. Thu cac huong di
		for (Direction dir : directions) {
			// Khong cho ran quay nguoc truc tiep
			if (current.currentDirection != STOP &&
				IsOppositeDirection(
					current.currentDirection,
					dir)) {
				continue;
			}
			Point newHead;
			std::vector<Point> newTail;
			bool ateFruit;
			// Giu nguyen logic mo phong
			SimulateMove(
				current.head,
				dir,
				fruit,
				current.tail,
				newHead,
				newTail,
				ateFruit
			);
			// Giu nguyen kiem tra an toan
			if (!CheckValid(
				newHead,
				newTail,
				width,
				height
			)) {
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
			int nextG = currentG + 1;
			auto found = bestG.find(key);
			// Da co duong den trang thai nay tot hon
			if (found != bestG.end() &&
				nextG >= found->second) {
				continue;
			}
			// Giu nguyen gioi han so trang thai moi
			if (found == bestG.end() &&
				statesDiscovered >= MAX_DISCOVERED) {
				discoveryLimitHit = true;
				continue;
			}
			if (found == bestG.end()) {
				statesDiscovered++;
			}
			bestG[key] = nextG;
			int nextH = heuristic(newHead);
			// Luu nut moi va lien ket den nut cha
			int nextIndex = static_cast<int>(
				nodes.size()
				);
			AStarNode nextNode;
			nextNode.state = std::move(next);
			nextNode.g = nextG;
			nextNode.h = nextH;
			nextNode.parentIndex = nodeIndex;
			nextNode.directionFromParent = dir;
			nodes.push_back(std::move(nextNode));
			// Hang doi chi luu chi so nut va f
			open.push({ nextIndex,nextG + nextH, nextH });
		}
	}
	bool stateLimitHit = (statesChecked >= MAX_STATES);
	if (stateLimitHit) {
		stateLimitCount++;
	}
	if (discoveryLimitHit) {
		discoveryLimitCount++;
	}
	if (stateLimitHit && discoveryLimitHit) {
		bothLimitCount++;
	}
	else if (!stateLimitHit && !discoveryLimitHit) {
		noPathCount++;
	}
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
	// Mac dinh fruit van con ton tai
	bool fruitAvailable = true;
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
			fruitAvailable = false;
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
	// 2. Kiem tra co an fruit khong
	ateFruit = (newHead.x == fruit.x &&
		newHead.y == fruit.y);
	// 3. Tao than moi
	newTail.clear();
	newTail.reserve(tail.size() + 1);
	// Dau cu tro thanh phan than dau tien
	newTail.push_back(head);
	if (ateFruit) {
		// An fruit: giu lai toan bo than cu
		for (const auto& part : tail) {
			newTail.push_back(part);
		}
	}
	else {
		// Khong an fruit:
		// Bo phan duoi cuoi vi duoi se di chuyen
		for (int i = 0; i < (int)tail.size() - 1; i++) {
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