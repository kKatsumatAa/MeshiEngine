
#include "Enemy.h"
#include "CollisionManager.h"
#include"EnemyManager.h"


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

	LoadGraph(L"Resources/white.png", textureHandle[0]);//ただの白（色変える用）
	LoadGraph(L"Resources/texture.jpg", textureHandle[1]);
	LoadGraph(L"Resources/texture2.jpg", textureHandle[2]);
	LoadGraph(L"Resources/back.png", textureHandle[3]);//黄色春日
	LoadGraph(L"Resources/scope.png", textureHandle[4]);//ロックオン
	LoadGraph(L"Resources/back.jpg", textureHandle[5]);//背景
	LoadGraph(L"Resources/ui.png", textureHandle[6]);//背景
	


	//キーボード入力初期化
	KeyboardInput::GetInstance();

	//音データ
	SoundData soundData[10];
	soundData[0] = SoundLoadWave("Resources/SE.wav", true);
	soundData[1] = SoundLoadWave("Resources/SE2.wav", true);
	soundData[2] = SoundLoadWave("Resources/test2.wav", false);
	soundData[3] = SoundLoadWave("Resources/SE3.wav", false);

	SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[2], 5.0f, true);

	//背景
	Draw backGround;
	backGround.worldMat->scale = { 180.0f,100.f,0 };
	backGround.worldMat->trans = { 0,0,1000 };
	backGround.worldMat->SetWorld();

	//UI
	Draw UI;

	//BulletManager
	BulletManager bulletManager;

	//player
	Player* player = new Player(soundData);

	//enemy
	EnemyManager enemyManager;
	enemyManager.Initialize(player, &bulletManager,soundData);
	enemyManager.LoadEnemyPopData();

	//衝突
	std::unique_ptr<CollisionManager> colliderManager = std::make_unique<CollisionManager>(&viewMat, &projectionMat);

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

		Directx::GetInstance().DrawUpdate();

//更新処理
		backGround.worldMat->scale += {0.03f, 0.03f, 0};
		backGround.worldMat->SetWorld();

		player->Update();
		enemyManager.Update();

		bulletManager.UpdateEnemyBullet();

		

		{//colliderManager

			colliderManager->ClearList();
			colliderManager->SetListCollider(player);
			const std::list<std::unique_ptr<Enemy>>& enemies = enemyManager.GetEnemies();
			//bulletはそれ自体がlistなので特別
			const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player->GetBullets();
			const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = bulletManager.GetEnemyBullets();

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

			colliderManager->CheckAllCollisions();


			//rayの当たり判定(敵とプレイヤーのみ)
			{
				colliderManager->ClearList();
				colliderManager->SetListCollider(player);
				const std::list<std::unique_ptr<Enemy>>& enemies = enemyManager.GetEnemies();

				int lockOnNum = 0;
				for (const std::unique_ptr<Enemy>& enemy : enemies)
				{
					colliderManager->SetListCollider(enemy.get());
					lockOnNum += enemy.get()->isLockOned;//ロックオンされてる数をカウント
				}
				player->isLockNum = lockOnNum;//代入

				colliderManager->CheckAllCollisions2();
			}
		}

		//if (enemyManager.GetEnemies().size() == 0&& angle >= 4000)
		//{
		//	SoundStopWave(soundData2);
		//}

// 4.描画コマンドここから　//-----------
		//back
		backGround.DrawBox(backGround.worldMat, &viewMat, &projectionMat, { 1.0f, 1.0f, 1.0f, 1.0f }, textureHandle[5]);//
		//Enemy
		bulletManager.DrawEnemyBullet(viewMat, projectionMat, textureHandle);
		enemyManager.Draw(viewMat, projectionMat, textureHandle);
		//player
		player->Draw(viewMat, projectionMat, textureHandle);//playerを後にしないと透過されない！
		//ui
		//UI.DrawBoxSprite({ 180,200,0 }, 0.5f, { 1.0f,1.0f,1.0f,1.0f }, textureHandle[6], {0.5f,0.5f},angle);

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


