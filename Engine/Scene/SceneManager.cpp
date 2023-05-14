#include "SceneManager.h"
#include <sstream>
#include <iomanip>
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "FbxLoader.h"

//---------------------------------------------------------------------------------------
//デストラクタ
SceneManager::~SceneManager()
{
	state->Finalize();
	delete state;
	for (int i = 0; i < _countof(model); i++)
	{
		delete model[i];
	}
	delete lightManager;
	camera.reset();
	//音データ解放

}

void SceneManager::ChangeScene(std::string sceneName)
{
	assert(sceneFactory_);

	if (this->state) {
		this->state->Finalize();
		delete this->state;
	}
	//シーンファクトリーでシーン生成
	this->state = sceneFactory_->CreateScene(sceneName);
	state->SetScene(this);
	this->state->Initialize();
}

void SceneManager::StopWaveAllScene()
{
	Sound::GetInstance().StopWave("Stage_BGM.wav");
}

void SceneManager::Initialize()
{
	{
		TextureManager::LoadGraph(L"Resources/image/loading.png", texhandle[3]);
		//白い画像
		TextureManager::LoadGraph(L"Resources/image/white.png", TextureManager::GetInstance().whiteTexHandle);

		//画像
		TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);

		TextureManager::LoadGraph(L"Resources/image/effect1.png", texhandle[1]);
		TextureManager::LoadGraph(L"Resources/image/a.png", texhandle[2]);
		TextureManager::LoadGraph(L"Resources/image/test.jpg", texhandle[4]);
	}

	//fbx読み込み
	modelFBX = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	{
		//Sound::GetInstance().LoadWave("Stage_BGM.wav", false);
	}

	model[0] = Model::LoadFromOBJ("skydome", true, true);
	model[1] = Model::LoadFromOBJ("ground");
	model[2] = Model::LoadFromOBJ("sphere", true);
	model[4] = Model::LoadFromOBJ("MiG-25PD", true);
	model[3] = Model::LoadFromOBJ("sphere");

	//Object::effectFlags.isGlassFilter = true;



	draw[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[0].worldMat->SetWorld();
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 0.0f, -10.0f, 0 };
	draw[1].worldMat->SetWorld();
	draw[2].worldMat->scale = { 10,10,10 };
	draw[2].worldMat->rot.y = { -pi };
	draw[2].worldMat->trans = { fighterPos[0],fighterPos[1],fighterPos[2] };
	draw[2].worldMat->SetWorld();
	draw[3].worldMat->scale = { 10,10,10 };
	draw[3].worldMat->rot.y = { -pi / 2.0f };
	draw[3].worldMat->trans = { 30.0f,0,0 };
	draw[3].worldMat->SetWorld();


	//球
	draw[4].worldMat->scale = { 5.0f,5.0f,5.0f };
	draw[4].worldMat->trans = { -20.0f,0,-10.0f };
	draw[4].worldMat->SetWorld();

	draw[5].worldMat->scale = { 1.3f,1.3f,1.3f };
	draw[5].worldMat->trans = { fighterPos[0],fighterPos[1] - 1.0f,fighterPos[2] };
	draw[5].worldMat->rot = { -3.14f / 2.0f,0,0 };
	draw[5].worldMat->SetWorld();


	//インスタンス生成
	lightManager = LightManager::Create();
	//ライト色を設定
	lightManager->SetDirLightColor(0, { 1,1,1 });
	//3Dオブジェクトにライトをセット(全体で一つを共有)
	Object::SetLight(lightManager);
	lightManager->SetDirLightActive(0, true);
	lightManager->SetDirLightActive(1, false);
	lightManager->SetDirLightActive(2, false);
	lightManager->SetDirLightDir(0, { 0, 0, 1.0 });
	//点光源
	for (int i = 0; i < 6; i++)
	{
		lightManager->SetPointLightActive(i, false);
	}
	//丸影
	lightManager->SetCircleShadowActive(0, false);

	//カメラ
	camera = std::make_unique<Camera>();
	camera->Initialize();


	//objPlayer->Initialize();

	//draw[4].SetCollider(new PlaneCollider);
	/*draw[4].SetIsValid(false);*/


}

void SceneManager::Update()
{
	lightManager->Update();

	state->Update();


	//#ifdef _DEBUG
		//if (KeyboardInput::GetInstance().KeyTrigger(DIK_E)) ChangeState(new SceneTitle);

	{
		cameraWorldMat.rot.y += (KeyboardInput::GetInstance().KeyPush(DIK_A) - KeyboardInput::GetInstance().KeyPush(DIK_D)) * 0.05f;

		cameraWorldMat.rot.x += (KeyboardInput::GetInstance().KeyPush(DIK_W) - KeyboardInput::GetInstance().KeyPush(DIK_S)) * 0.05f;
		cameraWorldMat.rot.x = min(cameraWorldMat.rot.x, pi / 2.0f);
		cameraWorldMat.rot.x = max(cameraWorldMat.rot.x, -pi / 2.0f);

		cameraWorldMat.SetWorld();
		camera->viewMat.eye = cameraPos;
		Vec3xM4(camera->viewMat.eye, cameraWorldMat.matWorld, 0);
		camera->UpdateViewMatrix();
	}
	//#endif

	lightManager->SetAmbientColor({ ambientColor[0],ambientColor[1], ambientColor[2] });
	lightManager->SetDiffuseColor({ diffuseColor[0],diffuseColor[1], diffuseColor[2] });
	lightManager->SetSpecularColor({ specularColor[0],specularColor[1], specularColor[2] });
}

void SceneManager::Draw()
{
	state->Draw();
}

void SceneManager::DrawPostEffect()
{
	state->DrawPostEffect();
}

void SceneManager::DrawPostEffect2()
{
	state->DrawPostEffect2();
}

void SceneManager::DrawSprite()
{
	state->DrawSprite();

	debugText.DrawAll(debugTextHandle);
}

void SceneManager::DrawImgui()
{
	ImGui::Begin("LightColor");

	ImGui::SliderFloat3("ambientColor", ambientColor, 0, 1.0f);
	ImGui::SliderFloat3("diffuseColor", diffuseColor, 0, 1.0f);
	ImGui::SliderFloat3("specularColor", specularColor, 0, 1.0f);

	ImGui::End();

	state->DrawImgui();
}


