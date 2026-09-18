#include "../include/SnakeGame.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <windows.h>
using namespace std;
int main(){
	//tao seed random
	srand(static_cast<unsigned int>(time(0)));
	//an con tro
	hideCursor();
	const int gameSpeed = 60;
	SnakeGame game;
	while(!game.IsExited()){
		game.Input();
		if(game.IsPlaying()){
			game.Logic();
		}
		game.Draw();
		Sleep(gameSpeed); //dieu chinh toc do game (ms)
	}
	gotoXY(0, 23);
	cout << "===============================================" << endl;
   	cout << "----    Thank you for your playing !!!!    ----" << endl;
    	cout << "===============================================" << endl;
    	system("pause"); // Dung man hinh de ban xem diem so truoc khi dong
	return 0;
}