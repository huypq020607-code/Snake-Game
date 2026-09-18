#include "../include/SnakeGame.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
using namespace std;
//di chuyen con tro
void gotoXY(int x, int y){
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
//an con tro
void hideCursor(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}
SnakeGame :: SnakeGame(){
	state = PLAYING;
	dir = STOP;
	head.x = width / 2;
	head.y = height / 2;
	score=0;
	SpawnFruit();
}
void SnakeGame :: SpawnFruit() {
	do {
		fruit.x = rand() % width;
		fruit.y = rand() % height;
	} while (IsOnSnake(fruit));
}
bool SnakeGame :: IsOnSnake(Point p) const{
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
void SnakeGame :: Draw(){
	gotoXY(0,0);
	//tuong tren
	for (int i = 0;i < width + 2;i++) {
		cout << "#";
	}
	cout << endl;
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			if(j==0) cout << "#"; //tuong trai
			if(i==head.y && j==head.x) cout << "O"; //dau ran
			else if(i==fruit.y && j==fruit.x) cout << "*"; //thuc an
			else{
				bool printTail=false;
				for(const auto& t:tail){
					if(t.x==j && t.y==i){
						cout << "o";
						printTail=true;
						break;
					}
				}
				if(!printTail) cout << " ";
			}
			if(j==width-1) cout << "#"; //tuong phai
		}
		cout << endl;
	}
	for(int i=0;i<width+2;i++) cout << "#";
	cout << endl;
	cout << "Score : " << score << endl;
	if(state == PAUSED){
		cout << "PAUSED - Press P to continue" << endl;
	}
	else if(state == GAME_OVER){
		cout << "GAME_OVER - Press R to restart game" << endl;
		cout << "Press X to exit game" << endl;
	}
	else if (state == PLAYING){
		cout << "WASD: Move | P: Pause | X: Exit" << endl;
	}
}
void SnakeGame :: Input(){
	if(_kbhit()){ //kiem tra neu co phim bam vao
		switch(_getch()){
			case 'r':
			case 'R':
				  if(state == GAME_OVER){
					ResetGame();
				  }
				  break;
			case 'p':
			case 'P':
				  if(state == PLAYING){
					state = PAUSED;
				  }
				  else if(state == PAUSED){
					state = PLAYING;
				  }
				  break;
			case 'a':
			case 'A':
				  if(state == PLAYING && dir!=RIGHT) 
					dir=LEFT;
				  break;
			case 'd':
			case 'D':
				  if(state == PLAYING && dir!=LEFT){ 
					dir=RIGHT;
				  }
				  break;
			case 'w':
			case 'W':
				  if(state == PLAYING && dir!=DOWN){ 
					dir=UP;
				  }
				  break;
			case 's':
			case 'S':
				  if(state == PLAYING && dir!=UP){
					dir=DOWN;
				  }
				  break;
			case 'x':
			case 'X':
				  state = EXITED;
				  break;
		}
	}
}
void SnakeGame :: Move() {
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
void SnakeGame :: CheckCollision() {
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
void SnakeGame :: ResetGame(){
	tail.clear();
	state = PLAYING;
	dir = STOP;
	head.x = width/2;
	head.y = height/2;
	score = 0;
	SpawnFruit();
}
void SnakeGame :: Logic() {
	Move();
	CheckCollision();
}
bool SnakeGame :: IsGameOver() const {
	return state == GAME_OVER;
}
bool SnakeGame :: IsPlaying() const{
	return state == PLAYING;
}
bool SnakeGame :: IsExited() const{
	return state == EXITED;
}