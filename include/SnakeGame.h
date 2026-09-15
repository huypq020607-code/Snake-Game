#ifndef SNAKEGAME_H
#define SNAKEGAME_H
#include <vector>
struct Point {
    int x;
    int y;
};
enum Direction {
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};
class SnakeGame {
private:
    bool gameOver;
    const int width = 20;
    const int height = 20;
    int score;
    Point head;
    Point fruit;
    std::vector<Point> tail;
    Direction dir;
    void SpawnFruit();
    bool IsOnSnake(Point p) const;
    void Move();
    void CheckCollision();
public:
    SnakeGame();
    void Draw();
    void Input();
    void Logic();
    bool IsGameOver() const;
};
void gotoXY(int x, int y);
void hideCursor();
#endif