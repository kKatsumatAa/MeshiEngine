#include "Scene.h"

void SceneState::SetScene(Scene* scene)
{
	//state�ł͂Ȃ�Scene�N���X�̃C���X�^���X//
	this->scene = scene;
}



//---------------------------------------------------------------------------------------
//�^�C�g��
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
//�Q�[��
void SceneGame::Initialize()
{

}

void SceneGame::Update(SoundData* soundData)
{
	//�V�[���J��
	//if (1) scene->ChangeState(new SceneEnd);
}

void SceneGame::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{

}



//---------------------------------------------------------------------------------------
//�I�����
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
//��ɍs������
Scene::~Scene()
{
	delete state;
	for (int i = 0; i < _countof(model); i++)
	{
		delete model[i];
	}
	imGuiManager->Finalize();
	delete imGuiManager;
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
}

void Scene::Update(SoundData* soundData)
{
	//imgui
	imGuiManager->Begin();

	{
		ImGui::SetNextWindowSize({ 500,100 });
		ImGui::SliderFloat2("position", pos, 0.0f,WindowsApp::GetInstance().window_height, "%4.1f");

		ImGui::ShowDemoWindow();
	}

	state->Update(soundData);

	//move
	{
		draw[2].worldMat->rot.y = draw[2].worldMat->rot.y + 0.01f;
		draw[3].worldMat->rot.y = draw[3].worldMat->rot.y + 0.01f;
		draw[4].worldMat->rot.y = draw[4].worldMat->rot.y + 0.01f;
		draw[2].worldMat->SetWorld();
		draw[3].worldMat->SetWorld();
		draw[4].worldMat->SetWorld();
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

	//�X�v���C�g
		draw[5].DrawClippingBoxSprite({ pos[0],pos[1],0}, 1.0f, {0,0}, {1.0f,1.0f}, {1.0f,1.0f,1.0f,1.0f}, texhandle[2], false, false, false, rot);
		/*draw[8].DrawBoxSprite({ 100,500,0 }, scale, { 1.0f,1.0f,0,1.0f }, texhandle[2], { 0.5f,0.5f }, false);
		draw[9].DrawBoxSprite({ 100,500,0 }, scale, { 1.0f,1.0f,0,1.0f }, texhandle[2], { 0.0f,0.0f },false,true);*/


	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
}

