#include "KeyboardInput.h"
#include "WindowsApp.h"
#include "DirectX.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
#include "WorldMat.h"
#include "Draw.h"
#include <random>


//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");

	MSG msg{};	//メッセージ

	//初期化処理　ここから//

	ViewMat viewMat;
	ProjectionMat projectionMat;
	WorldMat worldMats[7];

	//大元
	worldMats[0].trans = { 0,0,0 };
	worldMats[0].SetWorld();
	//腰
	worldMats[1].trans = { 0,-10,0 };
	worldMats[1].parent = &worldMats[0];
	worldMats[1].SetWorld();
	//頭
	worldMats[2].trans = { 0,10,0 };
	worldMats[2].parent= &worldMats[0];
	worldMats[2].SetWorld();
	//右手
	worldMats[3].trans = { -10,-10,0 };
	worldMats[3].parent = &worldMats[2];
	worldMats[3].SetWorld();
	//左手
	worldMats[4].trans = { 10,-10,0 };
	worldMats[4].parent = &worldMats[2];
	worldMats[4].SetWorld();
	//右足
	worldMats[5].trans = { -10,-10,0 };
	worldMats[5].parent = &worldMats[1];
	worldMats[5].SetWorld();
	//左足
	worldMats[6].trans = { 10,-10,0 };
	worldMats[6].parent = &worldMats[1];
	worldMats[6].SetWorld();

	int bodyNum = 0;

	//Directx directx;
	Draw draw[7];
	/*Draw draw2;
	Draw draw3;*/
	UINT64 textureHandle[3] = {0};
	
	LoadGraph(L"Resources/texture.jpg", textureHandle[0]);
	LoadGraph(L"Resources/texture2.jpg", textureHandle[1]);
	LoadGraph(L"Resources/back.png", textureHandle[2]);//back

	
	//draw3.LoadGraph(L"Resources/texture2.jpg");

	XMFLOAT3 pos[4] = {
		{-5.0f,-5.0f,0.0f},//左下
		{-5.0f,5.0f, 0.0f},//左上
		{5.0f,-5.0f, 0.0f},//右下
		{5.0f, 5.0f,  0.0f}//右上
	};
	
	XMFLOAT3 pos2[4] = {
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
		if (KeyboardInput::GetInstance().keyTrigger(DIK_SPACE)) bodyNum++;
		if (bodyNum >= _countof(draw))bodyNum = 0;

		if (KeyboardInput::GetInstance().keyPush(DIK_DOWN) || KeyboardInput::GetInstance().keyPush(DIK_UP) || KeyboardInput::GetInstance().keyPush(DIK_LEFT) || KeyboardInput::GetInstance().keyPush(DIK_RIGHT))//rotlasion
		{
			if (KeyboardInput::GetInstance().keyPush(DIK_DOWN)) { worldMats[bodyNum].rot.x -= 1.0f; }
			 if (KeyboardInput::GetInstance().keyPush(DIK_UP)) { worldMats[bodyNum].rot.x += 1.0f; }
			if (KeyboardInput::GetInstance().keyPush(DIK_LEFT)) { worldMats[bodyNum].rot.y += 1.0f; }
			 if (KeyboardInput::GetInstance().keyPush(DIK_RIGHT)) { worldMats[bodyNum].rot.y -= 1.0f; }
		}
		/*worldMat.rot.z++;*/
		for (WorldMat& i : worldMats)
		{
			i.SetWorld();
		}

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
		for (int i = 0; i < _countof(draw); i++)
		{
			draw[i].DrawCube3D(&worldMats[i], &viewMat, &projectionMat, textureHandle[primitiveNum + 1]);
			//draw[1].DrawCube3D(&worldMat2, &viewMat, &projectionMat, textureHandle[primitiveNum + 1]);
			//draw[2].DrawBox(pos[0], pos[1], pos[2], pos[3], &worldMat3, &viewMat, &projectionMat, textureHandle[1]);
		}

		// 4.描画コマンドここまで //

		Directx::GetInstance().DrawUpdate2();

		//毎フレーム処理　ここまで//

		if (KeyboardInput::GetInstance().keyPush(DIK_ESCAPE)) break;
	}

	//ウィンドウクラスを登録解除
	 WindowsApp::GetInstance().UnregisterClassA();

	return 0;
}


