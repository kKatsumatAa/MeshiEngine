#include "KeyboardInput.h"
#include "WindowsApp.h"
#include "DirectX.h"

KeyboardInput keys;
WindowsApp win;
Directx directx(win);

//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");

	MSG msg{};	//メッセージ

	//初期化処理　ここから//


	//キーボード入力初期化
	keys.Initialize(directx.result, win.hwnd, win.w);
	

	 //描画初期化処理-------------
	
	directx.DrawInitialize();

	//初期化処理　ここまで//

	//ゲームループ
	while (true) {

		if (win.MessegeRoop(msg))
		{
			break;
		}

		//毎フレーム処理　ここから//
		//キーボード情報の取得開始
		keys.Update();

		directx.GraphicsCommand();

		//数字の0キーが押されていたら
		if (keys.keyTrigger(DIK_0))
		{
			OutputDebugStringA("HIT 0 \n");
		}

		
		if (keys.keyTrigger(DIK_SPACE))
		{
			FLOAT clearColor[] = { 0.8f,0.8f, 0.0f,0.0f };
			directx.commandList->ClearRenderTargetView(directx.rtvHandle, clearColor, 0, nullptr);
		}

		// 4.描画コマンドここから　//-----------

		directx.DrawUpdate(win);

		// 4.描画コマンドここまで //

		directx.GraphicsCommand2();

		//毎フレーム処理　ここまで//
	}

	//ウィンドウクラスを登録解除
	win.UnregisterClassA();

	return 0;
}


