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

	Sound::Initialize();

	DrawInitialize();

	//ポストエフェクト//
	postPera[0] = std::make_unique<PostPera>();
	postPera[1] = std::make_unique<PostPera>();
	postPera[0]->Initialize(L"Resources/image/normalImage.jpg");
	postPera[1]->Initialize(L"Resources/image/normalImage.jpg");

	//キーボード入力初期化
	KeyboardInput::GetInstance();

	ParticleManager::GetInstance()->Initialize();

	//Light
	LightManager::StaticInitialize();

	//model
	Model::StaticInitialize();

	//シーン
	sceneM = &SceneManager::GetInstance();
	sceneM->Initialize();

	imguiM = std::make_unique<ImGuiManager>();
	imguiM->Initialize();
}

void Framework::Finalize()
{
	//FBX
	FbxLoader::GetInstance()->Finalize();

	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();

	imguiM->Finalize();
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
	sceneM->Update();

	//imgui
	imguiM->Begin();

	//ポストエフェクト(imgui)
	postPera[0]->Update();

	//(imgui)
	Object::StaticUpdate();
	//(imgui)
	sceneM->DrawImgui();

	imguiM->End();

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
