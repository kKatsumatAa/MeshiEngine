#include "MyGame.h"

void MyGame::Initialize()
{
	//基底クラスの初期化処理
	Framework::Initialize();
	
	//ゲーム固有の初期化


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
	{
		DirectXWrapper::GetInstance().DrawUpdate();

		// 4.描画コマンドここから　//-----------
		scene->Draw();

		//
		DirectXWrapper::GetInstance().DrawUpdate2();
	}

	//マルチパス
	{
		//2枚目--------------------
		DirectXWrapper::GetInstance().PreDrawToPera();

		//
		scene->DrawPostEffect();

		DirectXWrapper::GetInstance().PostDrawToPera();

	}
	{
		//実際に描画----------------
		DirectXWrapper::GetInstance().PreDrawToPera2();

		//
		scene->DrawPostEffect2();

		scene->DrawSprite();

		// 4.描画コマンドここまで //
		DirectXWrapper::GetInstance().PostDrawToPera2();
	}
}
