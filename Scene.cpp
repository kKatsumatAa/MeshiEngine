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
	scene->player.get()->Initialize(scene->model[2], scene->model[3], scene->playerBulletM.get(), &scene->debugText, scene->camera.get());
	//��ő��̃C�j�V�����C�Y���ǉ�


}

void SceneGame::Update()
{
	//�e
	scene->playerBulletM.get()->Update();
	//player
	scene->player.get()->Update();
	//�G
	scene->enemyM.get()->Update();
	//�A�C�e��
	scene->itemM.get()->Update();
	//�����蔻��
	scene->colliderM.get()->Update(scene->player.get(), scene->enemyM.get(), scene->playerBulletM.get(), scene->itemM.get(), scene->stage.get(),
		scene->camera.get());
	//�J����
	Vec3 pos = scene->player.get()->GetWorldTransForm()->trans;
	{
		scene->camera->SetEye({ 0,pos.y - 10.0f,scene->camera->viewMat.eye.z });
		scene->camera->SetTarget({ 0,pos.y - 10.0f,1.0f });
	}
	scene->camera->Update();
	//�|�C���g���C�g
	scene->lightManager->SetPointLightPos(0, { pos.x,pos.y,pos.z - 10.0f });
	scene->lightManager->Update();

	ParticleManager::GetInstance()->Update(&scene->camera->viewMat, &scene->camera->projectionMat);
	ReloadEffectManager::GetInstance().Update();

	scene->cartridgeEffectM->Update();
	scene->breakEffectM->Update();

	//�V�[���J��
	//if (1) scene->ChangeState(new SceneEnd);
}

void SceneGame::Draw()
{
	//player
	scene->player.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//�e
	scene->playerBulletM.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//�G
	scene->enemyM.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//�A�C�e��
	scene->itemM.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//�X�e�[�W
	scene->stage.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//�p�[�e�B�N��
	ParticleManager::GetInstance()->Draw(scene->texhandle[1]);
	//���
	scene->cartridgeEffectM->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//
	scene->breakEffectM->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//
	ReloadEffectManager::GetInstance().Draw(scene->camera->viewMat, scene->camera->projectionMat);
}

void SceneGame::DrawSprite()
{
	scene->playerBulletM.get()->DrawSprite();
	scene->player.get()->DrawSprite();
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
//�f�X�g���N�^
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
	itemM.reset();
	breakEffectM.reset();
	camera.reset();
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

	TextureManager::LoadGraph(L"Resources/image/effect1.png", texhandle[1]);

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
	for (int i = 0; i < LightManager::PointLightNum; i++)
	{
		lightManager->SetPointLightActive(i, true);
		lightManager->SetPointLightAtten(i, { 0.005f,0.005f,0.005f });
		if (i == 0)
		{
			lightManager->SetPointLightColor(i, { 0.8f,0.8f,0.01f });
		}
		else
		{
			lightManager->SetPointLightColor(i, { 0.3f,0.5f,0.8f });
		}
	}
	//�J����
	camera = std::make_unique<Camera>();
	//���
	cartridgeEffectM = std::make_unique<CartridgeEffectManager>();
	cartridgeEffectM->Initialize();
	//�v���C���[�e
	playerBulletM = std::make_unique<PlayerBulletManager>();
	playerBulletM.get()->Initialize(model[2], cartridgeEffectM.get(), camera.get());
	//player
	player = std::make_unique<Player>();
	player.get()->Initialize(model[2], model[3], playerBulletM.get(), &debugText, camera.get());
	//�G
	enemyM = std::make_unique<EnemyManager>();
	enemyM.get()->Initialize(model[2], player.get());
	//�����蔻��N���X
	colliderM = std::make_unique<ColliderManager>();
	colliderM.get()->Initialize();
	//�A�C�e��
	itemM = std::make_unique<ItemManager>();
	itemM.get()->Initialize(model[2]);
	//
	breakEffectM = std::make_unique<BreakEffectManager>();
	breakEffectM.get()->Initialize();
	//�X�e�[�W
	stage = std::make_unique<Stage>();
	stage.get()->Initialize(model[2], enemyM.get(), itemM.get(), lightManager, breakEffectM.get());
	stage.get()->GenerateStage();
	//
	ParticleManager::GetInstance()->Initialize();
	ReloadEffectManager::GetInstance().Initialize();



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
	draw[1].DrawModel(draw[1].worldMat, &camera->viewMat, &camera->projectionMat, model[1]);

	state->Draw();

	//imgui
	imGuiManager->Draw();
}

void Scene::DrawSprite()
{
	state->DrawSprite();

	debugText.DrawAll(debugTextHandle);
}

