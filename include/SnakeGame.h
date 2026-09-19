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
enum GameState {
	MENU,
	PLAYING,
	PAUSED,
	GAME_OVER,
	EXITED
};
enum MenuOption {
	START_GAME = 0,
	DIFFICULTY,
	INSTRUCTIONS,
	EXIT_MENU
};
enum Difficulty {
	EASY = 0,
	NORMAL,
	HARD
};
class SnakeGame {
private:
	GameState state;
	const int width = 24;
	const int height = 24;
	int score;
	Point head;
	Point fruit;
	std::vector<Point> tail;
	Direction dir;
	int selectedOption;
	Difficulty difficulty;
	int gameSpeed;
	void SpawnFruit();
	bool IsOnSnake(Point p) const;
	void Move();
	void CheckCollision();
	void DrawMenu();
	void MenuInput();
	void DrawDifficultyMenu();
	void DrawGameOverMenu();
public:
	SnakeGame();
	void Draw();
	void Input();
	void Logic();
	void ResetGame();
	void RunMenu();
	bool IsGameOver() const;
	bool IsPlaying() const;
	bool IsPaused() const;
	bool IsExited() const;
	bool IsInMenu() const;
};
void gotoXY(int x, int y);
void hideCursor();
#endif