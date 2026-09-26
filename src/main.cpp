#include "../include/SnakeGame.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <windows.h>
using namespace std;
int main() {
	//tao seed random
	srand(static_cast<unsigned int>(time(0)));
	//an con tro
	hideCursor();
	SnakeGame game;
	while (!game.IsExited()) {
		if (game.IsInMenu()) {
			game.RunMenu();
		}
		else if (game.IsPlaying()) {
			game.Input();
			game.Logic();
			game.Draw();
			Sleep(game.GetGameSpeed()); //dieu chinh toc do game (ms)
		}
		else if (game.IsPaused()) {
			game.Input();
			Sleep(50);
		}
		else if (game.IsGameOverScreen()) {
			game.RunGameOverScreen();
			Sleep(50);
		}
		else if (game.IsGameOver()) {
			game.RunGameOverMenu ();
			Sleep(50);
		}
	}
	gotoXY(0, 23);
	cout << "===============================================" << endl;
	cout << "----    Thank you for your playing !!!!    ----" << endl;
	cout << "===============================================" << endl;
	system("pause"); // Dung man hinh de ban xem diem so truoc khi dong
	return 0;
}