#pragma once
#include"Scene.h"
#include"DebugText.h"
#include"FbxLoader.h"


//�Q�[���S��
class MyGame
{
private:
	Scene* scene;

	MSG msg{};	//���b�Z�[�W

public:
	//������
	void Initialize();

	//�I��
	void Finalize();

	//���t���[���X�V
	bool Update();

	//�`��
	void Draw();
};

