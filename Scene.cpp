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
	LoadGraph(L"Resources/image/white.png", texhandle[0]);
	LoadGraph(L"Resources/image/particle.png", texhandle[1]);
	LoadGraph(L"Resources/image/p.jpg", texhandle[2]);

	model[0].CreateModel("skydome");
	draw[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	model[1].CreateModel("ground");
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 10.0f, -10.0f, 0 };
	model[2].CreateModel("boss");
	draw[2].worldMat->scale = { 5,5,5 };
	/*draw[2].worldMat->rot = { pi / 2.0f, 0, 0 };
	draw[2].worldMat->trans = { 10.0f, 10.0f, 0 };*/

	draw[3].worldMat->trans.x += 30.0f;
	draw[6].worldMat->trans.x -= 30.0f;
	draw[7].worldMat->trans.z -= 20.0f;

	//当たり判定用
	tama[0].centor = { 0.0f,draw[2].worldMat->trans.y,0.0f };
	tama[0].radius = draw[2].worldMat->scale.y;

	tama[1].centor = { 0,0,0 };
	tama[1].radius = 5;

	plane.distance = draw[1].worldMat->trans.y;
	plane.normal = { 0,1,0 };

	triangle.p0 = XMVectorSet(-10.0f, 0, -1.0f, 1);//左手前
	triangle.p1 = XMVectorSet(-10.0f, 0, +1.0f, 1);//左手前
	triangle.p2 = XMVectorSet(+10.0f, 0, -1.0f, 1);//左手前
	triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0);//上向き


	ChangeState(new SceneTitle);
	state->SetScene(this);
}


void Scene::Update(SoundData* soundData)
{
	state->Update(soundData);

	//move
	{
		draw[4].worldMat->trans.x = draw[4].worldMat->trans.x + (KeyboardInput::GetInstance().keyPush(DIK_RIGHT) - KeyboardInput::GetInstance().keyPush(DIK_LEFT));
		draw[4].worldMat->trans.y = draw[4].worldMat->trans.y + (KeyboardInput::GetInstance().keyPush(DIK_UP) - KeyboardInput::GetInstance().keyPush(DIK_DOWN));
		tama[0].centor.m128_f32[0] = draw[4].worldMat->trans.x;
		tama[0].centor.m128_f32[1] = draw[4].worldMat->trans.y;
		tama[0].centor.m128_f32[2] = draw[4].worldMat->trans.z;
	}
	draw[2].worldMat->SetWorld();


#ifdef _DEBUG
	if (KeyboardInput::GetInstance().keyTrigger(DIK_E)) ChangeState(new SceneTitle);
#endif 

}

float rot = 0;
float scale = 1.0f;
float uvwidth = 0;
float color = 0;//

void Scene::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{
	draw[0].DrawModel(draw[0].worldMat, &viewMat, &projectionMat, &model[0]);
	draw[1].DrawModel(draw[1].worldMat, &viewMat, &projectionMat, &model[1]);
	draw[2].DrawModel(draw[2].worldMat, &viewMat, &projectionMat, &model[2]);
	draw[3].DrawModel(draw[3].worldMat, &viewMat, &projectionMat, &model[2]);
	draw[4].DrawSphere(draw[4].worldMat, &viewMat, &projectionMat, { 1.0f,1.0f,1.0f,1.0f }, texhandle[0]);

	rot += 1.0f;
	//scale += 0.01f;
	uvwidth += 0.005f;
	color += 0.005f;
	draw[6].DrawCube3D(draw[6].worldMat, &viewMat, &projectionMat, { 1.0f,0.0f,1.0f,1.0f }, texhandle[0]);
	draw[7].DrawLine(draw[7].worldMat, &viewMat, &projectionMat, { 1.0f,1.0f,1.0f,1.0f }, texhandle[0]);

	state->Draw(textureHandle, textureNumHundle);

	draw[5].DrawClippingBoxSprite({ 0,0,0 }, scale, { 0,0.2f }, { uvwidth,0.8f }, { 0,color,0,1.0f }, texhandle[2], false, false, false, rot);
	draw[8].DrawBoxSprite({ 100,500,0 }, scale, { 1.0f,1.0f,0,1.0f }, texhandle[2], { 0.5f,0.5f }, false);
	draw[9].DrawBoxSprite({ 100,500,0 }, scale, { 1.0f,1.0f,0,1.0f }, texhandle[2], { 0.0f,0.0f },false,true);


	debugText.Printf("posX:", 0, 22, draw[4].worldMat->trans.x);
	debugText.Printf("posY:", 0, 34, draw[4].worldMat->trans.y);
	debugText.Printf("posZ:", 0, 46, draw[4].worldMat->trans.z);

	//当たり判定
	XMVECTOR inter;
	if (Collision::CheckSphere2Plane(tama[0], plane, &inter))
	{
		debugText.Print("hit", 0, 10);
	}
	XMVECTOR inter2;
	if (Collision::CheckSphere2Triangle(tama[0], triangle, &inter2))
	{
		debugText.Print("hitTriangle", 0, 60);
		debugText.Printf("inter.x:", 0, 64 + 14, inter2.m128_f32[0]);
		debugText.Printf("      y:", 0, 78 + 14, inter2.m128_f32[1]);
		debugText.Printf("      z:", 0, 92 + 14, inter2.m128_f32[2]);
	}


	debugText.DrawAll(debugTextHandle);
}

