#pragma once
#include"SceneManager.h"
#include"DebugText.h"
#include"FbxLoader.h"
#include"AbstractSceneFactory.h"


//�Q�[���S�́i���ʏ����j
class Framework
{
protected:
	SceneManager* sceneM = nullptr;

	MSG msg{};	//���b�Z�[�W

	//�V�[���t�@�N�g���[
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

	std::unique_ptr<PostPera> postPera[2];

	std::unique_ptr <ImGuiManager> imguiM;


public:
	//���z�f�X�g���N�^(�h���N���X�̃f�X�g���N�^���Ăׂ�悤��)
	virtual ~Framework() = default;

	//������
	virtual void Initialize();

	//�I��
	virtual void Finalize();

	//���t���[���X�V
	virtual bool Update();

	//�`��(�p�����Ȃ��ƃC���X�^���X�𐶐��o���Ȃ����邽��)
	virtual void Draw() = 0;

public:
	//���s
	void Run();
};

