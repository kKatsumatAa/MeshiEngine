#include "KeyboardInput.h"
#include "WindowsApp.h"
#include "DirectX.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
#include "WorldMat.h"
#include "Draw.h"


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
	WorldMat worldMat2;
	worldMat2.scale = { 1.0f, 0.5f, 1.0f };
	worldMat2.rot = { 15.f, 30.0f, 0.0f };
	worldMat2.trans = { -50.f, 0.0f, 0.0f };
	worldMat2.SetWorld();

	Directx directx(win);
	Draw draw(win, directx);
	Draw draw2(win, directx);
	draw.LoadGraph(L"Resources/texture.jpg");
	draw2.LoadGraph(L"Resources/texture2.jpg");

	XMFLOAT3 pos[4] = {
		{-50.0f,-50.0f,0.0f},//左下
		{0.0f,50.0f, 0.0f},//左上
		{50.0f,-50.0f, 0.0f},//右下
		{50.0f,50.0f,  0.0f}//右上
	};

	int pipelineNum = 0;
	bool primitiveNum = false;

	//キーボード入力初期化
	keys.Initialize(directx.result, win.hwnd, win.w);
	

	 //描画初期化処理-------------
	
	//directx.DrawInitialize(win);

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
		
		draw.constMapTransform->mat = worldMat.matWorld * viewMat.matView * projectionMat.matProjection;
		draw2.constMapTransform->mat = worldMat2.matWorld * viewMat.matView * projectionMat.matProjection;

		directx.DrawUpdate();

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
		//directx.GraphicsCommand(win,pipelineNum,primitiveNum);
		draw.DrawBox(pos[0], pos[1], pos[2], pos[3]);
		draw2.DrawTriangle(pos[0], pos[1], pos[2]);
		

		// 4.描画コマンドここまで //

		directx.DrawUpdate2();

		//毎フレーム処理　ここまで//

		if (keys.keyPush(DIK_ESCAPE)) break;
	}

	//ウィンドウクラスを登録解除
	win.UnregisterClassA();

	return 0;
}


