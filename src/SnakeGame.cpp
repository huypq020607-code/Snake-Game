#include "../include/SnakeGame.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <fstream>
using namespace std;
//di chuyen con tro
void gotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
//an con tro
void hideCursor() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hOut, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(hOut, &cursorInfo);
}
//xoa toan bo ky tu tren console
void ClearScreen() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hOut, &csbi);
	DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
	DWORD count;
	COORD homeCoords = { 0, 0 };
	FillConsoleOutputCharacter(
		hOut,
		' ',
		cellCount,
		homeCoords,
		&count
	);
	FillConsoleOutputAttribute(
		hOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
	);
	SetConsoleCursorPosition(hOut, homeCoords);
}
SnakeGame::SnakeGame() {
	state = MENU;
	selectedOption = START_GAME;
	selectedGameOverOption = RESTART_GAME;
	difficulty = NORMAL;
	gameSpeed = 64;
	controlMode = HUMAN_MODE;
	selectedGameMode = HUMAN_MODE;
	dir = STOP;
	head.x = width / 2;
	head.y = height / 2;
	score = 0;
	highScore = 0;
	ateFruit = false;
	needFullDraw = true;
	previousHead = head;
	previousTail = { -1,-1 };
	SpawnFruit();
	LoadHighScore();
	DrawMenu();
}
void SnakeGame::DrawMenu() {
	gotoXY(0, 0);
	cout << "================================";
	gotoXY(0, 1);
	cout << "          SNAKE GAME            ";
	gotoXY(0, 2);
	cout << "================================";
	gotoXY(0, 4);
	cout << "           MAIN MENU            ";
	// START GAME
	gotoXY(0, 6);
	if (selectedOption == START_GAME) {
		cout << "        > START GAME";
	}
	else {
		cout << "          START GAME";
	}
	// GAME MODE
	gotoXY(0, 7);
	if (selectedOption == GAME_MODE) {
		cout << "        > GAME MODE";
	}
	else {
		cout << "          GAME MODE";
	}
	// DIFFICULTY
	gotoXY(0, 8);
	if (selectedOption == DIFFICULTY) {
		cout << "        > DIFFICULTY";
	}
	else {
		cout << "          DIFFICULTY";
	}
	// INSTRUCTIONS
	gotoXY(0, 9);
	if (selectedOption == INSTRUCTIONS) {
		cout << "        > INSTRUCTIONS";
	}
	else {
		cout << "          INSTRUCTIONS";
	}
	// EXIT
	gotoXY(0, 10);
	if (selectedOption == EXIT_MENU) {
		cout << "        > EXIT";
	}
	else {
		cout << "          EXIT";
	}
	gotoXY(0, 12);
	cout << "================================";
	gotoXY(0, 13);
	cout << "        W / S : Move            ";
	gotoXY(0, 14);
	cout << "        ENTER : Select          ";
	gotoXY(0, 15);
	cout << "================================";
}
void SnakeGame::DrawGameModeMenu() {
	gotoXY(0, 0);
	cout << "===============================================";
	gotoXY(0, 1);
	cout << "                 GAME MODE                    ";
	gotoXY(0, 2);
	cout << "===============================================";
	gotoXY(0, 5);
	cout << "                 ";
	cout << (selectedGameMode == HUMAN_MODE ? "> " : "  ");
	cout << "HUMAN                    ";
	gotoXY(0, 6);
	cout << "                 ";
	cout << (selectedGameMode == AI_MODE ? "> " : "  ");
	cout << "AI                    ";
	gotoXY(0, 8);
	cout << "===============================================";
	gotoXY(0, 9);
	cout << "                 W/S : Select";
	gotoXY(0, 10);
	cout << "                 ENTER : Confirm";
	gotoXY(0, 11);
	cout << "                 X : Back" << endl;
	gotoXY(0, 12);
	cout << "===============================================";
}
void SnakeGame::DrawGameOverMenu() {
	gotoXY(0, 0);
	cout << "================================";
	gotoXY(0, 1);
	cout << "           GAME OVER            ";
	gotoXY(0, 2);
	cout << "================================";
	gotoXY(0, 4);
	cout << "         WHAT NEXT?             ";
	// RESTART GAME
	gotoXY(0, 6);
	if (selectedGameOverOption == RESTART_GAME) {
		cout << "        > RESTART GAME";
	}
	else {
		cout << "          RESTART GAME";
	}
	// MAIN MENU
	gotoXY(0, 7);
	if (selectedGameOverOption == MAIN_MENU) {
		cout << "        > MAIN MENU";
	}
	else {
		cout << "          MAIN MENU";
	}
	// EXIT GAME
	gotoXY(0, 8);
	if (selectedGameOverOption == EXIT_GAME) {
		cout << "        > EXIT GAME";
	}
	else {
		cout << "          EXIT GAME";
	}
	gotoXY(0, 10);
	cout << "================================";
	gotoXY(0, 11);
	cout << "        W / S : Move            ";
	gotoXY(0, 12);
	cout << "        ENTER : Select          ";
	gotoXY(0, 13);
	cout << "================================";
}
void SnakeGame::DrawGameOverScreen() {
	gotoXY(0, 0);
	cout << "================================";
	gotoXY(0, 1);
	cout << "           GAME OVER            ";
	gotoXY(0, 2);
	cout << "================================";
	gotoXY(0, 5);
	cout << "          FINAL SCORE           ";
	gotoXY(0, 7);
	cout << "          Score: " << score;
	gotoXY(0, 8);
	cout << "          High Score: " << highScore;
	gotoXY(0, 10);
	cout << "================================";
	gotoXY(0, 12);
	cout << "      Press ENTER to continue   ";
	gotoXY(0, 14);
	cout << "================================";
}
void SnakeGame::DrawDifficultyMenu() {
	gotoXY(0, 0);
	cout << "================================";
	gotoXY(0, 1);
	cout << "          SNAKE GAME            ";
	gotoXY(0, 2);
	cout << "================================";
	gotoXY(0, 4);
	cout << "          DIFFICULTY            ";
	// EASY
	gotoXY(0, 6);
	if (difficulty == EASY) {
		cout << "        > EASY";
	}
	else {
		cout << "          EASY";
	}
	// NORMAL
	gotoXY(0, 7);
	if (difficulty == NORMAL) {
		cout << "        > NORMAL";
	}
	else {
		cout << "          NORMAL";
	}
	// HARD
	gotoXY(0, 8);
	if (difficulty == HARD) {
		cout << "        > HARD";
	}
	else {
		cout << "          HARD";
	}
	gotoXY(0, 10);
	cout << "================================";
	gotoXY(0, 11);
	cout << "        W / S : Move            ";
	gotoXY(0, 12);
	cout << "        ENTER : Select          ";
	gotoXY(0, 13);
	cout << "        X     : Back            ";
	gotoXY(0, 14);
	cout << "================================";
}
void SnakeGame::DrawInstructionsMenu() {
	gotoXY(0, 0);
	cout << "================================";
	gotoXY(0, 1);
	cout << "          SNAKE GAME            ";
	gotoXY(0, 2);
	cout << "================================";
	gotoXY(0, 4);
	cout << "         INSTRUCTIONS           ";
	gotoXY(0, 6);
	cout << "         HOW TO PLAY            ";
	gotoXY(0, 8);
	cout << "        W : Move Up             ";
	gotoXY(0, 9);
	cout << "        S : Move Down           ";
	gotoXY(0, 10);
	cout << "        A : Move Left           ";
	gotoXY(0, 11);
	cout << "        D : Move Right          ";
	gotoXY(0, 13);
	cout << "        P : Pause               ";
	gotoXY(0, 14);
	cout << "        X : Exit                ";
	gotoXY(0, 16);
	cout << "        * : +10 Score           ";
	gotoXY(0, 17);
	cout << "        Wall : Game Over        ";
	gotoXY(0, 18);
	cout << "        Body : Game Over        ";
	gotoXY(0, 20);
	cout << "================================";
	gotoXY(0, 21);
	cout << "        X : Back                ";
	gotoXY(0, 22);
	cout << "================================";
}
void SnakeGame::DrawFullBoard() {
	ClearScreen();
	// ===== BOARD =====
	gotoXY(0, 0);
	// Top border
	for (int i = 0; i < width + 2; i++) {
		cout << "#";
	}
	cout << endl;
	// Board content
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// Left border
			if (j == 0) {
				cout << "#";
			}
			// Snake head
			if (i == head.y && j == head.x) {
				cout << "O";
			}
			// Fruit
			else if (i == fruit.y && j == fruit.x) {
				cout << "*";
			}
			else {
				bool printTail = false;

				for (const auto& t : tail) {
					if (t.x == j && t.y == i) {
						cout << "o";
						printTail = true;
						break;
					}
				}

				if (!printTail) {
					cout << " ";
				}
			}
			// Right border
			if (j == width - 1) {
				cout << "#";
			}
		}

		cout << endl;
	}
	// Bottom border
	for (int i = 0; i < width + 2; i++) {
		cout << "#";
	}
	cout << endl;
	// ===== HUD =====
	gotoXY(0, height + 3);
	cout << "Score: " << score
		<< "    High Score: " << highScore
		<< "        ";

	gotoXY(0, height + 4);

	if (state == PLAYING) {
		cout << "WASD: Move | P: Pause | X: Exit        ";
	}
	else if (state == PAUSED) {
		cout << "          [ GAME PAUSED ]              ";
		gotoXY(0, height + 5);
		cout << "          P: Resume | X: Exit           ";
	}
	needFullDraw = false;
}
void SnakeGame::DrawUpdatedBoard() {
	// Xóa vị trí đầu cũ
	gotoXY(previousHead.x + 1, previousHead.y + 1);
	cout << " ";
	// Xóa đuôi cũ
	if (!ateFruit && previousTail.x >= 0 && previousTail.y >= 0) {
		gotoXY(previousTail.x + 1, previousTail.y + 1);
		cout << " ";
	}
	// Vẽ thân mới tại vị trí đầu cũ
	if (!tail.empty()) {
		gotoXY(previousHead.x + 1, previousHead.y + 1);
		cout << "o";
	}
	// Vẽ đầu mới
	gotoXY(head.x + 1, head.y + 1);
	cout << "O";
	// Nếu ăn trái cây thì vẽ trái cây mới
	if (ateFruit) {
		gotoXY(fruit.x + 1, fruit.y + 1);
		cout << "*";
	}
	// ===== HUD =====
	gotoXY(0, height + 3);
	cout << "Score: " << score
		<< "    High Score: " << highScore
		<< "        ";
	gotoXY(0, height + 4);
	if (state == PLAYING) {
		cout << "WASD: Move | P: Pause | X: Exit        ";
	}
	else if (state == PAUSED) {
		cout << "GAME PAUSED | P: Resume | X: Exit     ";
	}
}
void SnakeGame::Draw() {
	if (state == MENU) {
		DrawMenu();
		return;
	}
	if (state == GAME_MODE_MENU) {
		DrawGameModeMenu();
		return;
	}
	if (state == DIFFICULTY_MENU) {
		DrawDifficultyMenu();
		return;
	}
	if (state == INSTRUCTIONS_MENU) {
		DrawInstructionsMenu();
		return;
	}
	if (state == GAME_OVER_SCREEN) {
		DrawGameOverScreen();
		return;
	}
	if (state == GAME_OVER) {
		DrawGameOverMenu();
		return;
	}
	if (needFullDraw) {
		DrawFullBoard();
	}
	else {
		DrawUpdatedBoard();
	}
}
void SnakeGame::MenuInput() {
	if (!_kbhit()) {
		return;
	}
	int key = _getch();
	//di chuyen len
	if (key == 'w' || key == 'W') {
		selectedOption--;
		if (selectedOption < START_GAME) {
			selectedOption = EXIT_MENU;
		}
		DrawMenu();
	}
	//di chuyen xuong
	else if (key == 's' || key == 'S') {
		selectedOption++;
		if (selectedOption > EXIT_MENU) {
			selectedOption = START_GAME;
		}
		DrawMenu();
	}
	//chon
	//trong window console 13=enter
	else if (key == 13) { //enter
		switch (selectedOption) {
		case START_GAME:
			ResetGame();
			Draw();
			break;
		case GAME_MODE:
			state = GAME_MODE_MENU;
			ClearScreen();
			DrawGameModeMenu();
			break;
		case DIFFICULTY:
			state = DIFFICULTY_MENU;
			ClearScreen();
			DrawDifficultyMenu();
			break;
		case INSTRUCTIONS:
			state = INSTRUCTIONS_MENU;
			ClearScreen();
			DrawInstructionsMenu();
			break;
		case EXIT_MENU:
			state = EXITED;
			break;
		}
	}
}
void SnakeGame::GameModeInput() {
	if (!_kbhit()) return;
	int key = _getch();
	if (key == 'w' || key == 'W') {
		int current = static_cast<int>(selectedGameMode);
		current--;
		if (current < HUMAN_MODE) {
			current = AI_MODE;
		}
		selectedGameMode = static_cast<ControlMode>(current);
		DrawGameModeMenu();
	}
	else if (key == 's' || key == 'S') {
		int current = static_cast<int>(selectedGameMode);
		current++;
		if (current > AI_MODE) {
			current = HUMAN_MODE;
		}
		selectedGameMode = static_cast<ControlMode>(current);
		DrawGameModeMenu();
	}
	else if (key == 13) {
		controlMode = static_cast<ControlMode>(selectedGameMode);
		state = MENU;
		selectedOption = GAME_MODE;
		ClearScreen();
		DrawMenu();
	}
	else if (key == 'x' || key == 'X') {
		state = MENU;
		selectedOption = GAME_MODE;
		ClearScreen();
		DrawMenu();
	}
}
void SnakeGame::DifficultyInput() {
	if (!_kbhit()) {
		return;
	}
	int key = _getch();
	if (key == 'w' || key == 'W') {
		int current = static_cast<int>(difficulty);
		current--;
		if (current < EASY) {
			current = HARD;
		}
		difficulty = static_cast<Difficulty>(current);
		DrawDifficultyMenu();
	}
	else if (key == 's' || key == 'S') {
		int current = static_cast<int>(difficulty);
		current++;
		if (current > HARD) {
			current = EASY;
		}
		difficulty = static_cast<Difficulty>(current);
		DrawDifficultyMenu();
	}
	else if (key == 13) {
		if (difficulty == EASY) {
			gameSpeed = 124;
		}
		else if (difficulty == NORMAL) {
			gameSpeed = 64;
		}
		else if (difficulty == HARD) {
			gameSpeed = 24;
		}
		state = MENU;
		selectedOption = START_GAME;
		ClearScreen();
		DrawMenu();
	}
	else if (key == 'x' || key == 'X') {
		state = MENU;
		selectedOption = DIFFICULTY;
		ClearScreen();
		DrawMenu();
	}
}
void SnakeGame::GameOverScreenInput() {
	if (!_kbhit()) {
		return;
	}
	int key = _getch();
	if (key == 13) { // ENTER
		state = GAME_OVER;
		ClearScreen();
		DrawGameOverMenu();
	}
}
void SnakeGame::GameOverInput() {
	if (!_kbhit()) {
		return;
	}
	int key = _getch();
	if (key == 'w' || key == 'W') {
		selectedGameOverOption--;
		if (selectedGameOverOption < RESTART_GAME) {
			selectedGameOverOption = EXIT_GAME;
		}
		DrawGameOverMenu();
	}
	else if (key == 's' || key == 'S') {
		selectedGameOverOption++;
		if (selectedGameOverOption > EXIT_GAME) {
			selectedGameOverOption = RESTART_GAME;
		}
		DrawGameOverMenu();
	}
	else if (key == 13) {
		switch (selectedGameOverOption) {
		case RESTART_GAME:
			ResetGame();
			Draw();
			break;
		case MAIN_MENU:
			ClearScreen();
			state = MENU;
			selectedOption = START_GAME;
			DrawMenu();
			break;
		case EXIT_GAME:
			state = EXITED;
			break;
		}
	}
}
void SnakeGame::InstructionsInput() {
	if (!_kbhit()) {
		return;
	}
	int key = _getch();
	if (key == 'x' || key == 'X') {
		state = MENU;
		selectedOption = INSTRUCTIONS;
		ClearScreen();
		DrawMenu();
	}
}
void SnakeGame::AIInput() {
	if (state != PLAYING) {
		return;
	}
	Direction newDirection = ai.GetNextDirection(head, fruit, tail, dir, width, height);
	// AI khong con nuoc di
	if (newDirection == STOP) {
		state = GAME_OVER_SCREEN;
		needFullDraw = true;
		return;
	}
	dir = newDirection;
}
void SnakeGame::Input() {
	if (_kbhit()) { //kiem tra neu co phim bam vao
		switch (_getch()) {
		case 'p':
		case 'P':
			if (state == PLAYING) {
				state = PAUSED;
				needFullDraw = true;
				ClearScreen();
				Draw();
			}
			else if (state == PAUSED) {
				state = PLAYING;
				needFullDraw = true;
				Draw();
			}
			break;
		case 'a':
		case 'A':
			if (state == PLAYING && dir != RIGHT)
				dir = LEFT;
			break;
		case 'd':
		case 'D':
			if (state == PLAYING && dir != LEFT) {
				dir = RIGHT;
			}
			break;
		case 'w':
		case 'W':
			if (state == PLAYING && dir != DOWN) {
				dir = UP;
			}
			break;
		case 's':
		case 'S':
			if (state == PLAYING && dir != UP) {
				dir = DOWN;
			}
			break;
		case 'x':
		case 'X':
			state = EXITED;
			break;
		}
	}
}
void SnakeGame::SpawnFruit() {
	do {
		fruit.x = rand() % width;
		fruit.y = rand() % height;
	} while (IsOnSnake(fruit));
}
bool SnakeGame::IsOnSnake(Point p) const {
	if (p.x == head.x && p.y == head.y) {
		return true;
	}
	for (const auto& t : tail) {
		if (p.x == t.x && p.y == t.y) {
			return true;
		}
	}
	return false;
}
void SnakeGame::Move() {
	if (dir == STOP) {
		return;
	}
	//vi tri dau cu
	previousHead = head;
	//Mac dinh frame nay khong an
	ateFruit = false;
	//vi tri duoi cu
	if (!tail.empty()) {
		previousTail = tail.back();
	}
	else {
		previousTail = { -1, -1 };
	}
	//di chuyen
	switch (dir) {
	case LEFT:
		head.x--;
		break;
	case RIGHT:
		head.x++;
		break;
	case UP:
		head.y--;
		break;
	case DOWN:
		head.y++;
		break;
	default:
		break;
	}
	if (head.x == fruit.x && head.y == fruit.y) {
		ateFruit = true;
		score += 10;
		if (highScore < score) {
			highScore = score;
			SaveHighScore();
		}
	}
	//di chuyen than
	if (!tail.empty()) {
		for (int i = static_cast<int>(tail.size()) - 1;i > 0;i--) {
			tail[i] = tail[i - 1];
		}
		tail[0] = previousHead;
	}
	//neu an thuc an
	if (ateFruit) {
		//them duoi moi
		if (tail.empty()) {
			tail.push_back(previousHead);
		}
		else {
			tail.push_back(previousTail);
		}
		SpawnFruit();
	}
}
// va cham
void SnakeGame::CheckCollision() {
	//va tuong
	if (head.x < 0 || head.x >= width || head.y < 0 || head.y >= height) {
		state = GAME_OVER_SCREEN;
		needFullDraw = true;
		ClearScreen();
		DrawGameOverScreen();
		return;
	}
	//va than
	for (const auto& t : tail) {
		if (head.x == t.x && head.y == t.y) {
			state = GAME_OVER_SCREEN;
			needFullDraw = true;
			ClearScreen();
			DrawGameOverScreen();
			return;
		}
	}
}
void SnakeGame::ResetGame() {
	tail.clear();
	state = PLAYING;
	dir = STOP;
	head.x = width / 2;
	head.y = height / 2;
	score = 0;
	selectedGameOverOption = RESTART_GAME;
	ateFruit = false;
	previousHead = head;
	previousTail = { -1,-1 };
	SpawnFruit();
	needFullDraw = true;
}
void SnakeGame::Logic() {
	if (state == PLAYING) {
		Move();
		CheckCollision();
	}
	//neu va cham thi dung
	if (state != PLAYING) {
		return;
	}
}
void SnakeGame::LoadHighScore() {
	ifstream file("data/highscore.txt");
	if (file.is_open()) {
		file >> highScore;
		file.close();
	}
	else {
		highScore = 0;
	}
}
void SnakeGame::SaveHighScore() {
	ofstream file("data/highscore.txt");
	if (file.is_open()) {
		file << highScore;
		file.close();
	}
}
void SnakeGame::ControlSnake() {
	if (controlMode == HUMAN_MODE) {
		Input();
	}
	else if (controlMode == AI_MODE) {
		AIInput();
	}
}
bool SnakeGame::IsGameOver() const {
	return state == GAME_OVER;
}
bool SnakeGame::IsPlaying() const {
	return state == PLAYING;
}
bool SnakeGame::IsExited() const {
	return state == EXITED;
}
bool SnakeGame::IsInMenu() const {
	return state == MENU ||
		state == GAME_MODE_MENU ||
		state == DIFFICULTY_MENU ||
		state == INSTRUCTIONS_MENU;
}
void SnakeGame::RunMenu() {
	if (state == MENU) {
		MenuInput();
	}
	else if (state == GAME_MODE_MENU) {
		GameModeInput();
	}
	else if (state == DIFFICULTY_MENU) {
		DifficultyInput();
	}
	else if (state == INSTRUCTIONS_MENU) {
		InstructionsInput();
	}
}
bool SnakeGame::IsPaused() const {
	return state == PAUSED;
}
void SnakeGame::RunGameOverMenu() {
	GameOverInput();
}
bool SnakeGame::IsInDifficultyMenu() const {
	return state == DIFFICULTY_MENU;
}
int SnakeGame::GetGameSpeed() const {
	return gameSpeed;
}
bool SnakeGame::IsGameOverScreen() const {
	return state == GAME_OVER_SCREEN;
}
void SnakeGame::RunGameOverScreen() {
    if (needFullDraw) {
        ClearScreen();
        DrawGameOverScreen();
        needFullDraw = false;
    }
    GameOverScreenInput();
}