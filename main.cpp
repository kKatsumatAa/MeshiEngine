#include"Scene.h"
#include"DebugText.h"
#include"FbxLoader.h"



//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");

	//FBX
	FbxLoader::GetInstance()->Initialize();

	//初期化
	WindowsApp::GetInstance();
	DirectXWrapper::GetInstance().Initialize();

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

		//毎フレーム処理　ここから//
		//キーボード情報の取得開始
		KeyboardInput::GetInstance().Update();
		PadInput::GetInstance().Update();
		{
			DirectXWrapper::GetInstance().DrawUpdate();

			//更新処理
			scene->Update();


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

		//実際に描画----------------
			DirectXWrapper::GetInstance().PreDrawToPera2();

			//
			scene->DrawPostEffect2();

			scene->DrawSprite();

			// 4.描画コマンドここまで //
			DirectXWrapper::GetInstance().PostDrawToPera2();
		}
		//毎フレーム処理　ここまで//


	}

	delete scene;

	//FBX
	FbxLoader::GetInstance()->Finalize();


	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();

	return 0;
}


