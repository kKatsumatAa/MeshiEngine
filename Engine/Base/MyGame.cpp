#include "MyGame.h"
#include"SceneFactory.h"


void MyGame::Initialize()
{
	//基底クラスの初期化処理
	Framework::Initialize();

	//ゲーム固有の初期化

	//シーンファクトリーを生成し、マネージャーにセット
	sceneFactory_ = new SceneFactory();
	sceneM->SetSceneFactory(sceneFactory_);
	sceneM->ChangeScene("GAME");

	postPera[0]->effectFlags.isBloom = true;
}

void MyGame::Finalize()
{
	//ゲーム固有の終了処理


	//基底クラスの終了処理
	Framework::Finalize();
}

bool MyGame::Update()
{
	//基底クラスの更新処理
	if (Framework::Update())
	{
		return true;
	}

	//ゲーム固有の更新処理


	return false;
}

void MyGame::Draw()
{
	//1枚目に描画
	{
		std::function<void()>f = [=]() {sceneM->Draw(); };
		//一枚目に描画結果、二枚目も描画する
		postPera[0]->DrawToPostpera(f);
		//std::function<void()>f2 = [=]() {postPera[0]->Draw2(); };
		//postPera[1]->DrawToPostpera(f2);
	}

	//ブルーム用
	{
		postPera[0]->DrawShrinkTextureForBlur();
	}

	//実際に描画
	{
		//実際に描画----------------
		DirectXWrapper::GetInstance().PreDraw();

		//
		//sceneM->DrawPostEffect2();
		//実際に描画
		postPera[0]->Draw2();

		sceneM->DrawSprite();

		imguiM->Draw();

		// 4.描画コマンドここまで //
		DirectXWrapper::GetInstance().PostDraw();
	}
}
