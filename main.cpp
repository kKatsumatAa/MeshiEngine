#include"MyGame.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	//ÉQÅ[ÉÄ
	std::unique_ptr<Framework> game = std::make_unique<MyGame>();
	
	//ÉQÅ[ÉÄé¿çs
	game->Run();

	return 0;
}


