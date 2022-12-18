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



//---------------------------------------------------------------------------------------
//�Q�[��
void SceneGame::Initialize()
{
	scene->player.get()->Initialize(scene->model[2], scene->model[3]);
}

void SceneGame::Update()
{
	scene->player.get()->Update();

	Vec3 pos = scene->player.get()->GetWorldTransForm()->trans;
	scene->viewMat.eye = { scene->viewMat.eye.x,pos.y,scene->viewMat.eye.z };
	scene->viewMat.target = { 0,pos.y,1.0f };
	scene->viewMat.SetMat();

	//�V�[���J��
	//if (1) scene->ChangeState(new SceneEnd);
}

void SceneGame::Draw()
{
	scene->player.get()->Draw(scene->viewMat, scene->projectionMat);
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
	//�摜
	TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);
	TextureManager::LoadGraph(L"Resources/image/white.png", texhandle[0]);

	//model
	Model::StaticInitialize();

	model[0] = Model::LoadFromOBJ("skydome");
	draw[0].worldMat->scale = { 100.0f, 100.0f, 100.0f };
	model[1] = Model::LoadFromOBJ("ground");
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 10.0f, -5.0f, 0 };
	model[2] = Model::LoadFromOBJ("cup_green_obj");
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
	Draw::SetLight(lightManager);

	//�_����
	lightManager->SetDirLightActive(0, false);
	lightManager->SetDirLightActive(1, false);
	lightManager->SetDirLightActive(2, false);
	lightManager->SetPointLightActive(0, true);
	pointLightPos[0] = 0.5f;
	pointLightPos[1] = 1.0f;
	pointLightPos[2] = 0.0f;

	//player
	player = std::make_unique<Player>();
	player.get()->Initialize(model[2], model[3]);

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

		{
			//�_����
			lightManager->SetPointLightPos(0, XMFLOAT3(pointLightPos));
			lightManager->SetPointLightColor(0, XMFLOAT3(pointLightColor));
			lightManager->SetPointLightAtten(0, XMFLOAT3(pointLightAtten));

			static bool a = true;
			ImGui::Begin("PointLight", &a, ImGuiWindowFlags_MenuBar);
			ImGui::ColorEdit3("pointLightColor", pointLightColor, ImGuiColorEditFlags_Float);
			ImGui::InputFloat3("pointLightPos", pointLightPos);
			ImGui::InputFloat3("pointLight", pointLightAtten);
			ImGui::End();
			lightManager->Update();
		}
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

	draw[0].DrawModel(draw[0].worldMat, &viewMat, &projectionMat, model[0]);
	draw[1].DrawModel(draw[1].worldMat, &viewMat, &projectionMat, model[1]);

	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
}

