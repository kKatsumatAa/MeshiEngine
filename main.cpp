#include "KeyboardInput.h"
#include "WindowsApp.h"
#include "DirectX.h"



//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");

	MSG msg{};	//メッセージ

	//初期化処理　ここから//
	KeyboardInput keys;
	WindowsApp win;

	D3D12_VIEWPORT viewport[4] = {
		{0,0,win.window_width * (2.0 / 3.0), win.window_height * (2.0 / 3.0), 0.0f,1.0f},
		{ win.window_width * (2.0 / 3.0),0,win.window_width * (1.0 / 3.0), win.window_height * (2.0 / 3.0),
		0.0f,1.0f },
		{ 0,win.window_height * (2.0 / 3.0),win.window_width * (2.0 / 3.0), win.window_height * (1.0 / 3.0),
		0.0f,1.0f },
		{ win.window_width * (2.0 / 3.0),win.window_height * (2.0 / 3.0),win.window_width * (1.0 / 3.0), win.window_height * (1.0 / 3.0),
		0.0f,1.0f }
	};

	Directx directx(win);

	int pipelineNum = 0;
	bool primitiveNum = false;

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

		directx.GraphicsCommand({ 0.1f,0.25f,0.5f,0.0f });

		//数字の0キーが押されていたら
		if (keys.keyTrigger(DIK_0))
		{
			OutputDebugStringA("HIT 0 \n");
		}

		
		if (keys.keyPush(DIK_SPACE))
		{
			FLOAT clearColor[] = { 0.8f,0.0f, 0.8f,0.0f };
			directx.commandList->ClearRenderTargetView(directx.rtvHandle, clearColor, 0, nullptr);
		}

		// 4.描画コマンドここから　//-----------
		if (keys.keyTrigger(DIK_1))
		{
			if (primitiveNum) primitiveNum = 0;
			else              primitiveNum = 1;
		}
		if (keys.keyTrigger(DIK_2))
		{
			if (pipelineNum)  pipelineNum = 0;
			else              pipelineNum = 1;
		}
		for (int i = 0; i < 4; i++)
		{
			directx.DrawUpdate(win,viewport[i],pipelineNum,primitiveNum);
		}

		// 4.描画コマンドここまで //

		directx.GraphicsCommand2();

		//毎フレーム処理　ここまで//

		if (keys.keyPush(DIK_ESCAPE)) break;
	}

	//ウィンドウクラスを登録解除
	win.UnregisterClassA();

	return 0;
}


