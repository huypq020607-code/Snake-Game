#ifndef SNAKEGAME_H
#define SNAKEGAME_H
#include <vector>
// ==================== DATA ====================
struct Point {
	int x;
	int y;
};
// ==================== ENUM ====================
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
// ==================== SNAKE GAME ====================
class SnakeGame {
private:
	// ---------- Game State ----------
	GameState state;
	// ---------- Board ----------
	const int width = 24;
	const int height = 24;
	// ---------- Score ----------
	int score;
	int highScore;
	// ---------- Snake ----------
	Point head;
	Point fruit;
	std::vector<Point> tail;
	// ---------- Rendering ----------
	Point previousHead;
	Point previousTail;
	bool ateFruit;
	bool needFullDraw;
	// ---------- Movement ----------
	Direction dir;
	// ---------- Menu ----------
	int selectedOption;
	int selectedGameOverOption;
	// ---------- Difficulty ----------
	Difficulty difficulty;
	int gameSpeed;
	// ==================== GAME LOGIC ====================
	void SpawnFruit();
	bool IsOnSnake(Point p) const;
	void Move();
	void CheckCollision();
	void ResetGame();
	// ==================== INPUT ====================
	void MenuInput();
	void DifficultyInput();
	void InstructionsInput();
	void GameOverScreenInput();
	void GameOverInput();
	// ==================== DRAWING ====================
	void DrawMenu();
	void DrawDifficultyMenu();
	void DrawInstructionsMenu();
	void DrawGameOverScreen();
	void DrawGameOverMenu();
	void DrawFullBoard();
	void DrawUpdatedBoard();
	// ==================== HIGH SCORE ====================
	void LoadHighScore();
	void SaveHighScore();
public:
	// ==================== CONSTRUCTOR ====================
	SnakeGame();
	// ==================== GAME LOOP ====================
	void Draw();
	void Input();
	void Logic();
	// ==================== MENU ====================
	void RunMenu();
	// ==================== GAME OVER ====================
	bool IsGameOverScreen() const;
	void RunGameOverScreen();
	bool IsGameOver() const;
	void RunGameOverMenu();
	// ==================== STATE ====================
	bool IsPlaying() const;
	bool IsPaused() const;
	bool IsExited() const;
	bool IsInMenu() const;
	bool IsInDifficultyMenu() const;
	// ==================== OTHER ====================
	int GetGameSpeed() const;
};
// ==================== CONSOLE FUNCTIONS ====================
void gotoXY(int x, int y);
void hideCursor();
void ClearScreen();
#endif