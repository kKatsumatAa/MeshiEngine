#include "KeyboardInput.h"
#include "WindowsApp.h"
#include "DirectX.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
#include "WorldMat.h"


//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");

	MSG msg{};	//メッセージ

	//初期化処理　ここから//
	KeyboardInput keys;
	WindowsApp win;
	ViewMat viewMat(0.0f, 0.0f, -100.0f
			, 0.0f, 0.0f, 0.0f
			, 0.f, 1.f, 0.f );
	ProjectionMat projectionMat(win);
	WorldMat worldMat;
	worldMat.scale = { 1.0f, 0.5f, 1.0f };
	worldMat.rot = { 15.f, 30.0f, 0.0f };
	worldMat.trans = { -50.f, 0.0f, 0.0f };
	worldMat.SetWorld();

	XMFLOAT3 position;

	Directx directx(win);

	int pipelineNum = 0;
	bool primitiveNum = false;

	//キーボード入力初期化
	keys.Initialize(directx.result, win.hwnd, win.w);
	

	 //描画初期化処理-------------
	
	directx.DrawInitialize(win);

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
		

		if (keys.keyPush(DIK_D) || keys.keyPush(DIK_A))//カメラ
		{
			if (keys.keyPush(DIK_D)) { viewMat.angle += XMConvertToRadians(1.0f); }
			else if (keys.keyPush(DIK_A)) { viewMat.angle -= XMConvertToRadians(1.0f); }

			//長さを決めてy軸周りを回す
			viewMat.eye.x = -100 * sinf(viewMat.angle);
			viewMat.eye.z = -100 * cosf(viewMat.angle);
		}
		viewMat.SetMat();

		if (keys.keyPush(DIK_DOWN) || keys.keyPush(DIK_UP)|| keys.keyPush(DIK_LEFT)|| keys.keyPush(DIK_RIGHT))//translasion
		{
			if (keys.keyPush(DIK_DOWN)) { worldMat.trans.y -= 1.0f; }
			else if (keys.keyPush(DIK_UP)) { worldMat.trans.y += 1.0f; }
			if (keys.keyPush(DIK_LEFT)) { worldMat.trans.x -= 1.0f; }
			else if (keys.keyPush(DIK_RIGHT)) { worldMat.trans.x += 1.0f; }

			worldMat.SetWorld();
		}
		
		directx.constMapTransform->mat = worldMat.matWorld * viewMat.matView * projectionMat.matProjection;

		directx.DrawUpdate();

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
		directx.GraphicsCommand(win,pipelineNum,primitiveNum);
		

		// 4.描画コマンドここまで //

		directx.DrawUpdate2();

		//毎フレーム処理　ここまで//

		if (keys.keyPush(DIK_ESCAPE)) break;
	}

	//ウィンドウクラスを登録解除
	win.UnregisterClassA();

	return 0;
}


