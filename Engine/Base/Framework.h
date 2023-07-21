#pragma once
#include"SceneManager.h"
#include"DebugText.h"
#include"FbxLoader.h"
#include"AbstractSceneFactory.h"
#include"CameraManager.h"
#include"MouseInput.h"
#include"PostEffectManager.h"


//�Q�[���S�́i���ʏ����j
class Framework
{
protected:
	SceneManager* sceneM_ = nullptr;

	MSG msg_{};	//���b�Z�[�W

	//�V�[���t�@�N�g���[
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

	std::unique_ptr <ImGuiManager> imguiM_;

	bool isEndGame_ = false;

public:
	//���z�f�X�g���N�^(�h���N���X�̃f�X�g���N�^���Ăׂ�悤��)
	virtual ~Framework() = default;

	//������
	virtual void Initialize();

	//�I��
	virtual void Finalize();

	//���t���[���X�V
	virtual void Update();

	//�`��(�p�����Ȃ��ƃC���X�^���X�𐶐��o���Ȃ����邽��)
	virtual void Draw() = 0;

public:
	//���s
	void Run();
};

