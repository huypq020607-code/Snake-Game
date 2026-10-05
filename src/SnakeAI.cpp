#include "../include/SnakeAI.h"
#include <vector>
#include <queue>
// kiem tra point co nam tren tail khong
bool CheckTail(
	Point p,
	const std::vector<Point>& tail
){
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
){
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
){
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
){
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
			if (current.x == head.x &&current.y == head.y) {
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
			if (!CheckValid(p,tail,width,height)) {
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
// Ham AI chinh
Direction SnakeAI::GetNextDirection(
	Point head,
	Point fruit,
	const std::vector<Point>& tail,
	Direction currentDirection,
	int width,
	int height
){
	Direction result = FindPathBFS(head,fruit,tail,width,height);
	// BFS tim duoc duong
	if (result != STOP &&
		!IsOppositeDirection(currentDirection, result)) {
		return result;
	}
	// Khong tim duoc thi tim huong khac
	if (CheckSafeMove(head, UP, tail, width, height) &&
		!IsOppositeDirection(currentDirection, UP)) {
		return UP;
	}
	if (CheckSafeMove(head, DOWN, tail, width, height) &&
		!IsOppositeDirection(currentDirection, DOWN)) {
		return DOWN;
	}
	if (CheckSafeMove(head, LEFT, tail, width, height) &&
		!IsOppositeDirection(currentDirection, LEFT)) {
		return LEFT;
	}
	if (CheckSafeMove(head, RIGHT, tail, width, height) &&
		!IsOppositeDirection(currentDirection, RIGHT)) {
		return RIGHT;
	}
	return STOP;
}