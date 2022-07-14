﻿#include "KeyboardInput.h"
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
	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲
	std::uniform_real_distribution<float> posDist(-50.0f, 50.0f);
	std::uniform_real_distribution<float> rotDist(0.0f, pi * 2);

	ViewMat viewMat;
	ProjectionMat projectionMat;
	WorldMat worldMats[30];
	WorldMat worldMat;
	worldMat.SetWorld();

	//worldMat.SetWorld();
	WorldMat cameraWorldMat;
	for (WorldMat& i : worldMats)
	{
		//i.scale = { 1.0f, 1.0f, 1.0f };
		i.rot = { RaditoAngle(rotDist(engine)), RaditoAngle(rotDist(engine)), RaditoAngle(rotDist(engine)) };
		i.trans = { posDist(engine),posDist(engine),posDist(engine) };
		i.SetWorld();
	}

	//Directx directx;
	Draw draw[30];
	Draw drawL[24];
	/*Draw draw2;
	Draw draw3;*/
	UINT64 textureHandle[3] = { 0 };

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

	XMFLOAT3 posTri[3] = {
		{-5.0f,-5.0f,0.0f},//左下
		{0.0f,5.0f, 0.0f},//上
		{5.0f,-5.0f, 0.0f},//右下
	};

	XMFLOAT3 posLine[2] = {
		{-5.0f,0,0.0f},//左下
		{5.0f,0.0f, 0.0f}//左上
	};

	int pipelineNum = 0;
	bool primitiveNum = false;
	float alpha = 1;
	bool isTrans = 0;
	int primitive = 0;

	XMFLOAT4 color = { 0,1,0,1 };
	XMFLOAT4 color3 = { 0,0,1,1 };
	XMFLOAT4 color2 = { NULL,NULL,NULL,NULL };

	//キーボード入力初期化
	KeyboardInput::GetInstance();


	//描画初期化処理-------------

   //directx.DrawInitialize(win);

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


		if (KeyboardInput::GetInstance().keyPush(DIK_D) || KeyboardInput::GetInstance().keyPush(DIK_A) ||
			KeyboardInput::GetInstance().keyPush(DIK_W) || KeyboardInput::GetInstance().keyPush(DIK_S))//カメラ
		{
			cameraWorldMat.rot.y += AngletoRadi((KeyboardInput::GetInstance().keyPush(DIK_A) - KeyboardInput::GetInstance().keyPush(DIK_D)));
			cameraWorldMat.rot.x += AngletoRadi((KeyboardInput::GetInstance().keyPush(DIK_W) - KeyboardInput::GetInstance().keyPush(DIK_S)));

			cameraWorldMat.SetWorld();

			Vec3 cameraEyeVec = { 0,0,-100.0f };

			Vec3xM4(cameraEyeVec, cameraWorldMat.matWorld, false);//ベクトルをカメラ用のworldの角度に対応

			viewMat.eye = cameraEyeVec;
		}


		//if (KeyboardInput::GetInstance().keyPush(DIK_S) || KeyboardInput::GetInstance().keyPush(DIK_W) || KeyboardInput::GetInstance().keyPush(DIK_A) || KeyboardInput::GetInstance().keyPush(DIK_D))//translasion
		//{
		//	if (KeyboardInput::GetInstance().keyPush(DIK_S)) { worldMat.trans.y -= 1.0f; }
		//	else if (KeyboardInput::GetInstance().keyPush(DIK_W)) { worldMat.trans.y += 1.0f; }
		//	if (KeyboardInput::GetInstance().keyPush(DIK_A)) { worldMat.trans.x -= 1.0f; }
		//	else if (KeyboardInput::GetInstance().keyPush(DIK_D)) { worldMat.trans.x += 1.0f; }
		//}

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
		if (KeyboardInput::GetInstance().keyTrigger(DIK_1))//テクスチャ
		{
			if (primitiveNum) primitiveNum = 0;
			else              primitiveNum = 1;
		}
		if (KeyboardInput::GetInstance().keyTrigger(DIK_2))//ワイヤーフレームか
		{
			if (pipelineNum)  pipelineNum = 0;
			else              pipelineNum = 1;
		}
		if (KeyboardInput::GetInstance().keyTrigger(DIK_3))//アルファ値変える
		{
			if (alpha == 1)  alpha = 0.5f;
			else           alpha = 1;
		}
		if (KeyboardInput::GetInstance().keyTrigger(DIK_4))//図形変える
		{
			primitive++;
			if (primitive >= 5)primitive = 0;
		}
		if (KeyboardInput::GetInstance().keyTrigger(DIK_SPACE))//図形の移動か回転かを変える
		{
			if (isTrans == true) isTrans = false;
			else                 isTrans = true;
		}

		{//グリッド線
			static float r = 150;
			for (int x = 0; x < _countof(drawL) / 2; x++)//縦線
			{
				drawL[x].DrawLine({ -r / 2.0f + r / (_countof(drawL) / 2 - 1) * (float)x, 0, -r / 2.0f },
					{ -r / 2.0f + r / (_countof(drawL) / 2 - 1) * (float)x, 0, r / 2.0f }, &worldMat, &viewMat, &projectionMat, color);
			}
			for (int z = _countof(drawL) / 2; z < _countof(drawL); z++)//横線
			{
				drawL[z].DrawLine({ -r / 2.0f, 0, -r / 2.0f + r / (_countof(drawL) / 2 - 1) * (float)(z - _countof(drawL) / 2) },
					{ r / 2.0f, 0, -r / 2.0f + r / (_countof(drawL) / 2 - 1) * (float)(z - _countof(drawL) / 2) },
					&worldMat, &viewMat, &projectionMat, color3);
			}
		}
		//directx.GraphicsCommand(win,pipelineNum,primitiveNum);
		//draw2.DrawBoxSprite(pos2[0], pos2[1], pos2[2], pos2[3], textureHandle[2]);//背景

		for (int i = 0; i < _countof(draw); i++)
		{
			if (KeyboardInput::GetInstance().keyPush(DIK_DOWN) || KeyboardInput::GetInstance().keyPush(DIK_UP) ||//図形を操作（アフィン） 
				KeyboardInput::GetInstance().keyPush(DIK_LEFT) || KeyboardInput::GetInstance().keyPush(DIK_RIGHT))
			{
				if (KeyboardInput::GetInstance().keyPush(DIK_DOWN))
				{
					if (!isTrans)worldMats[i].rot.x -= AngletoRadi(1.0f);
					else worldMats[i].trans.y -= 1.0f;
				}
				if (KeyboardInput::GetInstance().keyPush(DIK_UP))
				{
					if (!isTrans)worldMats[i].rot.x += AngletoRadi(1.0f);
					else worldMats[i].trans.y += 1.0f;
				}
				if (KeyboardInput::GetInstance().keyPush(DIK_LEFT))
				{
					if (!isTrans)worldMats[i].rot.y += AngletoRadi(1.0f);
					else worldMats[i].trans.x -= 1.0f;
				}
				if (KeyboardInput::GetInstance().keyPush(DIK_RIGHT))
				{
					if (!isTrans)worldMats[i].rot.y -= AngletoRadi(1.0f);
					else worldMats[i].trans.x += 1.0f;
				}
			}


			//数字で操作系
			draw[i].isWireFrame = pipelineNum;
			draw[i].color2.w = alpha;
			color.w = alpha;
			if (primitive == 0)
			{
				draw[i].DrawTriangle(posTri[0], posTri[1], posTri[2], &worldMats[i], &viewMat, &projectionMat, color2, textureHandle[primitiveNum + 1]);
			}
			else if (primitive == 1)
			{
				draw[i].DrawBox(pos[0], pos[1], pos[2], pos[3], &worldMats[i], &viewMat, &projectionMat, color2, textureHandle[primitiveNum + 1]);
			}
			else if (primitive == 2)
			{
				draw[i].DrawCube3D(&worldMats[i], &viewMat, &projectionMat, color2, textureHandle[primitiveNum + 1]);
			}
			else if (primitive == 3)
			{
				draw[i].DrawCircle(5.0f, &worldMats[i], &viewMat, &projectionMat, color);
			}
			else
			{
				draw[i].DrawLine(posLine[0], posLine[1], &worldMats[i], &viewMat, &projectionMat, color);
			}
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


