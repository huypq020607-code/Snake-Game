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
		}
		else if (game.IsPaused()) {
			game.Input();
		}
		else if (game.IsGameOver()) {
			game.Input();
		}
		Sleep(60); //dieu chinh toc do game (ms)
	}
	gotoXY(0, 23);
	cout << "===============================================" << endl;
	cout << "----    Thank you for your playing !!!!    ----" << endl;
	cout << "===============================================" << endl;
	system("pause"); // Dung man hinh de ban xem diem so truoc khi dong
	return 0;
}