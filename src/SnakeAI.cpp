#include "../include/SnakeAI.h"
#include <vector>
#include <queue>
#include <cstdlib>
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
	//kiem tra tail
	if (CheckTail(p, tail)) {
		return false;
	}
	return true;
}
//kiem tra huong di chuyen co an toan khong
bool SnakeAI::CheckSafeMove(
	Point head,
	Direction dir,
	const std::vector<Point>& tail,
	int width,
	int height
) {
	Point next = head;
	switch (dir) {
	case LEFT:
		next.x--;
		break;
	case RIGHT:
		next.x++;
		break;
	case UP:
		next.y--;
		break;
	case DOWN:
		next.y++;
		break;
	default:
		break;
	}
	return CheckValid(
		next,
		tail,
		width,
		height
	);
}
// BFS tim duong tu head->fruit
Direction SnakeAI::FindPathBFS(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	int width,
	int height
) {
	std::queue<Point> q;
	bool visited[24][24] = {};
	Direction firstDirection[24][24];
	// bat dau tai Head
	q.push(head);
	visited[head.y][head.x] = true;
	while (!q.empty()) {
		Point current = q.front();
		q.pop();
		//neu da toi Fruit
		if (current.x == fruit.x && current.y == fruit.y) {
			if (current.x == head.x && current.y == head.y) {
				return STOP;
			}
			return firstDirection[current.y][current.x];
		}
		// 4 vi tri tiep theo
		Point next[4] = {
			{current.x + 1, current.y},
			{current.x - 1, current.y},
			{current.x, current.y + 1},
			{current.x, current.y - 1}
		};
		// 4 huong di tuong ung
		Direction directions[4] = {
			RIGHT,
			LEFT,
			DOWN,
			UP
		};
		for (int i = 0; i < 4; i++) {
			Point p = next[i];
			// Khong di duoc
			if (!CheckValid(p, tail, width, height)) {
				continue;
			}
			// da di den
			if (visited[p.y][p.x]) {
				continue;
			}
			visited[p.y][p.x] = true;
			// day la buoc dau tien tu head
			if (current.x == head.x && current.y == head.y) {
				firstDirection[p.y][p.x] = directions[i];
			}
			// giu nguyen huong dau tien
			else {
				firstDirection[p.y][p.x] =
					firstDirection[current.y][current.x];
			}
			q.push(p);
		}
	}
	//Khong co duong
	return STOP;
}
//kiem tra hai huong co nguoc nhau khong
bool IsOppositeDirection(
	Direction current,
	Direction next
){
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
	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}
//kiem tra khi an fruit con bao nhieu o co the di toi
int SnakeAI::CountReachableCells(
	Point start,
	const std::vector<Point>& tail,
	int width,
	int height
){
	if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) {
		return 0;
	}
	std::queue<Point> q;
	bool visited[24][24] = {};
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
){
	Direction directions[4] = { UP,DOWN,RIGHT,LEFT };
	Direction bestDirection = STOP;
	int bestScore = -1000000;
	for (int i = 0;i < 4;i++) {
		Direction dir = directions[i];
		//khong duoc quay nguoc dau
		if (IsOppositeDirection(currentDirection, dir)) {
			continue;
		}
		//huong nay co an toan khong
		if (!CheckSafeMove(head, dir, tail, width, height)) {
			continue;
		}
		//mo phong nuoc di
		Point newHead;
		std::vector<Point> newTail;
		bool ateFruit;
		SimulateMove(head, dir, fruit, tail, newHead, newTail, ateFruit);
		//kiem tra sau nuoc di nay co an toan khong
		if (!IsSafeAfterMove(head, dir, fruit, tail, width, height)) {
			continue;
		}
		//dem so o co the di toi
		int reachable = CountReachableCells(newHead, newTail, width, height);
		//tinh khoang cach toi fruit
		int distance = Distance(newHead, fruit);
		/* Diem danh gia:
			+ Vung di duoc cang lon -> cang tot
			+ Gan Fruit -> cang tot
			+ reachable * 10
			+ uu tien hon distance
		*/
		int score = reachable * 10 - distance;
		if (score > bestScore) {
			bestScore = score;
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
){
	Point newHead;
	std::vector<Point> newTail;
	bool ateFruit;
	//mo phong nuoc di
	SimulateMove(head, dir, fruit, tail, newHead, newTail, ateFruit);
	//kiem tra tuong
	if (newHead.x < 0 || newHead.x >= width || newHead.y < 0 || newHead.y >= height) {
		return false;
	}
	//kiem tra dau moi co dam vao than moi khong
	for (const auto& part : newTail) {
		if (newHead.x == part.x && newHead.y == part.y) {
			return false;
		}
	}
	//kiem tra sau khi di con khong gian di chuyen khong
	int reachable = CountReachableCells(newHead, newTail, width, height);
	//neu con 1 o thi rat nguy hiem
	if (reachable <= 1) {
		return false;
	}
	return true;
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
){
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
// Ham AI chinh
Direction SnakeAI::GetNextDirection(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
){
	Direction result = FindPathBFS(head, fruit, tail, width, height);
	// BFS tim duoc duong
	if (result != STOP &&
		!IsOppositeDirection(currentDirection, result)) {
		//kiem tra co an toan khong
		if (IsSafeAfterMove(head, result, fruit, tail, width, height)) {
			return result;
		}
	}
	// Khong tim duoc thi tim huong khac
	return FindSafeDirection(head, fruit, tail, currentDirection, width, height);
}