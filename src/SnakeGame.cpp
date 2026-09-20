#include "../include/SnakeGame.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
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
	gameSpeed = 60;
	dir = STOP;
	head.x = width / 2;
	head.y = height / 2;
	score = 0;
	SpawnFruit();
	DrawMenu();
}
void SnakeGame::DrawMenu() {
	gotoXY(0, 0);
	cout << "==========================" << endl;
	cout << "          SNAKE GAME                " << endl;
	cout << "==========================" << endl;
	cout << endl;
	cout << "          MAIN MENU                 " << endl;
	cout << endl;
	// START GAME
	if (selectedOption == START_GAME) {
		cout << "   > START GAME" << endl;
	}
	else {
		cout << "     START GAME" << endl;
	}
	// DIFFICULTY
	if (selectedOption == DIFFICULTY) {
		cout << "   > DIFFICULTY" << endl;
	}
	else {
		cout << "     DIFFICULTY" << endl;
	}
	// INSTRUCTIONS
	if (selectedOption == INSTRUCTIONS) {
		cout << "   > INSTRUCTIONS" << endl;
	}
	else {
		cout << "     INSTRUCTIONS" << endl;
	}
	// EXIT
	if (selectedOption == EXIT_MENU) {
		cout << "   > EXIT" << endl;
	}
	else {
		cout << "     EXIT" << endl;
	}
	cout << endl;
	cout << "==========================" << endl;
	cout << "       W / S: Move" << endl;
	cout << "       ENTER: Select" << endl;
	cout << "==========================" << endl;
}
void SnakeGame::DrawGameOverMenu() {
	gotoXY(0, 0);
	cout << "==========================" << endl;
	cout << "         GAME OVER        " << endl;
	cout << "==========================" << endl;
	cout << endl;
	// RESTART GAME
	if (selectedGameOverOption == RESTART_GAME) {
		cout << "#   > RESTART GAME" << endl;
	}
	else {
		cout << "#     RESTART GAME" << endl;
	}
	// MAIN MENU
	if (selectedGameOverOption == MAIN_MENU) {
		cout << "#   > MAIN MENU" << endl;
	}
	else {
		cout << "#     MAIN MENU" << endl;
	}
	// EXIT GAME
	if (selectedGameOverOption == EXIT_GAME) {
		cout << "#   > EXIT GAME" << endl;
	}
	else {
		cout << "#     EXIT GAME" << endl;
	}
	cout << endl;
	cout << "==========================" << endl;
	cout << "#       W / S: Move" << endl;
	cout << "#       ENTER: Select" << endl;
	cout << "==========================" << endl;
}
void SnakeGame::DrawDifficultyMenu() {
	gotoXY(0, 0);
	cout << "==========================" << endl;
	cout << "        DIFFICULTY        " << endl;
	cout << "==========================" << endl;
	cout << endl;
	// EASY
	if (difficulty == EASY) {
		cout << "   > EASY" << endl;
	}
	else {
		cout << "     EASY" << endl;
	}
	// NORMAL
	if (difficulty == NORMAL) {
		cout << "   > NORMAL" << endl;
	}
	else {
		cout << "     NORMAL" << endl;
	}
	// HARD
	if (difficulty == HARD) {
		cout << "   > HARD" << endl;
	}
	else {
		cout << "     HARD" << endl;
	}
	cout << endl;
	cout << "==========================" << endl;
	cout << "       W / S: Move" << endl;
	cout << "       ENTER: Select" << endl;
	cout << "       X: Back" << endl;
	cout << "==========================" << endl;
}
void SnakeGame::Draw() {
	if (state == MENU) {
		DrawMenu();
		return;
	}
	if (state == GAME_OVER) {
		DrawGameOverMenu();
		return;
	}
	gotoXY(0, 0);
	//tuong tren
	for (int i = 0;i < width + 2;i++) {
		cout << "#";
	}
	cout << endl;
	for (int i = 0;i < height;i++) {
		for (int j = 0;j < width;j++) {
			if (j == 0) cout << "#"; //tuong trai
			if (i == head.y && j == head.x) cout << "O"; //dau ran
			else if (i == fruit.y && j == fruit.x) cout << "*"; //thuc an
			else {
				bool printTail = false;
				for (const auto& t : tail) {
					if (t.x == j && t.y == i) {
						cout << "o";
						printTail = true;
						break;
					}
				}
				if (!printTail) cout << " ";
			}
			if (j == width - 1) cout << "#"; //tuong phai
		}
		cout << endl;
	}
	for (int i = 0;i < width + 2;i++) cout << "#";
	cout << endl;
	// Hien thi Score
	cout << "Score : " << score << "                    " << endl;
	// Hien thi trang thai
	if (state == PLAYING) {
		cout << "WASD: Move | P: Pause | X: Exit        " << endl;
	}
	else if (state == PAUSED) {
		cout << "              PAUSED                  " << endl;
		cout << "        Press P to continue            " << endl;
		cout << "        Press X to exit                " << endl;

	}
	else if (state == GAME_OVER) {
		DrawGameOverMenu();
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
			state = PLAYING;
			Draw();
			break;
		case DIFFICULTY:
			state = DIFFICULTY_MENU;
			ClearScreen();
			DrawDifficultyMenu();
			break;
		case INSTRUCTIONS:
			//tam chua co
			break;
		case EXIT_MENU:
			state = EXITED;
			break;
		}
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
void SnakeGame::GameOverInput() {
	if (!_kbhit()) {
		return;
	}
	int key = _getch();
	if (key == 'w' || key == 'w') {
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
void SnakeGame::Input() {
	if (_kbhit()) { //kiem tra neu co phim bam vao
		switch (_getch()) {
		case 'p':
		case 'P':
			if (state == PLAYING) {
				state = PAUSED;
				Draw();
			}
			else if (state == PAUSED) {
				state = PLAYING;
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
	Point previousHead = head;
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
	//neu an thuc an
	if (head.x == fruit.x && head.y == fruit.y) {
		score += 10;
		//them than moi
		tail.push_back(previousHead);
		//tao thuc an moi
		SpawnFruit();
	}
	else {
		//di chuyen than
		if (!tail.empty()) {
			for (int i = static_cast<int>(tail.size()) - 1;i > 0;i--) {
				tail[i] = tail[i - 1];
			}
			tail[0] = previousHead;
		}
	}
}
// va cham
void SnakeGame::CheckCollision() {
	//va tuong
	if (head.x < 0 || head.x >= width || head.y < 0 || head.y >= height) {
		state = GAME_OVER;
		return;
	}
	//va than
	for (const auto& t : tail) {
		if (head.x == t.x && head.y == t.y) {
			state = GAME_OVER;
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
	SpawnFruit();
}
void SnakeGame::Logic() {
	Move();
	CheckCollision();
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
	return state == MENU || state == DIFFICULTY_MENU;
}
void SnakeGame::RunMenu() {
	if (state == MENU) {
		MenuInput();
	}
	else if (state == DIFFICULTY_MENU) {
		DifficultyInput();
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