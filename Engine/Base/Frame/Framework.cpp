#include "Framework.h"

void Framework::Initialize()
{
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");

	//初期化
	WindowsApp::GetInstance();
	DirectXWrapper::GetInstance().Initialize();

	//texture
	TextureManager::InitializeDescriptorHeap();

	//imgui
	imguiM_ = std::make_unique<ImGuiManager>();
	imguiM_->Initialize();

	//FBX
	FbxLoader::GetInstance()->Initialize();

	//音
	Sound::Initialize();

	//object系
	IObject::CommonInitialize();
	IObject3D::CommonInitialize();
	Object::CommonInitialize();
	ObjectFBX::CommonInitialize();
	Sprite::CommonInitialize();

	//ポストエフェクト
	PostEffectManager::GetInstance().Initialize("Resources/image/lava.jpg");

	//キーボード入力初期化
	KeyboardInput::GetInstance();

	//マウスを表示するか
	bool isDebug = true;
#ifdef _DEBUG
	isDebug = false;
#endif // _DEBUG

	//trueにすることでマウスがほかのアプリで使用されない
	MouseInput::GetInstance().SetIsExclusive(isDebug);

	//Light
	LightManager::StaticInitialize();

	//パーティクル
	ParticleManager::GetInstance()->Initialize();

	//model
	IModel::StaticInitialize();

	//objマネージャ
	ObjectManager::GetInstance().Initialize();

	//シーン
	sceneM_ = &SceneManager::GetInstance();
	sceneM_->Initialize();
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
	if (WindowsApp::GetInstance().MessegeRoop(msg_) || sceneM_->GetIsEscapingGame())
	{
		isEndGame_ = true;
	}

	//キーボード情報の取得開始
	KeyboardInput::GetInstance().Update();
	PadInput::GetInstance().Update();
	MouseInput::GetInstance().Update();

	//更新処理
	sceneM_->Update();

	//ポストエフェクト
	PostEffectManager::GetInstance().Update();
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
