#include"Scene.h"
#include"DebugText.h"



//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");

	//初期化
	WindowsApp::GetInstance();
	Directx::GetInstance();

	DrawInitialize();



	MSG msg{};	//メッセージ

	//キーボード入力初期化
	KeyboardInput::GetInstance();

	//シーン
	Scene* scene = new Scene();
	scene->Initialize();

	//描画初期化処理-------------

   //初期化処理　ここまで//

   //ゲームループ
	while (true) {

		if (WindowsApp::GetInstance().MessegeRoop(msg))
		{
			break;
		}
		if (KeyboardInput::GetInstance().KeyPush(DIK_ESCAPE)) { break; }

		//毎フレーム処理　ここから//
		//キーボード情報の取得開始
		KeyboardInput::GetInstance().Update();
		{
			Directx::GetInstance().DrawUpdate({ 1.0f,0.5,0.15f,0.5f });

			//更新処理
			scene->Update();


			// 4.描画コマンドここから　//-----------
			scene->Draw();


			scene->DrawSprite();

			//
			Directx::GetInstance().DrawUpdate2();
		}

		//マルチパス
		{
			Directx::GetInstance().PreDrawToPera();

			//
			scene->DrawPostEffect();

			// 4.描画コマンドここまで //
			Directx::GetInstance().PostDrawToPera();
		}
		//毎フレーム処理　ここまで//


	}

	delete scene;


	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();

	return 0;
}


