#include"MyGame.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	//ゲーム
	std::unique_ptr<Framework> game = std::make_unique<MyGame>();
	
	//ゲーム実行
	game->Run();

	return 0;
}


