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

	//非同期処理(ステージ作成中にもロード画面出す的な)
	async.StartAsyncFunction([=]() { this->Load(); });
}

void SceneLoad::Update()
{


	////シーン遷移
	//if (async.GetLockFlag())
	//{
	//	async.EndThread();

	//	//ステージ作り終わったら
	//	sceneM->ChangeState(new Scene5);
	//}
}

void SceneLoad::Draw()
{
}

void SceneLoad::DrawSprite()
{
	count++;
	loadObj.DrawBoxSprite({ 0,0 + sinf(count * 0.1f) * 3.0f,0 }, 1.0f, { 1.0f,1.0f,1.0f,1.0f }, sceneM->texhandle[3]);
}

void SceneLoad::DrawImgui()
{
}

void SceneLoad::DrawPostEffect()
{
}

void SceneLoad::DrawPostEffect2()
{
}
