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
	//��
	scene->StopWaveAllScene();
}

void SceneTitle::Update()
{
	////�V�[���J��
	//if ()
	//{
	//	scene->ChangeState(new SceneGame);
	//}
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
	//��
	scene->StopWaveAllScene();
}

void SceneGame::Update()
{

	////�V�[���J��
	//if ()
	//{
	//	scene->ChangeState(new SceneClear);
	//}
	//else if ()
	//{
	//	scene->ChangeState(new SceneGameOver);
	//}
}

void SceneGame::Draw()
{

}

void SceneGame::DrawSprite()
{

}



//---------------------------------------------------------------------------------------
//�I�����
void SceneGameOver::Initialize()
{

	//��
	scene->StopWaveAllScene();
}

void SceneGameOver::Update()
{

	/*if ()
	{
		scene->ChangeState(new SceneLoad);
	}*/
}

void SceneGameOver::Draw()
{

}

void SceneGameOver::DrawSprite()
{
}


//----------------------------------------------------------------------------------------
void SceneClear::Initialize()
{
	//��
	scene->StopWaveAllScene();
}

void SceneClear::Update()
{

	/*if ()
	{
		scene->ChangeState(new SceneLoad);
	}*/
}

void SceneClear::Draw()
{
}

void SceneClear::DrawSprite()
{

}


//--------------------------------------------------------------------------------------

void SceneLoad::Initialize()
{
	//�񓯊�����
	//async.StartAsyncFunction([=]() { func() });
}

void SceneLoad::Update()
{

	/*if (async.GetLockFlag())
	{
		async.EndThread();

		scene->ChangeState(new SceneTitle);
	}*/
}

void SceneLoad::Draw()
{
}

void SceneLoad::DrawSprite()
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
	this->state->Initialize();
}

void Scene::StopWaveAllScene()
{
}

void Scene::Initialize()
{
	//�����摜
	TextureManager::GetInstance().LoadGraph(L"Resources/image/white.png", TextureManager::GetInstance().whiteTexHandle);

	//�摜
	TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);

	TextureManager::LoadGraph(L"Resources/image/effect1.png", texhandle[1]);


	//��
	{

	}

	//model
	Model::StaticInitialize();

	model[0] = Model::LoadFromOBJ("skydome", true, true);
	draw[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[0].worldMat->SetWorld();
	model[1] = Model::LoadFromOBJ("ground");
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 0.0f, -10.0f, 0 };
	draw[1].worldMat->SetWorld();
	model[2] = Model::LoadFromOBJ("player");
	draw[2].worldMat->scale = { 5.0f, 5.0f, 5.0f };
	draw[2].worldMat->rot.y = { -pi / 2.0f };
	draw[2].worldMat->trans = { fighterPos[0],fighterPos[1],fighterPos[2] };
	draw[2].worldMat->SetWorld();



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
	lightManager->SetDirLightActive(0, true);
	lightManager->SetDirLightActive(1, false);
	lightManager->SetDirLightActive(2, false);
	//�_����
	for (int i = 0; i < 6; i++)
	{
		lightManager->SetPointLightActive(i, false);
	}
	//�ۉe
	lightManager->SetCircleShadowActive(0, true);

	//�J����
	camera = std::make_unique<Camera>();
	camera->Initialize();

	//�X�e�[�g�ύX
	ChangeState(new SceneLoad);
}

void Scene::Update()
{
	//imgui
	imGuiManager->Begin();

	{
		//�f��
		ImGui::ShowDemoWindow();

		//�ۉe
		lightManager->SetCircleShadowDir(0,
			XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2],0 }));
		lightManager->SetCircleShadowCasterPos(0,
			XMFLOAT3({ fighterPos[0],fighterPos[1],fighterPos[2] }));
		lightManager->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
		lightManager->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));
		lightManager->SetCircleShadowDistanceCasterLight(0, circleShadowDistance);

		static bool a = true;
		ImGui::Begin("circleShadow", &a, ImGuiWindowFlags_MenuBar);

		ImGui::InputFloat3("circleShadowDir", circleShadowDir);
		ImGui::InputFloat3("circleShadowAtten", circleShadowAtten);
		ImGui::InputFloat2("circleShadowFactorAngle", circleShadowFactorAngle);
		ImGui::InputFloat3("fihgterPos", fighterPos);
		ImGui::InputFloat("distanceLight", &circleShadowDistance);


		ImGui::End();
		lightManager->Update();

	}

	draw[2].worldMat->trans = { fighterPos[0],fighterPos[1],fighterPos[2] };

	state->Update();


#ifdef _DEBUG
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
#endif 

	//imgui
	imGuiManager->End();
}

void Scene::Draw()
{
	for (int i = 0; i < 3; i++)
	{
		draw[i].DrawModel(draw[i].worldMat, &camera->viewMat, &camera->projectionMat, model[i]);
	}
	draw[3].worldMat->trans = { 0,-10,0 };
	draw[3].worldMat->SetWorld();
	draw[3].DrawCube3D(draw[3].worldMat, &camera->viewMat, &camera->projectionMat);

	state->Draw();
}

void Scene::DrawPostEffect()
{
	draw[2].DrawPera();
}

void Scene::DrawSprite()
{
	state->DrawSprite();

#ifdef _DEBUG
	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
#endif
}
