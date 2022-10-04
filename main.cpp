#include"Scene.h"
#include"DebugText.h"



//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");
	
	//初期化
	WindowsApp::GetInstance();
	Directx::GetInstance();

	DrawInitialize();

	MSG msg{};	//メッセージ

	//デバッグテキスト
	UINT64 debugTextHandle;
	LoadGraph(L"Resources/ascii.png", debugTextHandle);

	DebugText debugText;

	//画像用ハンドル
	UINT64 textureHandle[30] = {0};

	//数字の画像
	UINT64 textureNumHundle[12];

	//音データ
	SoundData soundData[10];

	//キーボード入力初期化
	KeyboardInput::GetInstance();

	//シーン
	Scene scene;
	scene.Initialize(soundData);


	float num = 114.514f;

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

		Directx::GetInstance().DrawUpdate({ 0.0f,0.0f,0.0f,0.0f });

//更新処理
		scene.Update(soundData);
		num+=0.001f;

// 4.描画コマンドここから　//-----------
		scene.Draw(textureHandle, textureNumHundle);

		debugText.Print("num:", 10, 10, debugTextHandle,num);

// 4.描画コマンドここまで //

		Directx::GetInstance().DrawUpdate2();

		//毎フレーム処理　ここまで//

		if (KeyboardInput::GetInstance().keyPush(DIK_ESCAPE)) break;
	}

	//音データ解放
	Directx::GetInstance().xAudio2.Reset();
	//SoundUnLoad(&soundData[0]);
	

	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();

	return 0;
}


