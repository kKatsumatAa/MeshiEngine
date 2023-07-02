#include "MyGame.h"
#include"SceneFactory.h"


void MyGame::Initialize()
{
	//基底クラスの初期化処理
	Framework::Initialize();

	//ゲーム固有の初期化

	//シーンファクトリーを生成し、マネージャーにセット
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneM_->SetSceneFactory(sceneFactory_.get());
	sceneM_->ChangeScene("GAME");

	postPera_[1]->effectFlags_.isBarrelCurve = true;
	postPera_[0]->effectFlags_.isBloom = true;
	postPera_[0]->effectFlags_.isVignette = true;
}

void MyGame::Finalize()
{
	//ゲーム固有の終了処理


	//基底クラスの終了処理
	Framework::Finalize();
}

void MyGame::Update()
{
	//基底クラスの更新処理
	Framework::Update();

	//ゲーム固有の更新処理
		//パーティクル
	ParticleManager::GetInstance()->Update();
}

void MyGame::Draw()
{
	//1枚目に描画
	{
		std::function<void()>f = [=]() {sceneM_->Draw(); };
		postPera_[0]->DrawToPostpera(f);
		postPera_[0]->DrawShrinkTextureForBlur();
		//一枚目に描画結果、二枚目も描画する
		std::function<void()>f2 = [=]() { postPera_[0]->Draw2();  };
		postPera_[1]->DrawToPostpera(f2);
	}

	//ブルーム用
	{
		postPera_[1]->DrawShrinkTextureForBlur();
	}

	//実際に描画
	{
		//実際に描画----------------
		DirectXWrapper::GetInstance().PreDraw();

		//実際に描画
		postPera_[1]->Draw2();

		sceneM_->DrawSprite();

		imguiM_->Draw();

		// 4.描画コマンドここまで //
		DirectXWrapper::GetInstance().PostDraw();
	}
}
