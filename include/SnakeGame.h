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
	DIFFICULTY_MENU,
	INSTRUCTIONS_MENU,
	PLAYING,
	PAUSED,
	GAME_OVER_SCREEN,
	GAME_OVER,
	EXITED
};
enum MenuOption {
	START_GAME = 0,
	DIFFICULTY,
	INSTRUCTIONS,
	EXIT_MENU
};
enum GameOverOption {
	RESTART_GAME = 0,
	MAIN_MENU,
	EXIT_GAME
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
	Point previousHead;
	Point previousTail;
	std::vector<Point> tail;
	Direction dir;
	int selectedOption;
	int selectedGameOverOption;
	Difficulty difficulty;
	int gameSpeed;
	bool ateFruit;
	bool needFullDraw;
	void SpawnFruit();
	bool IsOnSnake(Point p) const;
	void Move();
	void CheckCollision();
	void DrawMenu();
	void MenuInput();
	void DrawDifficultyMenu();
	void DifficultyInput();
	void DrawInstructionsMenu();
	void InstructionsInput();
	void DrawGameOverScreen();
	void GameOverScreenInput();
	void DrawGameOverMenu();
	void GameOverInput();
public:
	SnakeGame();
	void Draw();
	void Input();
	void Logic();
	void ResetGame();
	void RunMenu();
	bool IsGameOverScreen() const;
	void RunGameOverScreen();
	void RunGameOverMenu();
	bool IsGameOver() const;
	bool IsPlaying() const;
	bool IsPaused() const;
	bool IsExited() const;
	bool IsInMenu() const;
	bool IsInDifficultyMenu() const;
	int GetGameSpeed() const;
};
void gotoXY(int x, int y);
void hideCursor();
void ClearScreen();
#endif