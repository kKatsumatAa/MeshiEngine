#include "SceneTransitionManager.h"
#include "SceneTransitionEffectState.h"


const std::string SceneTransitionManager::TRANSITION_SOUND_NAME_ = "noise.wav";



SceneTransitionManager& SceneTransitionManager::GetInstance()
{
	static SceneTransitionManager inst;
	return inst;
}

void SceneTransitionManager::Initialize()
{
	isDoingEffect_ = false;

	//シーン遷移音
	Sound::GetInstance().LoadWave(TRANSITION_SOUND_NAME_, false);

	//ステート
	ChangeEffectState(std::make_unique<TransitionEffectNothingState>());
}

void SceneTransitionManager::ChangeEffectState(std::unique_ptr<SceneTransitionEffectState> state)
{
	if (effectState_)
	{
		effectState_.reset();
	}
	effectState_ = std::move(state);
	effectState_->SetSceneTransitionEffect(this, loadFunc_);
	effectState_->Initialize();
}

void SceneTransitionManager::BeginSceneTransition(const std::function<void()>& loadFunc)
{
	isDoingEffect_ = true;

	//読み込みの関数コピー
	loadFunc_ = loadFunc;

	//ステート変更
	ChangeEffectState(std::make_unique<TransitionEffectBeginState>());
}

void SceneTransitionManager::BeginAsyncLoad(std::function<void()> loadFunc)
{
	//非同期で読み込み開始
	async_.StartAsyncFunction(loadFunc);
}

//---------------------------------------------
void SceneTransitionManager::Update()
{
	//ロード終わった瞬間
	if (async_.GetFinishedAsync())
	{
		async_.EndThread();
	}

	effectState_->Update();
}

void SceneTransitionManager::Draw()
{
	effectState_->Draw();
}
