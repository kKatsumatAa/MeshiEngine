#include"MyGame.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	//�Q�[��
	Framework* game = new MyGame();
	
	//�Q�[�����s
	game->Run();

	//���
	delete game;

	return 0;
}


