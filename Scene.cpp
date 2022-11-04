#include "Scene.h"

void SceneState::SetScene(Scene* scene)
{
	//stateではなくSceneクラスのインスタンス
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

	LoadGraph(L"Resources/ascii.png", debugTextHandle);

	Model[0].CreateModel("skydome");
	Model[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	Model[1].CreateModel("ground");
	Model[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	Model[1].worldMat->trans = { 10.0f, -10.0f, 0 };
	Model[2].CreateModel("ufo_");
	Model[2].worldMat->scale = { 5.0f, 5.0f, 5.0f };
	Model[2].worldMat->rot = { pi / 2.0f, 0, 0 };
	Model[2].worldMat->trans = { 10.0f, 10.0f, 0 };

	Model[3].CreateModel("ufo_");
	Model[3].worldMat->trans.x += 30.0f;

	//当たり判定用
	tama[0].centor = { 0.0f,Model[2].worldMat->trans.y,0.0f };
	tama[0].radius = Model[2].worldMat->scale.y;

	tama[1].centor = { 0,0,0 };
	tama[1].radius = 5;

	plane.distance = Model[1].worldMat->trans.y;
	plane.normal = { 0,1,0 };


	ChangeState(new SceneTitle);
	state->SetScene(this);
}


void Scene::Update(SoundData* soundData)
{
	state->Update(soundData);

	{

		Model[2].worldMat->trans.x = Model[2].worldMat->trans.x + (KeyboardInput::GetInstance().keyPush(DIK_RIGHT) - KeyboardInput::GetInstance().keyPush(DIK_LEFT));
		Model[2].worldMat->trans.y = Model[2].worldMat->trans.y + (KeyboardInput::GetInstance().keyPush(DIK_UP) - KeyboardInput::GetInstance().keyPush(DIK_DOWN));
		tama[0].centor.m128_f32[1] = Model[2].worldMat->trans.y;
	}
	Model[2].worldMat->SetWorld();


#ifdef _DEBUG
	if (KeyboardInput::GetInstance().keyTrigger(DIK_E)) ChangeState(new SceneTitle);
#endif 

}

void Scene::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{
	for (int i = 0; i < 4; i++)
	{
		Model[i].DrawModel(Model[i].worldMat, &viewMat, &projectionMat);
	}
	state->Draw(textureHandle, textureNumHundle);

	debugText.Printf("posX:", 0, 22, tama[0].centor.m128_f32[0]);
	debugText.Printf("posY:", 0, 34, tama[0].centor.m128_f32[1]);
	debugText.Printf("posZ:", 0, 46, tama[0].centor.m128_f32[2]);

	if (Collision::CheckSphere2Plane(tama[0], plane))
	{
		debugText.Print("hit", 0, 10);
	}

	debugText.DrawAll(debugTextHandle);
}

