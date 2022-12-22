#include "Scene.h"
#include <sstream>
#include <iomanip>

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

void SceneTitle::Update()
{
	/*if (1){scene->ChangeState(new SceneGame);}*/
}

void SceneTitle::Draw()
{

}

void SceneTitle::DrawSprite()
{
}



//---------------------------------------------------------------------------------------
//�Q�[��
void SceneGame::Initialize()
{
	scene->player.get()->Initialize(scene->model[2], scene->model[3], scene->playerBulletM.get());
}

void SceneGame::Update()
{
	//�����蔻��
	scene->colliderM.get()->Update(scene->player.get(), scene->enemyM.get(), scene->playerBulletM.get(), scene->stage.get());

	//�e
	scene->playerBulletM.get()->Update();
	//player
	scene->player.get()->Update();
	//�G
	scene->enemyM.get()->Update();
	
	//�J����
	Vec3 pos = scene->player.get()->GetWorldTransForm()->trans;
	scene->viewMat.eye = { scene->viewMat.eye.x,pos.y - 10.0f,scene->viewMat.eye.z };
	scene->viewMat.target = { 0,pos.y - 10.0f,1.0f };
	scene->viewMat.SetMat();
	//�|�C���g���C�g
	scene->lightManager->SetPointLightPos(0, { pos.x,pos.y,pos.z - 10.0f });
	scene->lightManager->Update();


	//�V�[���J��
	//if (1) scene->ChangeState(new SceneEnd);
}

void SceneGame::Draw()
{
	//player
	scene->player.get()->Draw(scene->viewMat, scene->projectionMat);
	//�e
	scene->playerBulletM.get()->Draw(scene->viewMat, scene->projectionMat);
	//�G
	scene->enemyM.get()->Draw(scene->viewMat, scene->projectionMat);

	scene->stage.get()->Draw(scene->viewMat, scene->projectionMat);
}

void SceneGame::DrawSprite()
{
	scene->playerBulletM.get()->DrawSprite();
}



//---------------------------------------------------------------------------------------
//�I�����
void SceneEnd::Initialize()
{

}

void SceneEnd::Update()
{
	//if (1) scene->ChangeState(new SceneTitle);
}

void SceneEnd::Draw()
{

}

void SceneEnd::DrawSprite()
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
	delete lightManager;
	player.reset();
	enemyM.reset();
	colliderM.reset();
	playerBulletM.reset();
	stage.reset();
	//���f�[�^���

}

void Scene::ChangeState(SceneState* state)
{
	if (state) {
		delete this->state;
	}
	this->state = state;
	state->SetScene(this);
}

void Scene::Initialize()
{
	//�����摜
	TextureManager::GetInstance().LoadGraph(L"Resources/image/white.png", TextureManager::GetInstance().whiteTexHandle);

	//�摜
	TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);
	TextureManager::LoadGraph(L"Resources/image/white.png", texhandle[0]);

	//model
	Model::StaticInitialize();

	model[1] = Model::LoadFromOBJ("ground");
	draw[1].worldMat->scale = { 100.0f, 100.0f, 100.0f };
	draw[1].worldMat->trans = { 0, 0, 10.0f };
	draw[1].worldMat->rot = { (-pi / 2.0f), 0, 0 };
	model[2] = Model::LoadFromOBJ("sphere");
	model[3] = Model::LoadFromOBJ("hanger");

	//��
	Sound::LoadWave("a.wav", true);
	Sound::LoadWave("BGM.wav", false);//

	//imgui
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize();

	//Light
	LightManager::StaticInitialize();
	//�C���X�^���X����
	lightManager = LightManager::Create();
	//���C�g�F��ݒ�
	lightManager->SetDirLightColor(0, { 1,1,1 });
	//3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g(�S�̂ň�����L)
	Object::SetLight(lightManager);

	//�_����
	lightManager->SetDirLightActive(0, false);
	lightManager->SetDirLightActive(1, false);
	lightManager->SetDirLightActive(2, false);
	lightManager->SetPointLightActive(0, true);
	lightManager->SetPointLightAtten(0, { 0.01f,0.01f,0.01f });
	lightManager->SetPointLightColor(0, { 0.8f,0.8f,0.01f });

	//
	playerBulletM = std::make_unique<PlayerBulletManager>();
	playerBulletM.get()->Initialize(model[2]);
	//player
	player = std::make_unique<Player>();
	player.get()->Initialize(model[2], model[3], playerBulletM.get());
	//
	enemyM = std::make_unique<EnemyManager>();
	enemyM.get()->Initialize(model[2], player.get());
	//
	colliderM = std::make_unique<ColliderManager>();
	colliderM.get()->Initialize();
	//
	stage = std::make_unique<Stage>();
	stage.get()->Initialize(model[2]);
	stage.get()->GenerateStage();


	//�X�e�[�g�ύX
	ChangeState(new SceneGame);
}

void Scene::Update()
{
	//imgui
	imGuiManager->Begin();

	{
		//�f��
		ImGui::ShowDemoWindow();

	}

	state->Update();




#ifdef _DEBUG
	//if (KeyboardInput::GetInstance().KeyTrigger(DIK_E)) ChangeState(new SceneTitle);

	/*{
		cameraWorldMat.rot.y += (KeyboardInput::GetInstance().KeyPush(DIK_A) - KeyboardInput::GetInstance().KeyPush(DIK_D)) * 0.05f;

		cameraWorldMat.rot.x += (KeyboardInput::GetInstance().KeyPush(DIK_W) - KeyboardInput::GetInstance().KeyPush(DIK_S)) * 0.05f;
		cameraWorldMat.rot.x = min(cameraWorldMat.rot.x, pi / 2.0f);
		cameraWorldMat.rot.x = max(cameraWorldMat.rot.x, -pi / 2.0f);

		cameraWorldMat.SetWorld();
		viewMat.eye = cameraPos;
		Vec3xM4(viewMat.eye, cameraWorldMat.matWorld, 0);
		viewMat.SetMat();
	}*/
#endif 

	//imgui
	imGuiManager->End();
}

void Scene::Draw()
{
	state->Draw();

	draw[1].DrawModel(draw[1].worldMat, &viewMat, &projectionMat, model[1]);

	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
}

void Scene::DrawSprite()
{
	state->DrawSprite();
}

