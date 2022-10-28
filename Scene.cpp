#include "Scene.h"

void SceneState::SetScene(Scene* scene)
{
	//state�ł͂Ȃ�Scene�N���X�̃C���X�^���X
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

	Model.CreateModel("Resources/triangle_tex/triangle_tex.obj");
	Model.worldMat->scale = { 10.0f, 10.0f, 10.0f };

	ChangeState(new SceneTitle);
	state->SetScene(this);
}


void Scene::Update(SoundData* soundData)
{
	state->Update(soundData);

	Model.worldMat->trans.x = Model.worldMat->trans.x + (KeyboardInput::GetInstance().keyPush(DIK_RIGHT) - KeyboardInput::GetInstance().keyPush(DIK_LEFT));
	Model.worldMat->trans.y = Model.worldMat->trans.y + (KeyboardInput::GetInstance().keyPush(DIK_UP) - KeyboardInput::GetInstance().keyPush(DIK_DOWN));
	Model.worldMat->SetWorld();

#ifdef _DEBUG
	if (KeyboardInput::GetInstance().keyTrigger(DIK_E)) ChangeState(new SceneTitle);
#endif 
	
}

void Scene::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{
	Model.DrawModel(Model.worldMat, &viewMat, &projectionMat, { 1.0f,1.0f,1.0f,1.0f },
		textureHandle[1]);
	state->Draw(textureHandle,textureNumHundle);
}

