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
	postPera_[0]->Initialize(L"Resources/image/normalImage.jpg", 0.952f);
	postPera_[1]->Initialize(L"Resources/image/normalImage.jpg");

	//キーボード入力初期化
	KeyboardInput::GetInstance();
	//trueにすることでマウスがほかのアプリで使用されない
	MouseInput::GetInstance().CreateDevice(/*true*/);

	//
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

void Framework::Update()
{
	//終了
	if (WindowsApp::GetInstance().MessegeRoop(msg_) || KeyboardInput::GetInstance().KeyPush(DIK_ESCAPE))
	{
		isEndGame_ = true;
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
	postPera_[1]->Update();

	//(imgui)
	Object::StaticUpdate();
	//(imgui)
	sceneM_->DrawImgui();

	imguiM_->End();
}

void Framework::Run()
{
	//ゲームの初期化
	Initialize();

	//ゲームループ
	while (!isEndGame_) {

		//ゲームの更新
		Update();

		//ゲームの描画
		Draw();
	}

	//ゲーム終了
	Finalize();
}
