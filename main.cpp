#include"MyGame.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	//ƒQ[ƒ€
	Framework* game = new MyGame();
	
	//ƒQ[ƒ€ŽÀs
	game->Run();

	//‰ð•ú
	delete game;

	return 0;
}


