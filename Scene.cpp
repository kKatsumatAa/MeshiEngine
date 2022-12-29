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
	scene->player.get()->Initialize(scene->model[2], scene->model[3], scene->playerBulletM.get(), &scene->debugText);
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
	scene->colliderM.get()->Update(scene->player.get(), scene->enemyM.get(), scene->playerBulletM.get(), scene->itemM.get(), scene->stage.get());
	//�J����
	Vec3 pos = scene->player.get()->GetWorldTransForm()->trans;
	//if (scene->stage->isPlayerRoom)
	//{
	//	scene->viewMat.eye = { pos.x,pos.y - 10.0f,scene->viewMat.eye.z };
	//	scene->viewMat.target = { pos.x,pos.y - 10.0f,1.0f };
	//}
	//else
	{
		scene->viewMat.eye = { 0,pos.y - 10.0f,scene->viewMat.eye.z };
		scene->viewMat.target = { 0,pos.y - 10.0f,1.0f };
	}
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
	//�A�C�e��
	scene->itemM.get()->Draw(scene->viewMat, scene->projectionMat);
	//�X�e�[�W
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
		lightManager->SetPointLightAtten(i, { 0.01f,0.01f,0.01f });
		if (i == 0)
		{
			lightManager->SetPointLightColor(i, { 0.8f,0.8f,0.01f });
		}
		else
		{
			lightManager->SetPointLightColor(i, { 1.0f,0.8f,0.3f });
		}
	}
	//�v���C���[�e
	playerBulletM = std::make_unique<PlayerBulletManager>();
	playerBulletM.get()->Initialize(model[2]);
	//player
	player = std::make_unique<Player>();
	player.get()->Initialize(model[2], model[3], playerBulletM.get(), &debugText);
	//�G
	enemyM = std::make_unique<EnemyManager>();
	enemyM.get()->Initialize(model[2], player.get());
	//�����蔻��N���X
	colliderM = std::make_unique<ColliderManager>();
	colliderM.get()->Initialize();
	//�A�C�e��
	itemM = std::make_unique<ItemManager>();
	itemM.get()->Initialize(model[2]);
	//�X�e�[�W
	stage = std::make_unique<Stage>();
	stage.get()->Initialize(model[2], enemyM.get(), itemM.get(), lightManager);
	stage.get()->GenerateStage();
	//
	ParticleManager::GetInstance()->Initialize();

	//�X�e�[�g�ύX
	ChangeState(new SceneGame);
}

void Scene::Update()
{
	for (int i = 0; i < 10; i++)
	{
		//XYZ�S��[-5.0f~+5.0f]�Ń����_���ɕ��z
		const float md_pos = 5.0f;
		Vec3 pos = player.get()->GetWorldPos();

		//XYZ�S��[-0.05~+0.05f]�Ń����_���ɕ��z
		const float md_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

		//�d�͂Ɍ����Ă�Y�̂�[-0.001f~0]�Ń����_����
		XMFLOAT3 acc{};
		const float md_acc = 0.001f;
		acc.y = -(float)rand() / RAND_MAX * md_acc;

		//�F
		XMFLOAT4 color{};
		const float md_color = 1.0f;
		color.x = (float)rand() / RAND_MAX * md_color;
		color.y = (float)rand() / RAND_MAX * md_color;
		color.z = (float)rand() / RAND_MAX * md_color;
		color.w = (float)rand() / RAND_MAX * md_color;

		//if (count2 == 0)
		//	particleM->Add(60, pos, vel, acc, 1.0f, 0.0f
		//		, color, color);
		//else if (count2 == 1)
		ParticleManager::GetInstance()->Add(180, { pos.x,pos.y,pos.z }, vel, acc, 1.0f, 0.0f
			, { 1.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,0.0f,1.0f });
	}
	ParticleManager::GetInstance()->Update(&viewMat, &projectionMat);


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

	ParticleManager::GetInstance()->Draw(texhandle[1]);

	//imgui
	imGuiManager->Draw();
}

void Scene::DrawSprite()
{
	state->DrawSprite();
}

