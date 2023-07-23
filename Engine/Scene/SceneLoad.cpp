#include "SceneLoad.h"
#include "SceneManager.h"


//--------------------------------------------------------------------------------------
void SceneLoad::Load()
{

}

void SceneLoad::Finalize()
{
}

void SceneLoad::Initialize()
{
	////非同期処理(ステージ作成中にもロード画面出す的な)
	//async_.StartAsyncFunction([=]() { sceneM_->GetLoadFunc(); });
}

void SceneLoad::Update()
{
	////シーン遷移
	//if (async_.GetLockFlag())
	//{
	//	async_.EndThread();

	//	//ロード終わったら
	//	sceneM_->ChangeScene(sceneM_->GetAfterLoadNextSceneName());
	//}
}

void SceneLoad::Draw()
{
}

void SceneLoad::DrawSprite()
{
}

void SceneLoad::DrawImgui()
{
}
