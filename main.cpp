#include"MyGame.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	//ゲーム
	Framework* game = new MyGame();
	
	//ゲーム実行
	game->Run();

	//解放
	delete game;

	return 0;
}


