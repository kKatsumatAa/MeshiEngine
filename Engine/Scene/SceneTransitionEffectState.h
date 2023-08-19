#pragma once
#include"PostEffectManager.h"


//�O���錾
class SceneTransitionManager;

//�X�e�[�g�e�N���X
class SceneTransitionEffectState
{
protected:
	int32_t timer_ = 0;
	SceneTransitionManager* sceneTransitionManager_ = nullptr;
	//�ǂݍ��ݗp�֐�
	std::function<void()>loadFunc_;

	//���o�̉�ʃT�C�Y�{��
	const float WINDOW_SIZE_EXTEND_ = 0.9f;


public:
	virtual ~SceneTransitionEffectState() { ; }

	virtual void Initialize() = 0;
	virtual void Update();
	virtual void Draw() = 0;

	//���Ԃ̌o�ߊ���
	float GetTimerT(int32_t timer, const int32_t TIMER_MAX) { return (float)timer / (float)TIMER_MAX; }
	//���Ԃ��I�[�o�[������
	bool GetIsTimeOver(int32_t timer, const int32_t TIMER_MAX) { return GetTimerT(timer, TIMER_MAX) >= 1.0f; }

	//�X�e�[�g�̎�����̃|�C���^�Ɠǂݍ��ݗp�֐������炤
	void SetSceneTransitionEffect(SceneTransitionManager* sceneTransitionEffect,
		const std::function<void()>& loadFunc);
};


//���o���Ă��Ȃ�
class TransitionEffectNothingState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 30;

public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};

//���o�͂���
class TransitionEffectBeginState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};

//���o��
class TransitionEffectDoingState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};

//���o�I���
class TransitionEffectEndState : public SceneTransitionEffectState
{
private:
	const int32_t TIMER_MAX_ = 20;

public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};