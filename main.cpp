#include"MyGame.h"


int32_t WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	//�Q�[��
	std::unique_ptr<Framework> game = std::make_unique<MyGame>();
	
	//�Q�[�����s
	game->Run();

	return 0;
}


