﻿
#include "Enemy.h"
#include "CollisionManager.h"
#include"EnemyManager.h"
#include"Background.h"


//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//コンソールへの文字入力
	OutputDebugStringA("Hello,DirectX!!\n");
	
	//
	WindowsApp::GetInstance();
	Directx::GetInstance();

	DrawInitialize();
	
	WorldMat cameraWorldMat;

	MSG msg{};	//メッセージ
	
	ViewMat viewMat;
	ProjectionMat projectionMat;

	//画像用ハンドル
	UINT64 textureHandle[30] = {0};
	{
		LoadGraph(L"Resources/white.png", textureHandle[0]);//ただの白（色変える用）
		LoadGraph(L"Resources/texture.jpg", textureHandle[1]);
		LoadGraph(L"Resources/texture2.jpg", textureHandle[2]);
		LoadGraph(L"Resources/back.png", textureHandle[3]);//黄色春日
		LoadGraph(L"Resources/lock.png", textureHandle[4]);//ロックオン
		LoadGraph(L"Resources/back.jpg", textureHandle[5]);//背景
		LoadGraph(L"Resources/lockOn.png", textureHandle[6]);//敵のロックオン
		LoadGraph(L"Resources/Title.png", textureHandle[7]);//タイトル
		LoadGraph(L"Resources/HP.png", textureHandle[8]);//hp
	}
	//数字の画像
	UINT64 textureNumHundle[12];
	{
		LoadGraph(L"Resources/numbers/0.png", textureNumHundle[0]);
		LoadGraph(L"Resources/numbers/1_.png", textureNumHundle[1]);
		LoadGraph(L"Resources/numbers/2.png", textureNumHundle[2]);
		LoadGraph(L"Resources/numbers/3.png", textureNumHundle[3]);
		LoadGraph(L"Resources/numbers/4.png", textureNumHundle[4]);
		LoadGraph(L"Resources/numbers/5.png", textureNumHundle[5]);
		LoadGraph(L"Resources/numbers/6.png", textureNumHundle[6]);
		LoadGraph(L"Resources/numbers/7.png", textureNumHundle[7]);
		LoadGraph(L"Resources/numbers/8.png", textureNumHundle[8]);
		LoadGraph(L"Resources/numbers/9.png", textureNumHundle[9]);
		LoadGraph(L"Resources/numbers/10.png", textureNumHundle[10]);
	}

	//キーボード入力初期化
	KeyboardInput::GetInstance();

	//音データ
	SoundData soundData[10];
	{
		soundData[0] = SoundLoadWave("Resources/SE.wav", true);
		soundData[1] = SoundLoadWave("Resources/SE2.wav", true);
		soundData[2] = SoundLoadWave("Resources/BGM2.wav", false);
		soundData[3] = SoundLoadWave("Resources/Death.wav", false);
		soundData[4] = SoundLoadWave("Resources/Damage.wav", false);
		soundData[5] = SoundLoadWave("Resources/SE3.wav", false);
		soundData[6] = SoundLoadWave("Resources/BGM.wav", false);
		soundData[7] = SoundLoadWave("Resources/heal.wav", false);
	}
	SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[2], 0.5f, true);

	//UI
	Draw UI;

	//パーティクル
	ParticleManager pManager;

	//BulletManager
	BulletManager bulletManager;

	//player
	Player* player = new Player(soundData);

	//item
	ItemManager iManager;
	iManager.Initialize(player, soundData, &pManager, &viewMat, &projectionMat);

	//enemy
	EnemyManager enemyManager;
	enemyManager.Initialize(player, &bulletManager,soundData, &pManager, &iManager);
	//enemyManager.LoadEnemyPopData();

	//衝突
	std::unique_ptr<CollisionManager> colliderManager = std::make_unique<CollisionManager>(&viewMat, &projectionMat);

	//背景
	Background back;

	int bossNum = 0;




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

		Directx::GetInstance().DrawUpdate({ 0.0f,0.0f,0.0f,0.0f });

//更新処理
		

		player->Update();
		enemyManager.Update();

		bulletManager.UpdateEnemyBullet();

		back.Update();

		int oldBossNum = bossNum;
		bossNum = 0;
		
		//colliderManager
		{

			colliderManager->ClearList();
			colliderManager->SetListCollider(player);
			const std::list<std::unique_ptr<Enemy>>& enemies = enemyManager.GetEnemies();
			//bulletはそれ自体がlistなので特別
			const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player->GetBullets();
			const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = bulletManager.GetEnemyBullets();
			const std::list<std::unique_ptr<Item>>& items = iManager.GetItems();

			for (const std::unique_ptr<Enemy>& enemy : enemies)
			{
				colliderManager->SetListCollider(enemy.get());
			}
			for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets)
			{
				colliderManager->SetListCollider(bullet.get());
			}
			for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets)
			{
				colliderManager->SetListCollider(bullet.get());
			}
			for (const std::unique_ptr<Item>& item : items)
			{
				colliderManager->SetListCollider(item.get());
			}

			colliderManager->CheckAllCollisions();


			//rayの当たり判定(敵とプレイヤーのみ)
			{
				colliderManager->ClearList();
				colliderManager->SetListCollider(player);
				const std::list<std::unique_ptr<Enemy>>& enemies = enemyManager.GetEnemies();
				const std::list<std::unique_ptr<Item>>& items = iManager.GetItems();

				int lockOnNum = 0;
				

				for (const std::unique_ptr<Enemy>& enemy : enemies)
				{
					colliderManager->SetListCollider(enemy.get());
					//playerがロックオンモードじゃなければロックオン状態を解除
					if (player->GetPlayerStatus() != TARGET && enemy->isLockOned && !enemy->isLockOnDead || player->isDead)
					{
						enemy->LockOnedReset();
					}
					lockOnNum += enemy.get()->isLockOned;//ロックオンされてる数をカウント
					bossNum += enemy.get()->isBoss;
				}
				//item用
				for (const std::unique_ptr<Item>& item : items)
				{
					colliderManager->SetListCollider(item.get());
					//playerがロックオンモードじゃなければロックオン状態を解除
					if (player->GetPlayerStatus() != TARGET && item->isLockOned && !item->isLockOnDead || player->isDead)
					{
						item->LockOnedReset();
					}
					lockOnNum += item.get()->isLockOned;//ロックオンされてる数をカウント
				}
				player->isLockNum = lockOnNum;//代入

				if (player->isLockNum > 10)
				{
					player->isLockNum = 10;
				}
				if (player->isLockNum < 10)//ロックオンは最大10まで
					colliderManager->CheckAllCollisions2();
			}
		}

		//ボス戦の時にbgm変える
		if (bossNum && !oldBossNum)
		{
			SoundStopWave(soundData[2]);
			SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[6], 0.5f, true);
		}
		else if (!bossNum && oldBossNum)
		{
			SoundStopWave(soundData[6]);
			SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[2], 0.5f, true);
		}

		back.phase = enemyManager.phase;

		pManager.Update();
		iManager.Update();

// 4.描画コマンドここから　//-----------
		//背景
		back.Draw(viewMat, projectionMat, textureHandle, bossNum);
		//Enemy
		bulletManager.DrawEnemyBullet(viewMat, projectionMat, textureHandle);
		enemyManager.Draw(viewMat, projectionMat, textureHandle);
		
		pManager.Draw(viewMat, projectionMat, textureHandle[0]);
		
		iManager.Draw(viewMat, projectionMat, textureHandle);

		//player
		player->Draw(viewMat, projectionMat, textureHandle, textureNumHundle);//playerを後にしないと透過されない！

// 4.描画コマンドここまで //

		Directx::GetInstance().DrawUpdate2();

		//毎フレーム処理　ここまで//

		if (KeyboardInput::GetInstance().keyPush(DIK_ESCAPE)) break;
	}

	//音データ解放
	Directx::GetInstance().xAudio2.Reset();
	SoundUnLoad(&soundData[0]);
	SoundUnLoad(&soundData[1]);
	SoundUnLoad(&soundData[2]);
	SoundUnLoad(&soundData[3]);
	delete player;

	//ウィンドウクラスを登録解除
	WindowsApp::GetInstance().UnregisterClassA();

	return 0;
}


