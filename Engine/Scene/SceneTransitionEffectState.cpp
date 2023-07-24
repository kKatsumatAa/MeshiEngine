#include "SceneTransitionEffectState.h"
#include "SceneTransitionManager.h"
#include "Util.h"


void SceneTransitionEffectState::SetSceneTransitionEffect(SceneTransitionManager* sceneTransitionEffect,
	const std::function<void()>& loadFunc)
{
	sceneTransitionManager_ = sceneTransitionEffect;
	loadFunc_ = loadFunc;
}

void SceneTransitionEffectState::Update()
{
	timer_++;
}

//--------------------------------------------------------
//�������Ă��Ȃ�
void TransitionEffectNothingState::Initialize()
{
	//�m�C�Y
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isNoise = false;

	//���o�t���O�I�t
	sceneTransitionManager_->SetIsDoingEffect(false);
}

void TransitionEffectNothingState::Update()
{
	SceneTransitionEffectState::Update();
}

void TransitionEffectNothingState::Draw()
{
}


//--------------------------------------------------------
//���o�n��
void TransitionEffectBeginState::Initialize()
{
	//�m�C�Y
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.isNoise = true;
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isBarrelCurve = true;
}

void TransitionEffectBeginState::Update()
{
	SceneTransitionEffectState::Update();

	//�p�Ȃ����X�ɂ悭
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.barrelCurvePow = EaseOut(GetTimerT(timer_, TIMER_MAX_));
	//�ꖇ�ڂ̉�ʂ̑傫�������X�ɏ�����
	PostEffectManager::GetInstance().GetPostEffect1()->SetPera2Extend(LerpVec3(
		{ 1.0f ,0,0 }, { PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_,0,0 },
		EaseOut(GetTimerT(timer_, TIMER_MAX_))).x_);

	//���Ԃ��I�������
	if (GetIsTimeOver(timer_, TIMER_MAX_))
	{
		//���o�Ȃ��X�e�[�g��
		sceneTransitionManager_->ChangeEffectState(std::make_unique<TransitionEffectDoingState>());
	}
}

void TransitionEffectBeginState::Draw()
{
}


//--------------------------------------------------------
//���o��
void TransitionEffectDoingState::Initialize()
{
	//�a�����Ă������[�h�p�̊֐������s
	sceneTransitionManager_->BeginAsyncLoad(loadFunc_);
}

void TransitionEffectDoingState::Update()
{
	SceneTransitionEffectState::Update();

	//���[�f�B���O�I�������
	if (!sceneTransitionManager_->GetIsLoadingOnly() && GetIsTimeOver(timer_, TIMER_MAX_))
	{
		sceneTransitionManager_->ChangeEffectState(std::make_unique<TransitionEffectEndState>());
	}
}

void TransitionEffectDoingState::Draw()
{
}


//--------------------------------------------------------
//���o�I���
void TransitionEffectEndState::Initialize()
{
}

void TransitionEffectEndState::Update()
{
	SceneTransitionEffectState::Update();

	//���Ԃ��I�������
	if (GetIsTimeOver(timer_, TIMER_MAX_))
	{
		//���o�Ȃ��X�e�[�g��
		sceneTransitionManager_->ChangeEffectState(std::make_unique<TransitionEffectNothingState>());
	}
}

void TransitionEffectEndState::Draw()
{
}