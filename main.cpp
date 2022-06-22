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

	ViewMat viewMat;
	ProjectionMat projectionMat;
	WorldMat worldMat;
	worldMat.scale = { 1.0f, 1.0f, 1.0f };
	//worldMat.rot = { 15.f, 30.0f, 0.0f };
	worldMat.trans = { 0.f, 0.0f, 0.0f };
	worldMat.SetWorld();
	WorldMat worldMat2;
	worldMat2.scale = { 1.0f, 1.0f, 1.0f };
	worldMat2.rot = { 15.f, 30.0f, 0.0f };
	worldMat2.trans = { -10.f, 0.0f, 0.0f };
	worldMat2.SetWorld();

	//Directx directx;
	Draw draw[10];
	Draw draw2;
	Draw draw3;
	UINT64 textureHandle[3] = {0};
	
	LoadGraph(L"Resources/texture.jpg", textureHandle[0]);
	LoadGraph(L"Resources/texture2.jpg", textureHandle[1]);
	LoadGraph(L"Resources/back.png", textureHandle[2]);//back

	
	//draw3.LoadGraph(L"Resources/texture2.jpg");

	Vec3 pos[4] = {
		{-30.0f,-30.0f,0.0f},//左下
		{-30.0f,30.0f, 0.0f},//左上
		{30.0f,-30.0f, 0.0f},//右下
		{30.0f,30.0f,  0.0f}//右上
	};
	
	Vec3 pos2[4] = {
		{0, WindowsApp::GetInstance().window_height ,0.0f},//左下
		{0,0, 0.0f},//左上
		{ WindowsApp::GetInstance().window_width, WindowsApp::GetInstance().window_height, 0.0f},//右下
		{ WindowsApp::GetInstance().window_width,0.0f,  0.0f}//右上
	};

	int pipelineNum = 0;
	bool primitiveNum = false;

	//キーボード入力初期化
	KeyboardInput::GetInstance();


	//描画初期化処理-------------

   //directx.DrawInitialize(win);

   //初期化処理　ここまで//

   //ゲームループ
	while (true) {

		if ( WindowsApp::GetInstance().MessegeRoop(msg))
		{
			break;
		}

		//毎フレーム処理　ここから//
		//キーボード情報の取得開始
		KeyboardInput::GetInstance().Update();


		if (KeyboardInput::GetInstance().keyPush(DIK_D) || KeyboardInput::GetInstance().keyPush(DIK_A))//カメラ
		{
			if (KeyboardInput::GetInstance().keyPush(DIK_D)) { viewMat.angle += XMConvertToRadians(1.0f); }
			else if (KeyboardInput::GetInstance().keyPush(DIK_A)) { viewMat.angle -= XMConvertToRadians(1.0f); }

			//長さを決めてy軸周りを回す
			viewMat.eye.x = -100 * sinf(viewMat.angle);
			viewMat.eye.z = -100 * cosf(viewMat.angle);
		}
		

		//if (KeyboardInput::GetInstance().keyPush(DIK_S) || KeyboardInput::GetInstance().keyPush(DIK_W) || KeyboardInput::GetInstance().keyPush(DIK_A) || KeyboardInput::GetInstance().keyPush(DIK_D))//translasion
		//{
		//	if (KeyboardInput::GetInstance().keyPush(DIK_S)) { worldMat.trans.y -= 1.0f; }
		//	else if (KeyboardInput::GetInstance().keyPush(DIK_W)) { worldMat.trans.y += 1.0f; }
		//	if (KeyboardInput::GetInstance().keyPush(DIK_A)) { worldMat.trans.x -= 1.0f; }
		//	else if (KeyboardInput::GetInstance().keyPush(DIK_D)) { worldMat.trans.x += 1.0f; }
		//}
		if (KeyboardInput::GetInstance().keyPush(DIK_DOWN) || KeyboardInput::GetInstance().keyPush(DIK_UP) || KeyboardInput::GetInstance().keyPush(DIK_LEFT) || KeyboardInput::GetInstance().keyPush(DIK_RIGHT))//rotlasion
		{
			if (KeyboardInput::GetInstance().keyPush(DIK_DOWN)) { worldMat.rot.x -= 1.0f; }
			 if (KeyboardInput::GetInstance().keyPush(DIK_UP)) { worldMat.rot.x += 1.0f; }
			if (KeyboardInput::GetInstance().keyPush(DIK_LEFT)) { worldMat.rot.y += 1.0f; }
			 if (KeyboardInput::GetInstance().keyPush(DIK_RIGHT)) { worldMat.rot.y -= 1.0f; }
		}
		/*worldMat.rot.z++;*/
		worldMat.SetWorld();

		//Vec3 v = viewMat.eye - worldMat.trans;

		//viewMat.eye += v.GetNormalized();
		//viewMat.target = { worldMat.trans };
		viewMat.SetMat();

		//draw.constMapTransform->mat = worldMat.matWorld * viewMat.matView * projectionMat.matProjection;
		//draw3.constMapTransform->mat = worldMat2.matWorld * viewMat.matView * projectionMat.matProjection;

		Directx::GetInstance().DrawUpdate();

		// 4.描画コマンドここから　//-----------
		if (KeyboardInput::GetInstance().keyTrigger(DIK_1))
		{
			if (primitiveNum) primitiveNum = 0;
			else              primitiveNum = 1;
		}
		if (KeyboardInput::GetInstance().keyTrigger(DIK_2))
		{
			if (pipelineNum)  pipelineNum = 0;
			else              pipelineNum = 1;
		}
		//directx.GraphicsCommand(win,pipelineNum,primitiveNum);
		//draw2.DrawBoxSprite(pos2[0], pos2[1], pos2[2], pos2[3], textureHandle[2]);//背景

		draw[0].DrawCube3D(worldMat, viewMat, projectionMat, textureHandle[primitiveNum]);
		draw[1].DrawCube3D(worldMat2, viewMat, projectionMat, textureHandle[primitiveNum + 1]);
		//draw3.DrawBox(pos[0], pos[1], pos[2],pos[3], worldMat2, viewMat, projectionMat, textureHandle[1]);


		// 4.描画コマンドここまで //

		Directx::GetInstance().DrawUpdate2();

		//毎フレーム処理　ここまで//

		if (KeyboardInput::GetInstance().keyPush(DIK_ESCAPE)) break;
	}

	//ウィンドウクラスを登録解除
	 WindowsApp::GetInstance().UnregisterClassA();

	return 0;
}


