#include"MyGame.h"


//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	//ゲーム
	MyGame game;
	//初期化
	game.Initialize();


   //ゲームループ//
	while (true) {

		//ゲームの更新
		if (game.Update())
		{
			break;
		}

		//ゲームの描画
		game.Draw();
	}

	//ゲーム終了
	game.Finalize();

	return 0;
}


