#include"Scene.h"



//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");
	
	//
	WindowsApp::GetInstance();
	Directx::GetInstance();

	DrawInitialize();

	MSG msg{};	//メッセージ

	//画像用ハンドル
	UINT64 textureHandle[30] = {0};
	{
		LoadGraph(L"Resources/white.png", textureHandle[0]);//ただの白（色変える用）
		LoadGraph(L"Resources/a.png", textureHandle[1]);//s
		LoadGraph(L"Resources/b.png", textureHandle[2]);//t
		LoadGraph(L"Resources/c.png", textureHandle[3]);//a
		LoadGraph(L"Resources/lock.png", textureHandle[4]);//ロックオン
		LoadGraph(L"Resources/d.png", textureHandle[5]);//r
		LoadGraph(L"Resources/lockOn.png", textureHandle[6]);//敵のロックオン
		LoadGraph(L"Resources/Title.png", textureHandle[7]);//タイトル
		LoadGraph(L"Resources/HP.png", textureHandle[8]);//hp
		LoadGraph(L"Resources/Clear.png", textureHandle[9]);//クリア
		LoadGraph(L"Resources/info.png", textureHandle[10]);//説明1
		LoadGraph(L"Resources/info2.png", textureHandle[11]);//説明2
		LoadGraph(L"Resources/info3.png", textureHandle[12]);//説明3
	}
	//数字の画像
	UINT64 textureNumHundle[12];
	{
		LoadGraph(L"Resources/numbers/0.png", textureNumHundle[0]);
		LoadGraph(L"Resources/numbers/1_.png", textureNumHundle[1]);
		LoadGraph(L"Resources/numbers/2.png", textureNumHundle[2]);
		LoadGraph(L"Resources/numbers/3.png", textureNumHundle[3]);
		LoadGraph(L"Resources/numbers/4.png", textureNumHundle[4]);
		LoadGraph(L"Resources/numbers/5.png", textureNumHundle[5]);
		LoadGraph(L"Resources/numbers/6.png", textureNumHundle[6]);
		LoadGraph(L"Resources/numbers/7.png", textureNumHundle[7]);
		LoadGraph(L"Resources/numbers/8.png", textureNumHundle[8]);
		LoadGraph(L"Resources/numbers/9.png", textureNumHundle[9]);
		LoadGraph(L"Resources/numbers/10.png", textureNumHundle[10]);
	}

	//キーボード入力初期化
	KeyboardInput::GetInstance();

	//音データ
	SoundData soundData[10];
	{
		soundData[0] = SoundLoadWave("Resources/SE.wav", true);
		soundData[1] = SoundLoadWave("Resources/SE2.wav", true);
		soundData[2] = SoundLoadWave("Resources/BGM2.wav", false);
		soundData[3] = SoundLoadWave("Resources/Death.wav", false);
		soundData[4] = SoundLoadWave("Resources/Damage.wav", false);
		soundData[5] = SoundLoadWave("Resources/SE3.wav", false);
		soundData[6] = SoundLoadWave("Resources/BGM.wav", false);
		soundData[7] = SoundLoadWave("Resources/heal.wav", false);
	}

	Scene scene;
	scene.Initialize(soundData);

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
		

// 4.描画コマンドここから　//-----------
		scene.Draw(textureHandle, textureNumHundle);


// 4.描画コマンドここまで //

		Directx::GetInstance().DrawUpdate2();

		//毎フレーム処理　ここまで//

		if (KeyboardInput::GetInstance().keyPush(DIK_ESCAPE)) break;
	}

	//音データ解放
	Directx::GetInstance().xAudio2.Reset();
	SoundUnLoad(&soundData[0]);
	SoundUnLoad(&soundData[1]);
	SoundUnLoad(&soundData[2]);
	SoundUnLoad(&soundData[3]);
	

	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();

	return 0;
}


