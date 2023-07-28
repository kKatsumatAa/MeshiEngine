#pragma once
#include"PostEffectManager.h"
#include"Async.h"

//�O���錾
class SceneTransitionEffectState;

//-----------------------------------
//�V�[���J�ډ��o
class SceneTransitionManager final
{
private:
	//���o����
	bool isDoingEffect_ = false;
	//���[�h�̊֐����Ăяo���̂�ҋ@��
	//���o�X�e�[�g
	std::unique_ptr<SceneTransitionEffectState> effectState_;

	//�񓯊��p
	Async async_;

	//�ǂݍ��݂̊֐��ۑ��p
	std::function<void()>loadFunc_;

public:
	static const std::string TRANSITION_SOUND_NAME_;


private:
	SceneTransitionManager() { ; }
	~SceneTransitionManager() { ; }

public:
	SceneTransitionManager& operator=(const SceneTransitionManager& inst) = delete;
	SceneTransitionManager(const SceneTransitionManager& inst) = delete;
	//�C���X�^���X�擾
	static SceneTransitionManager& GetInstance();

public:
	void Initialize();
	void Update();
	void Draw();

	void ChangeEffectState(std::unique_ptr<SceneTransitionEffectState> state);

private:

public:
	void SetIsDoingEffect(bool isDoingEffect) { isDoingEffect_ = isDoingEffect; }
	bool GetIsDoingEffect() { return isDoingEffect_; }

	//�ǂݍ��ݒ���
	bool GetIsLoadingOnly() { return async_.GetIsLoading(); }
	//�V�[���J�ډ��o����(���o�S��)
	bool GetIsDoingLoadEffect() { return isDoingEffect_; }

	//�V�[���J�ډ��o�J�n
	void BeginSceneTransition(const std::function<void()>& loadFunc);

	//�a�����Ă����ǂݍ��ݗp�̊֐���񓯊��ŊJ�n
	void BeginAsyncLoad(std::function<void()> loadFunc);
};
