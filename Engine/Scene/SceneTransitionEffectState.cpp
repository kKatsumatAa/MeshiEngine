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
	//���~�߂�
	Sound::GetInstance().StopWave(SceneTransitionManager::TRANSITION_SOUND_NAME_);

	//�Q�ڂ��m�C�Y
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isNoise = false;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBloom = false;

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
	//������
	Sound::GetInstance().PlayWave(SceneTransitionManager::TRANSITION_SOUND_NAME_, true);

	//��ڃm�C�Y
	PostEffectManager::GetInstance().GetPostEffect2()->effectFlags_.isNoise = true;
	//�O�ڂɕ`�悳�ꂽ���́i�Q���ڂ܂Łj��p��
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBarrelCurve = true;

	//�u���[��
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBloom = true;
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.bloomPow = PostPera::S_BLOOM_POW_;

#ifdef _DEBUG

	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBloom = false;

#endif // debug
}

void TransitionEffectBeginState::Update()
{
	SceneTransitionEffectState::Update();

	//�p�Ȃ����X�ɂ悭
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.barrelCurvePow = EaseInOutBack(GetTimerT(timer_, TIMER_MAX_));
	//2���ڂ̉�ʂ̑傫�������X�ɏ�����
	PostEffectManager::GetInstance().GetPostEffect2()->SetPera2Extend(LerpVec3(
		{ 1.0f ,0,0 }, { PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_ * WINDOW_SIZE_EXTEND_,0,0 },
		EaseInOutBack(GetTimerT(timer_, TIMER_MAX_))).x_);

	//�u���[�����X��
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.bloomPow = LerpVec3(
		{ 0.0f ,0,0 }, { PostPera::S_BLOOM_POW_,0,0 },
		EaseInOutBack(GetTimerT(timer_, TIMER_MAX_))).x_;

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

	//2�ڂ�2���ڂ̉�ʂ̑傫�������X�ɑ傫��
	PostEffectManager::GetInstance().GetPostEffect2()->SetPera2Extend(LerpVec3(
		{ PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_ * WINDOW_SIZE_EXTEND_,0,0 }, { PostEffectManager::GetInstance().DISPLAY_SIZE_MIN_ ,0,0 },
		EaseInOutBack(GetTimerT(timer_, TIMER_MAX_))).x_);

	//�u���[�����X��
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.bloomPow = LerpVec3(
		 { PostPera::S_BLOOM_POW_,0,0 }, { 0.0f ,0,0 },
		EaseInOutBack(GetTimerT(timer_, TIMER_MAX_))).x_;

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