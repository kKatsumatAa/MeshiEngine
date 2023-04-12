#include "MyGame.h"

void MyGame::Initialize()
{
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");

	//FBX
	FbxLoader::GetInstance()->Initialize();

	//初期化
	WindowsApp::GetInstance();
	DirectXWrapper::GetInstance().Initialize();

	DrawInitialize();	

	//キーボード入力初期化
	KeyboardInput::GetInstance();

	//シーン
	scene = new Scene();
	scene->Initialize();
}

void MyGame::Finalize()
{
	delete scene;

	//FBX
	FbxLoader::GetInstance()->Finalize();


	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();
}

bool MyGame::Update()
{
	if (WindowsApp::GetInstance().MessegeRoop(msg))
	{
		return true;
	}

	//毎フレーム処理　ここから//
	//キーボード情報の取得開始
	KeyboardInput::GetInstance().Update();
	PadInput::GetInstance().Update();

	//更新処理
	scene->Update();


	return false;
}

void MyGame::Draw()
{
	{
		DirectXWrapper::GetInstance().DrawUpdate();

		// 4.描画コマンドここから　//-----------
		scene->Draw();

		//
		DirectXWrapper::GetInstance().DrawUpdate2();
	}

	//マルチパス
	{
		//2枚目--------------------
		DirectXWrapper::GetInstance().PreDrawToPera();

		//
		scene->DrawPostEffect();

		DirectXWrapper::GetInstance().PostDrawToPera();

	}
	{
		//実際に描画----------------
		DirectXWrapper::GetInstance().PreDrawToPera2();

		//
		scene->DrawPostEffect2();

		scene->DrawSprite();

		// 4.描画コマンドここまで //
		DirectXWrapper::GetInstance().PostDrawToPera2();
	}
}
