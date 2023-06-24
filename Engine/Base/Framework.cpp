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
	postPera_[0] = std::make_unique<PostPera>();
	postPera_[1] = std::make_unique<PostPera>();
	postPera_[0]->Initialize(L"Resources/image/normalImage.jpg");
	postPera_[1]->Initialize(L"Resources/image/normalImage.jpg");

	//キーボード入力初期化
	KeyboardInput::GetInstance();

	ParticleManager::GetInstance()->Initialize();

	//Light
	LightManager::StaticInitialize();

	//model
	Model::StaticInitialize();

	//シーン
	sceneM_ = &SceneManager::GetInstance();
	sceneM_->Initialize();

	imguiM_ = std::make_unique<ImGuiManager>();
	imguiM_->Initialize();
}

void Framework::Finalize()
{
	//モデル全てを解放
	ModelManager::GetInstance().Finalize();

	//FBX
	FbxLoader::GetInstance()->Finalize();

	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();

	imguiM_->Finalize();

	MouseInput::GetInstance().Finalize();
	PadInput::GetInstance().Finalize();
}

bool Framework::Update()
{
	if (WindowsApp::GetInstance().MessegeRoop(msg_))
	{
		return true;
	}

	//キーボード情報の取得開始
	KeyboardInput::GetInstance().Update();
	PadInput::GetInstance().Update();
	MouseInput::GetInstance().Update();

	//カメラマネージャー
	CameraManager::GetInstance().Update();

	//更新処理
	sceneM_->Update(postPera_->get());

	//imgui
	imguiM_->Begin();

	//ポストエフェクト(imgui)
	postPera_[0]->Update();

	//(imgui)
	Object::StaticUpdate();
	//(imgui)
	sceneM_->DrawImgui();

	imguiM_->End();

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
