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
		postPera->PreDraw();

		// 4.描画コマンドここから　//-----------
		sceneM->Draw();

		//
		postPera->PostDraw();
	}

	//マルチパス
	{
		//2枚目--------------------
		postPera->Draw2All();
	}

	//実際に描画
	{
		//実際に描画----------------
		DirectXWrapper::GetInstance().PreDraw();

		//
		//sceneM->DrawPostEffect2();
		//実際に描画
		postPera->Draw2();

		sceneM->DrawSprite();

		imguiM->Draw();

		// 4.描画コマンドここまで //
		DirectXWrapper::GetInstance().PostDraw();
	}
}
