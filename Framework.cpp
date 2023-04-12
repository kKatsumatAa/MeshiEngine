#include "Framework.h"

void Framework::Initialize()
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

void Framework::Finalize()
{
	delete scene;

	//FBX
	FbxLoader::GetInstance()->Finalize();


	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();
}

bool Framework::Update()
{
	if (WindowsApp::GetInstance().MessegeRoop(msg))
	{
		return true;
	}

	//キーボード情報の取得開始
	KeyboardInput::GetInstance().Update();
	PadInput::GetInstance().Update();

	//更新処理
	scene->Update();

    return false;
}

void Framework::Run()
{
    //ゲームの初期化
    Initialize();

	//ゲームループ
	while (true) {

		//ゲームの更新
		if (Update())
		{
			break;
		}

		//ゲームの描画
		Draw();
	}

	//ゲーム終了
	Finalize();
}
