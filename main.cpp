#include "KeyboardInput.h"
#include "WindowsApp.h"
#include "DirectX.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
#include "WorldMat.h"
#include "Draw.h"
#include "Player.h"
#include "Enemy.h"
#include <random>


//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");
	
	//
	WindowsApp::GetInstance();
	Directx::GetInstance();

	DrawInitialize();
	
	WorldMat cameraWorldMat;

	MSG msg{};	//メッセージ
	

	ViewMat viewMat;
	ProjectionMat projectionMat;

	//画像用ハンドル
	UINT64 textureHandle[5] = {0};

	LoadGraph(L"Resources/white.png", textureHandle[0]);//ただの白（色変える用）
	LoadGraph(L"Resources/texture.jpg", textureHandle[1]);
	LoadGraph(L"Resources/texture2.jpg", textureHandle[2]);
	LoadGraph(L"Resources/back.png", textureHandle[3]);//back
	LoadGraph(L"Resources/scope.png", textureHandle[4]);//ロックオン
	


	//キーボード入力初期化
	KeyboardInput::GetInstance();

	//音データ
	/*SoundData soundData = SoundLoadWave("Resources/SE.wav",false);
	SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData, 10.0f, true);*/
	
	SoundData soundData2 = SoundLoadWave("Resources/test2.wav",false);
	SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData2, 5.0f, true);

	//player
	Player player;
	//enemy
	Enemy enemy(&player);

	//描画初期化処理-------------

   //初期化処理　ここまで//

   //ゲームループ
	while (true) {

		if (WindowsApp::GetInstance().MessegeRoop(msg))
		{
			break;
		}

		//毎フレーム処理　ここから//
		//キーボード情報の取得開始
		KeyboardInput::GetInstance().Update();

		Directx::GetInstance().DrawUpdate();

//更新処理
		player.Update();
		enemy.Update();

// 4.描画コマンドここから　//-----------
		enemy.Draw(viewMat, projectionMat, textureHandle);
		player.Draw(viewMat, projectionMat, textureHandle);//playerを後にしないと透過されない！

// 4.描画コマンドここまで //

		Directx::GetInstance().DrawUpdate2();

		//毎フレーム処理　ここまで//

		if (KeyboardInput::GetInstance().keyPush(DIK_ESCAPE)) break;
	}

	//音データ解放
	Directx::GetInstance().xAudio2.Reset();
	//SoundUnLoad(&soundData);
	SoundUnLoad(&soundData2);

	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();

	return 0;
}


