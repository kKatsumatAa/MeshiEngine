#include "Scene.h"
#include <sstream>
#include <iomanip>
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"


void SceneState::SetScene(Scene* scene)
{
	//state�ł͂Ȃ�Scene�N���X�̃C���X�^���X//
	this->scene = scene;
}

//--------------------------------------------------------------------------------------
void SceneLoad::Load()
{
	TextureManager::LoadGraph(L"Resources/image/loading.png", scene->texhandle[3]);
	//�����摜
	TextureManager::GetInstance().LoadGraph(L"Resources/image/white.png", TextureManager::GetInstance().whiteTexHandle);

	//�摜
	TextureManager::LoadGraph(L"Resources/ascii.png", scene->debugTextHandle);

	TextureManager::LoadGraph(L"Resources/image/effect1.png", scene->texhandle[1]);
	TextureManager::LoadGraph(L"Resources/image/a.png", scene->texhandle[2]);


	{
		Sound::GetInstance().LoadWave("Stage_BGM.wav", false);
	}

	scene->model[0] = Model::LoadFromOBJ("skydome", true, true);
	scene->model[1] = Model::LoadFromOBJ("ground");
	scene->model[2] = Model::LoadFromOBJ("player");
	scene->model[4] = Model::LoadFromOBJ("MiG-25PD", true);
	scene->model[3] = Model::LoadFromOBJ("player", true);
}

void SceneLoad::Initialize()
{

	//�񓯊�����(�X�e�[�W�쐬���ɂ����[�h��ʏo���I��)
	async.StartAsyncFunction([=]() { this->Load(); });
}

void SceneLoad::Update()
{


	//�V�[���J��
	if (async.GetLockFlag())
	{
		async.EndThread();

		//�X�e�[�W���I�������
		scene->ChangeState(new SceneBasic);
	}
}

void SceneLoad::Draw()
{
}

void SceneLoad::DrawSprite()
{
	count++;
	loadObj.DrawBoxSprite({ 0,0 + sinf(count * 0.1f) * 3.0f,0 }, 1.0f, { 1.0f,1.0f,1.0f,1.0f }, scene->texhandle[3]);
}

//---------------------------------------------------------------------------------------
void SceneBasic::Initialize()
{
	plane.distance = scene->draw[1].worldMat->trans.y;
	plane.normal = XMVectorSet(0, 1, 0, 0);
	tama.radius = scene->draw[2].worldMat->scale.x;

	scene->draw[2].worldMat->trans = { scene->fighterPos[0],scene->fighterPos[1],scene->fighterPos[2] };
}

void SceneBasic::Update()
{
	count++;

	scene->draw[2].worldMat->trans.y = 10.0f + sinf(count * 0.03f) * 20.0f;
	scene->draw[2].worldMat->SetWorld();

	tama.center =
	{ scene->draw[2].worldMat->trans.x,scene->draw[2].worldMat->trans.y,scene->draw[2].worldMat->trans.z };

	if (Collision::CheckSphere2Plane(tama, plane))
	{
		tamaColor = { 1.0f,0,0,1.0f };
	}
	else
	{
		tamaColor = { 1.0f,1.0f,1.0f,1.0f };
	}

	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		scene->ChangeState(new Scene1);
	}
}

void SceneBasic::Draw()
{


	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawSphere(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, tamaColor);
}

void SceneBasic::DrawSprite()
{
	scene->debugText.Print("[Basic]", 10, 10);
}

//---------------------------------------------------------------------------------------
void Scene1::Initialize()
{
	float angle = pi * 2.0f + pi * 2.0f / 3.0f * 2.0f;
	float angle2 = pi * 2.0f;
	float angle3 = pi * 2.0f + pi * 2.0f / 3.0f;

	float rad = scene->draw[3].worldMat->scale.x;

	triangle.p0 = { sinf(angle) * rad,cosf(angle) * rad,0 };
	triangle.p1 = { 0,cosf(angle2) * rad,0 };
	triangle.p2 = { sinf(angle3) * rad,cosf(angle3) * rad,0 };
	triangle.ComputeNormal();

	tama.radius = scene->draw[2].worldMat->scale.x;

	scene->draw[2].worldMat->trans = { scene->fighterPos[0],scene->fighterPos[1],scene->fighterPos[2] };
}

void Scene1::Update()
{
	count++;

	scene->draw[2].worldMat->trans.z = -10.0f + sinf(count * 0.03f) * 20.0f;
	scene->draw[2].worldMat->SetWorld();

	tama.center =
	{ scene->draw[2].worldMat->trans.x,scene->draw[2].worldMat->trans.y,scene->draw[2].worldMat->trans.z };

	if (Collision::CheckSphere2Triangle(tama, triangle))
	{
		tamaColor = { 1.0f,0,0,1.0f };
	}
	else
	{
		tamaColor = { 1.0f,1.0f,1.0f,1.0f };
	}

	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		scene->ChangeState(new Scene2);
	}
}

void Scene1::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawSphere(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, tamaColor);
	scene->draw[3].DrawTriangle(scene->draw[3].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, { 0.2,0,0.2f,1.0f });
}

void Scene1::DrawSprite()
{
	scene->debugText.Print("[1]", 10, 10);
}

//---------------
void Scene2::Initialize()
{
	ray.dir = { 0,-1.0f,0 };
	ray.start = { 0,20,0 };
	//��
	plane.distance = scene->draw[1].worldMat->trans.y;
	plane.normal = XMVectorSet(0, 1, 0, 0);

	scene->draw[1].worldMat->trans.z = scene->draw[1].worldMat->scale.z*100.0f;
}

void Scene2::Update()
{
	if (Collision::CheckRay2Plane(ray, plane, &distance, &inter))
	{
		tamaColor = { 1.0f,0,0,1.0f };
	}
	else
	{
		distance = 100;
		tamaColor = { 1.0f,1.0f,1.0f,1.0f };
	}

	count++;

	ray.start.m128_f32[1] = -10.0f + sinf(count * 0.03f) * 20.0f;

	//���f���̈ʒu�ƒ���
	scene->draw[4].worldMat->trans = { ray.start.m128_f32[0] + ray.dir.m128_f32[0] * distance / 2.0f,
		ray.start.m128_f32[1] + ray.dir.m128_f32[1] * distance / 2.0f,
	ray.start.m128_f32[2] + ray.dir.m128_f32[2] * distance / 2.0f };

	scene->draw[4].worldMat->scale = { 1.0f,distance / 2.0f,1.0f };

	scene->draw[4].worldMat->SetWorld();


	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		scene->draw[1].worldMat->trans.z = 0;

		scene->ChangeState(new Scene3);
	}
}

void Scene2::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}

	scene->draw[4].DrawCube3D(scene->draw[4].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, tamaColor);
}

void Scene2::DrawSprite()
{
	scene->debugText.Print("[2]", 10, 10);
}

//---------------
void Scene3::Initialize()
{
	//�_����
	for (int i = 0; i < 6; i++)
	{
		scene->lightManager->SetPointLightActive(i, false);
	}
	//�X�|�b�g���C�g
	scene->lightManager->SetSpotLightActive(0, true);
}

void Scene3::Update()
{
	scene->lightManager->SetSpotLightDir(0,
		XMVECTOR({ spotLightDir[0], spotLightDir[1], spotLightDir[2] }));
	scene->lightManager->SetSpotLightPos(0, XMFLOAT3(spotLightPos));
	scene->lightManager->SetSpotLightColor(0, XMFLOAT3(spotLightColor));
	scene->lightManager->SetSpotLightAtten(0, XMFLOAT3(spotLightAtten));
	scene->lightManager->SetSpotLightFactorAngle(0, XMFLOAT2(spotLightFactorAngle));

	static bool a = true;
	ImGui::Begin("spotLight", &a, ImGuiWindowFlags_MenuBar);
	ImGui::InputFloat3("spotLightDir", spotLightDir);
	ImGui::ColorEdit3("spotLightColor", spotLightColor, ImGuiColorEditFlags_Float);
	ImGui::InputFloat3("spotLightPos", spotLightPos);
	ImGui::InputFloat3("spotLightAtten", spotLightAtten);
	ImGui::InputFloat2("spotLightFactorAngle", spotLightFactorAngle);
	ImGui::End();

	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		scene->ChangeState(new Scene4);
	}
}

void Scene3::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
}

void Scene3::DrawSprite()
{
	scene->debugText.Print("[3]", 10, 10);
}

//---------------
void Scene4::Initialize()
{
	ray.dir = { 0,-1.0f,0 };
	ray.start = { 0,20,0 };
	tama.radius = scene->draw[2].worldMat->scale.x;

	tama.center =
	{ scene->draw[2].worldMat->trans.x,scene->draw[2].worldMat->trans.y,scene->draw[2].worldMat->trans.z };
	scene->draw[2].worldMat->trans = { 0,0,0 };
}

void Scene4::Update()
{
	if (Collision::CheckRay2Sphere(ray, tama, &distance, &inter))
	{
		tamaColor = { 1.0f,0,0,1.0f };
	}
	else
	{
		distance = 100;
		tamaColor = { 1.0f,1.0f,1.0f,1.0f };
	}

	count++;

	ray.start.m128_f32[0] = -10.0f + sinf(count * 0.03f) * 20.0f;

	//���f���̈ʒu�ƒ���
	scene->draw[4].worldMat->trans = { ray.start.m128_f32[0] + ray.dir.m128_f32[0] * distance / 2.0f,
		ray.start.m128_f32[1] + ray.dir.m128_f32[1] * distance / 2.0f,
	ray.start.m128_f32[2] + ray.dir.m128_f32[2] * distance / 2.0f };

	scene->draw[4].worldMat->scale = { 1.0f,distance / 2.0f,1.0f };

	scene->draw[4].worldMat->SetWorld();

	//��
	tama.center =
	{ scene->draw[2].worldMat->trans.x,scene->draw[2].worldMat->trans.y,scene->draw[2].worldMat->trans.z };



	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		scene->ChangeState(new Scene5);
	}
}

void Scene4::Draw()
{
	for (int i = 0; i < 1; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}

	scene->draw[2].DrawSphere(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, tamaColor);

	scene->draw[4].DrawCube3D(scene->draw[4].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, tamaColor);
}

void Scene4::DrawSprite()
{
	scene->debugText.Print("[4]", 10, 10);
}

//----------------
void Scene5::Initialize()
{
	scene->lightManager->SetDirLightActive(0, true);
	scene->lightManager->SetDirLightActive(1, true);
	scene->lightManager->SetDirLightActive(2, false);
	scene->lightManager->SetDirLightDir(0, { 0, 0, 1.0 });
	scene->lightManager->SetDirLightDir(1, { 0, -1.0, 0 });
	//�_����
	for (int i = 0; i < 6; i++)
	{
		scene->lightManager->SetPointLightActive(i, false);
	}
	//�ۉe
	scene->lightManager->SetCircleShadowActive(0, false);
	scene->lightManager->SetSpotLightActive(0, false);

	//�R���C�_�[
	CollisionManager::GetInstance()->Initialize();
	collisionManager = CollisionManager::GetInstance();
	objPlayer = Player::Create();
	objPlayer->worldMat->scale = { 2,2,2 };
	obj.worldMat->scale = { 5,5,5 };
	obj.SetCollider(new SphereCollider);
}

void Scene5::Update()
{
	objPlayer->Update();
	obj.Update();
	collisionManager->CheckAllCollisions();

	ParticleManager::GetInstance()->Update(&scene->camera->viewMat, &scene->camera->projectionMat);


	Ray ray;
	ray.start = { 50.0f, 0.5f, 0.0f, 1 };
	ray.dir = { -1,0,0,0 };
	RaycastHit raycastHit;

	if (collisionManager->Raycast(ray, &raycastHit)) {
		scene->debugText.Print("Raycast Hit.", 10, 90);

		for (int i = 0; i < 1; ++i) {

			const float rnd_vel = 1.1f;
			XMFLOAT3 vel{};
			vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
			vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
			vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

			ParticleManager::GetInstance()->Add(10, XMFLOAT3(raycastHit.inter.m128_f32), vel, XMFLOAT3(), 0.0f, 1.0f);
		}
	}

	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		scene->ChangeState(new Scene6);
	}
}

void Scene5::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}

	objPlayer->DrawModel(objPlayer->worldMat,
		&scene->camera->viewMat, &scene->camera->projectionMat, scene->model[3]);

	obj.DrawSphere(obj.worldMat, &scene->camera->viewMat, &scene->camera->projectionMat);

	ParticleManager::GetInstance()->Draw(scene->texhandle[1]);
}

void Scene5::DrawSprite()
{
	Vec2 v = PadInput::GetInstance().GetRightStickTilt();
	Vec2 v2 = PadInput::GetInstance().GetLeftStickTilt();

	scene->debugText.Printf("RX:", 100, 10, v.x);
	scene->debugText.Printf("RY:", 100, 30, v.y);
	scene->debugText.Printf("LX:", 100, 60, v2.x);
	scene->debugText.Printf("LY:", 100, 90, v2.y);

	scene->debugText.Print("[5]", 10, 10);
	scene->debugText.Print("ARROW:move", 10, 30);
}

//-----------------------------------------------------------------------------------
void Scene6::Initialize()
{
	Object::effectFlags.isFog = true;
}

void Scene6::Update()
{
	count++;

	if (count >= countMax)
	{
		if (Object::effectFlags.isFog) {
			Object::effectFlags.isFog = false;
			Object::effectFlags.isEmboss = true;
		}
		else if (Object::effectFlags.isEmboss) {
			Object::effectFlags.isEmboss = false;
			Object::effectFlags.isGaussian2 = true;
		}
		else if (Object::effectFlags.isGaussian2) {
			Object::effectFlags.isGaussian2 = false;
			Object::effectFlags.isGradation = true;
		}
		else if (Object::effectFlags.isGradation) {
			Object::effectFlags.isGradation = false;
			Object::effectFlags.isOutLine = true;
		}
		else if (Object::effectFlags.isOutLine) {
			Object::effectFlags.isOutLine = false;
			Object::effectFlags.isSharpness = true;
		}
		else if (Object::effectFlags.isSharpness) {
			Object::effectFlags.isSharpness = false;
			Object::effectFlags.isFog = true;
		}

		count = 0;
	}

	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		Object::effectFlags.isFog = false;
		Object::effectFlags.isEmboss = false;
		Object::effectFlags.isGaussian2 = false;
		Object::effectFlags.isGradation = false;
		Object::effectFlags.isOutLine = false;
		Object::effectFlags.isSharpness = false;

		scene->ChangeState(new SceneBasic);
	}
}

void Scene6::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
}

void Scene6::DrawSprite()
{
	scene->debugText.Print("[6]", 10, 10);
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
	Sound::GetInstance().StopWave("Stage_BGM.wav");
}

void Scene::Initialize()
{


	//model
	Model::StaticInitialize();

	draw[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[0].worldMat->SetWorld();
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 0.0f, -10.0f, 0 };
	draw[1].worldMat->SetWorld();
	draw[2].worldMat->scale = { 5,5,5 };
	draw[2].worldMat->rot.y = { -pi / 2.0f };
	draw[2].worldMat->trans = { fighterPos[0],fighterPos[1],fighterPos[2] };
	draw[2].worldMat->SetWorld();
	draw[3].worldMat->scale = { 5,5,5 };
	//draw[3].worldMat->rot.y = { -pi / 2.0f };
	//draw[3].worldMat->trans = { 15.0f,0,0 };
	draw[3].worldMat->SetWorld();


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
	lightManager->SetDirLightActive(1, true);
	lightManager->SetDirLightActive(2, false);
	lightManager->SetDirLightDir(0, { 0, 0, 1.0 });
	lightManager->SetDirLightDir(1, { 0, -1.0, 0 });
	//�_����
	for (int i = 0; i < 6; i++)
	{
		lightManager->SetPointLightActive(i, false);
	}
	//�ۉe
	lightManager->SetCircleShadowActive(0, false);

	//�J����
	camera = std::make_unique<Camera>();
	camera->Initialize();


	//objPlayer->Initialize();

	draw[4].SetCollider(new SphereCollider);
	/*draw[4].SetIsValid(false);*/

	ParticleManager::GetInstance()->Initialize();

	//Object::effectFlags.isVignette = true;

	//�X�e�[�g�ύX
	ChangeState(new SceneLoad);
}

void Scene::Update()
{
	//imgui
	imGuiManager->Begin();


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

		//imgui
	imGuiManager->End();
}

void Scene::Draw()
{
	state->Draw();
}

void Scene::DrawPostEffect()
{
	Object::DrawPera();
}

void Scene::DrawSprite()
{
	state->DrawSprite();

	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
}


