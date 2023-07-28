#pragma once
#include"PostEffectManager.h"
#include"Async.h"

//前方宣言
class SceneTransitionEffectState;

//-----------------------------------
//シーン遷移演出
class SceneTransitionManager final
{
private:
	//演出中か
	bool isDoingEffect_ = false;
	//ロードの関数を呼び出すのを待機中
	//演出ステート
	std::unique_ptr<SceneTransitionEffectState> effectState_;

	//非同期用
	Async async_;

	//読み込みの関数保存用
	std::function<void()>loadFunc_;

public:
	static const std::string TRANSITION_SOUND_NAME_;


private:
	SceneTransitionManager() { ; }
	~SceneTransitionManager() { ; }

public:
	SceneTransitionManager& operator=(const SceneTransitionManager& inst) = delete;
	SceneTransitionManager(const SceneTransitionManager& inst) = delete;
	//インスタンス取得
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

	//読み込み中か
	bool GetIsLoadingOnly() { return async_.GetIsLoading(); }
	//シーン遷移演出中か(演出全体)
	bool GetIsDoingLoadEffect() { return isDoingEffect_; }

	//シーン遷移演出開始
	void BeginSceneTransition(const std::function<void()>& loadFunc);

	//預かっていた読み込み用の関数を非同期で開始
	void BeginAsyncLoad(std::function<void()> loadFunc);
};
