#include "Scene.h"
#include <sstream>
#include <iomanip>

void SceneState::SetScene(Scene* scene)
{
	//stateではなくSceneクラスのインスタンス//
	this->scene = scene;
}



//---------------------------------------------------------------------------------------
//タイトル
void SceneTitle::Initialize()
{

}

void SceneTitle::Update(SoundData* soundData)
{
	/*if (1){scene->ChangeState(new SceneGame);}*/
}

void SceneTitle::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{

}



//---------------------------------------------------------------------------------------
//ゲーム
void SceneGame::Initialize()
{

}

void SceneGame::Update(SoundData* soundData)
{
	//シーン遷移
	//if (1) scene->ChangeState(new SceneEnd);
}

void SceneGame::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{

}



//---------------------------------------------------------------------------------------
//終了画面
void SceneEnd::Initialize()
{

}

void SceneEnd::Update(SoundData* soundData)
{
	//if (1) scene->ChangeState(new SceneTitle);
}

void SceneEnd::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{

}



//---------------------------------------------------------------------------------------
//常に行うもの
Scene::~Scene()
{
	delete state;
	for (int i = 0; i < _countof(model); i++)
	{
		delete model[i];
	}
	imGuiManager->Finalize();
	delete imGuiManager;
	delete light;
}

void Scene::ChangeState(SceneState* state)
{
	delete this->state;
	this->state = state;
	state->SetScene(this);
	state->Initialize();
}

void Scene::Initialize(SoundData* soundData)
{
	this->soundData = soundData;

	TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);
	TextureManager::LoadGraph(L"Resources/image/white.png", texhandle[0]);
	TextureManager::LoadGraph(L"Resources/image/particle.png", texhandle[1]);
	TextureManager::LoadGraph(L"Resources/image/p.jpg", texhandle[2]);

	model[0] = Model::LoadFromOBJ("skydome");
	draw[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	model[1] = Model::LoadFromOBJ("ground");
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 10.0f, -10.0f, 0 };
	model[2] = Model::LoadFromOBJ("chr_sword");
	draw[2].worldMat->scale = { 15,15,15 };
	draw[2].worldMat->trans = { 10.0f, -10.0f, 0 };
	model[3] = Model::LoadFromOBJ("sphere", true);
	draw[3].worldMat->scale = { 5,5,5 };
	model[4] = Model::LoadFromOBJ("sphere");
	draw[4].worldMat->scale = { 5,5,5 };
	draw[4].worldMat->trans = { -10.0f, 0.0f, 0 };

	draw[3].worldMat->trans.x += 30.0f;
	draw[6].worldMat->trans.x -= 30.0f;
	draw[7].worldMat->trans.z -= 20.0f;

	//imgui
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize();

	ChangeState(new SceneTitle);
	state->SetScene(this);

	//Light
	Light::StaticInitialize();
	//インスタンス生成
	light = Light::Create();
	//ライト色を設定
	light->SetLightColor({ 1,1,1 });
	//3Dオブジェクトにライトをセット(全体で一つを共有)
	Draw::SetLight(light);
}

void Scene::Update(SoundData* soundData)
{

	//imgui
	imGuiManager->Begin();

	{
		ImGui::SetNextWindowSize({ 500,100 });
		ImGui::SliderFloat2("position", pos, 0.0f, WindowsApp::GetInstance().window_height, "%4.1f");
		//デモ
		ImGui::ShowDemoWindow();

		{
			//ライト
			//光線方向初期化
			static XMVECTOR lightDir = { 0,1,5,0 };

			if (KeyboardInput::GetInstance().keyPush(DIK_UP)) { lightDir.m128_f32[1] += 1.0f; }
			if (KeyboardInput::GetInstance().keyPush(DIK_DOWN)) { lightDir.m128_f32[1] -= 1.0f; }
			if (KeyboardInput::GetInstance().keyPush(DIK_RIGHT)) { lightDir.m128_f32[0] += 1.0f; }
			if (KeyboardInput::GetInstance().keyPush(DIK_LEFT)) { lightDir.m128_f32[0] -= 1.0f; }

			light->SetLightDir(lightDir);

			std::ostringstream debugstr;
			debugstr << "lightDirFactor("
				<< std::fixed << std::setprecision(2)
				<< lightDir.m128_f32[0] << ","
				<< lightDir.m128_f32[1] << ","
				<< lightDir.m128_f32[2] << ")";
			debugText.Print(debugstr.str(), 50, 50, 1.0f);

			debugstr.str("");
			debugstr.clear();

			const XMFLOAT3& cameraPos = { viewMat.eye.x,viewMat.eye.y,viewMat.eye.z };
			debugstr << "cameraPos("
				<< std::fixed << std::setprecision(2)
				<< lightDir.m128_f32[0] << ","
				<< lightDir.m128_f32[1] << ","
				<< lightDir.m128_f32[2] << ")";
			debugText.Print(debugstr.str(), 50, 70, 1.0f);

			light->Update();
		}
	}

	state->Update(soundData);

	//move
	{
		draw[2].worldMat->rot.y = draw[2].worldMat->rot.y + 0.01f;
		draw[3].worldMat->rot.y = draw[3].worldMat->rot.y + 0.01f;
		draw[4].worldMat->rot.y = draw[4].worldMat->rot.y + 0.01f;
		draw[6].worldMat->rot.y = draw[6].worldMat->rot.y + 0.01f;
		draw[2].worldMat->SetWorld();
		draw[3].worldMat->SetWorld();
		draw[4].worldMat->SetWorld();
		draw[6].worldMat->SetWorld();
	}

	{
		cameraWorldMat.rot.y += (KeyboardInput::GetInstance().keyPush(DIK_A) - KeyboardInput::GetInstance().keyPush(DIK_D)) * 0.05f;

		cameraWorldMat.rot.x += (KeyboardInput::GetInstance().keyPush(DIK_W) - KeyboardInput::GetInstance().keyPush(DIK_S)) * 0.05f;
		cameraWorldMat.rot.x = min(cameraWorldMat.rot.x, pi / 2.0f);
		cameraWorldMat.rot.x = max(cameraWorldMat.rot.x, -pi / 2.0f);

		cameraWorldMat.SetWorld();
		viewMat.eye = cameraPos;
		Vec3xM4(viewMat.eye, cameraWorldMat.matWorld, 0);
		viewMat.SetMat();
	}


#ifdef _DEBUG
	if (KeyboardInput::GetInstance().keyTrigger(DIK_E)) ChangeState(new SceneTitle);
#endif 

	//imgui
	imGuiManager->End();
}

void Scene::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{
	draw[0].DrawModel(draw[0].worldMat, &viewMat, &projectionMat, model[0]);
	draw[1].DrawModel(draw[1].worldMat, &viewMat, &projectionMat, model[1]);
	draw[2].DrawModel(draw[2].worldMat, &viewMat, &projectionMat, model[2]);
	draw[3].DrawModel(draw[3].worldMat, &viewMat, &projectionMat, model[3]);
	draw[4].DrawModel(draw[4].worldMat, &viewMat, &projectionMat, model[4]);

	state->Draw(textureHandle, textureNumHundle);

	//スプライト
	draw[5].DrawClippingBoxSprite({ pos[0],pos[1],0 }, 1.0f, { 0,0 }, { 1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, texhandle[2], false, false, false, rot);
	/*draw[8].DrawBoxSprite({ 100,500,0 }, scale, { 1.0f,1.0f,0,1.0f }, texhandle[2], { 0.5f,0.5f }, false);
	draw[9].DrawBoxSprite({ 100,500,0 }, scale, { 1.0f,1.0f,0,1.0f }, texhandle[2], { 0.0f,0.0f },false,true);*/

	draw[6].DrawCube3D(draw[6].worldMat, &viewMat, &projectionMat, { 1,1,1,1 }, texhandle[0]);

	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
}

