#include "Scene.h"

void SceneState::SetScene(Scene* scene)
{
	//state�ł͂Ȃ�Scene�N���X�̃C���X�^���X
	this->scene = scene;
}



//---------------------------------------------------------------------------------------
//�^�C�g��
void SceneTitle::Initialize()
{
	scene->bossNum = 0;

	scene->pManager.Initialize();
	scene->bulletManager.Initialize(&scene->pManager);
	scene->iManager.Initialize(scene->player, scene->soundData, &scene->pManager, &scene->viewMat, &scene->projectionMat);
	scene->player->Initialize(scene->soundData);
	scene->enemyManager.Initialize(scene->player, &scene->bulletManager, scene->soundData, &scene->pManager, &scene->iManager);
	scene->colliderManager->Initialize();
	scene->back.Initialize();
	
	SoundStopWave(scene->soundData[2]);
	SoundStopWave(scene->soundData[6]);
	SoundPlayWave(Directx::GetInstance().xAudio2.Get(), scene->soundData[2], 0.5f, true);
	count = 0;
	pos = { WindowsApp::GetInstance().window_width / 2.0f,WindowsApp::GetInstance().window_height / 2.0f + sinf(count) * 10.0f - 150.0f,0 };
	
	infoNum = 0;
	infoGauge = 0;
	infoGauge2 = 0;
	info.worldMat->scale = { 0,0,0 };

	scene->enemyManager.StartGenerate();
}

void SceneTitle::Update(SoundData* soundData)
{
	//�����̕\��
	if (info.worldMat->scale.x < 1.0f)
	{
		info.worldMat->scale += {0.2f, 0.2f, 0.2f};
		info.worldMat->SetWorld();
	}

	switch (infoNum)
	{
	case 0:
		if (KeyboardInput::GetInstance().keyPush(DIK_UPARROW) || KeyboardInput::GetInstance().keyPush(DIK_DOWNARROW) ||
			KeyboardInput::GetInstance().keyPush(DIK_LEFTARROW) || KeyboardInput::GetInstance().keyPush(DIK_RIGHTARROW))
		{
			infoGauge += 1.0f;
		}
		break;
	case 1:
		if (KeyboardInput::GetInstance().keyTrigger(DIK_X))
		{
			infoGauge += 50.0f;
		}
		break;
	case 2:
		if (KeyboardInput::GetInstance().keyPush(DIK_Z))
		{
			infoGauge += 0.5f;
		}
		break;
	}

	if (infoGauge2 >= 100.0f)
	{
		infoNum++;
		info.worldMat->scale = { 0,0,0 };
		info.worldMat->SetWorld();
		infoGauge = 0;
		infoGauge2 = 0;

		if (infoNum > 2) infoNum = 0;
	}

	//���������f
	if (infoGauge2 < infoGauge)
	{
		infoGauge2 += 1.0f;
	}
	

	int startNum = 0;

	count += 0.02f;
	pos = { WindowsApp::GetInstance().window_width / 2.0f,WindowsApp::GetInstance().window_height / 2.0f + sinf(count) * 10.0f - 150.0f,0 };

	scene->enemyManager.Update();
	scene->player->Update();
	scene->pManager.Update();

	//colliderManager
	{

		scene->colliderManager->ClearList();
		scene->colliderManager->SetListCollider(scene->player);
		const std::list<std::unique_ptr<Enemy>>& enemies = scene->enemyManager.GetEnemies();
		//bullet�͂��ꎩ�̂�list�Ȃ̂œ���
		const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = scene->player->GetBullets();
		const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = scene->bulletManager.GetEnemyBullets();
		const std::list<std::unique_ptr<Item>>& items = scene->iManager.GetItems();

		for (const std::unique_ptr<Enemy>& enemy : enemies)
		{
			scene->colliderManager->SetListCollider(enemy.get());
		}
		for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets)
		{
			scene->colliderManager->SetListCollider(bullet.get());
		}
		for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets)
		{
			scene->colliderManager->SetListCollider(bullet.get());
		}
		for (const std::unique_ptr<Item>& item : items)
		{
			scene->colliderManager->SetListCollider(item.get());
		}

		scene->colliderManager->CheckAllCollisions();


		//ray�̓����蔻��(�G�ƃv���C���[�̂�)
		{
			scene->colliderManager->ClearList();
			scene->colliderManager->SetListCollider(scene->player);
			const std::list<std::unique_ptr<Enemy>>& enemies = scene->enemyManager.GetEnemies();
			const std::list<std::unique_ptr<Item>>& items = scene->iManager.GetItems();

			int lockOnNum = 0;


			for (const std::unique_ptr<Enemy>& enemy : enemies)
			{
				scene->colliderManager->SetListCollider(enemy.get());
				//player�����b�N�I�����[�h����Ȃ���΃��b�N�I����Ԃ�����
				if (scene->player->GetPlayerStatus() != TARGET && enemy->isLockOned && !enemy->isLockOnDead || scene->player->isDead)
				{
					enemy->LockOnedReset();
				}
				lockOnNum += enemy.get()->isLockOned;//���b�N�I������Ă鐔���J�E���g
				scene->bossNum += enemy.get()->isBoss;

				//start�̎c����J�E���g
				startNum += enemy->isSTART;
			}
			//item�p
			for (const std::unique_ptr<Item>& item : items)
			{
				scene->colliderManager->SetListCollider(item.get());
				//player�����b�N�I�����[�h����Ȃ���΃��b�N�I����Ԃ�����
				if (scene->player->GetPlayerStatus() != TARGET && item->isLockOned && !item->isLockOnDead || scene->player->isDead)
				{
					item->LockOnedReset();
				}
				lockOnNum += item.get()->isLockOned;//���b�N�I������Ă鐔���J�E���g
			}
			scene->player->isLockNum = lockOnNum;//���

			if (scene->player->isLockNum > 10)
			{
				scene->player->isLockNum = 10;
			}
			if (scene->player->isLockNum < 10)//���b�N�I���͍ő�10�܂�
				scene->colliderManager->CheckAllCollisions2();
		}
	}


	scene->back.Update(scene->bossNum);

	//start�S�������āA�p�[�e�B�N������������J��
	if (startNum <= 0 && scene->pManager.particles.size() <= 0)
	{
		scene->ChangeState(new SceneGame);
		SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[4], 1.0f, false);
	}
}

void SceneTitle::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{
	scene->enemyManager.Draw(scene->viewMat, scene->projectionMat, textureHandle);

	scene->pManager.Draw(scene->viewMat, scene->projectionMat, textureHandle[0]);
	//player
	scene->player->Draw(scene->viewMat, scene->projectionMat, textureHandle, textureNumHundle);//player����ɂ��Ȃ��Ɠ��߂���Ȃ��I

	//�^�C�g��
	title.DrawBoxSprite(pos, 0.8f, { 1.0f,1.0f,1.0f,1.0f }, textureHandle[7], { 0.5f,0.5f });
	//����
	switch (infoNum)
	{
	case 0:
		info.DrawBoxSprite({ 1100.0f,550.0f - sinf(count) * 4.0f,0.0f }, 0.6f, { 1.0f,1.0f,1.0f,1.0f }, textureHandle[10], { 0.5f,0.5f });
		break;
	case 1:
		info.DrawBoxSprite({ 1100.0f,550.0f - sinf(count) * 4.0f,0.0f }, 0.6f, { 1.0f,1.0f,1.0f,1.0f }, textureHandle[11], { 0.5f,0.5f });
		break;
	case 2:
		info.DrawBoxSprite({ 1100.0f,550.0f - sinf(count) * 4.0f,0.0f }, 0.6f, { 1.0f,1.0f,1.0f,1.0f }, textureHandle[12], { 0.5f,0.5f });
		break;
	}
	//�Q�[�W
	gauge.DrawClippingBoxSprite({ 950.0f,430.0f - sinf(count) * 4.0f,0.0f }, 0.1f, { 0,0 },
		{ 0.13f * (infoGauge2), 0.6f }, { 1.0f,1.0f,0,1.0f }, 0.0f, textureHandle[0]);
}



//---------------------------------------------------------------------------------------
//�Q�[��
void SceneGame::Initialize()
{
	scene->bossNum = 0;

	isHit[0] = { 0 };
	isHit[1] = { 0 };

	gameOverTimer = 0;

	SoundStopWave(scene->soundData[6]);

	scene->pManager.Initialize();
	scene->bulletManager.Initialize(&scene->pManager);
	scene->iManager.Initialize(scene->player, scene->soundData, &scene->pManager, &scene->viewMat, &scene->projectionMat);
	scene->player->Initialize(scene->soundData);
	scene->enemyManager.Initialize(scene->player, &scene->bulletManager, scene->soundData, &scene->pManager, &scene->iManager);
	scene->enemyManager.LoadEnemyPopData();
	scene->colliderManager->Initialize();
}

void SceneGame::Update(SoundData* soundData)
{
	isHit[1] = isHit[0];

	//�q�b�g�X�g�b�v���ĂȂ��Ƃ�
	if (!isHit[0])
	{
		scene->player->Update();
		scene->enemyManager.Update();

		scene->bulletManager.UpdateEnemyBullet();

		int oldBossNum = scene->bossNum;
		scene->bossNum = 0;

		//colliderManager
		{

			scene->colliderManager->ClearList();
			scene->colliderManager->SetListCollider(scene->player);
			const std::list<std::unique_ptr<Enemy>>& enemies = scene->enemyManager.GetEnemies();
			//bullet�͂��ꎩ�̂�list�Ȃ̂œ���
			const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = scene->player->GetBullets();
			const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = scene->bulletManager.GetEnemyBullets();
			const std::list<std::unique_ptr<Item>>& items = scene->iManager.GetItems();

			for (const std::unique_ptr<Enemy>& enemy : enemies)
			{
				scene->colliderManager->SetListCollider(enemy.get());
			}
			for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets)
			{
				scene->colliderManager->SetListCollider(bullet.get());
			}
			for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets)
			{
				scene->colliderManager->SetListCollider(bullet.get());
			}
			for (const std::unique_ptr<Item>& item : items)
			{
				scene->colliderManager->SetListCollider(item.get());
			}

			scene->colliderManager->CheckAllCollisions();


			//ray�̓����蔻��(�G�ƃv���C���[�̂�)
			{
				scene->colliderManager->ClearList();
				scene->colliderManager->SetListCollider(scene->player);
				const std::list<std::unique_ptr<Enemy>>& enemies = scene->enemyManager.GetEnemies();
				const std::list<std::unique_ptr<Item>>& items = scene->iManager.GetItems();

				int lockOnNum = 0;


				for (const std::unique_ptr<Enemy>& enemy : enemies)
				{
					scene->colliderManager->SetListCollider(enemy.get());
					//player�����b�N�I�����[�h����Ȃ���΃��b�N�I����Ԃ�����
					if (scene->player->GetPlayerStatus() != TARGET && enemy->isLockOned && !enemy->isLockOnDead || scene->player->isDead)
					{
						enemy->LockOnedReset();
					}
					lockOnNum += enemy.get()->isLockOned;//���b�N�I������Ă鐔���J�E���g
					scene->bossNum += enemy.get()->isBoss;
				}
				//item�p
				for (const std::unique_ptr<Item>& item : items)
				{
					scene->colliderManager->SetListCollider(item.get());
					//player�����b�N�I�����[�h����Ȃ���΃��b�N�I����Ԃ�����
					if (scene->player->GetPlayerStatus() != TARGET && item->isLockOned && !item->isLockOnDead || scene->player->isDead)
					{
						item->LockOnedReset();
					}
					lockOnNum += item.get()->isLockOned;//���b�N�I������Ă鐔���J�E���g
				}
				scene->player->isLockNum = lockOnNum;//���

				if (scene->player->isLockNum > 10)
				{
					scene->player->isLockNum = 10;
				}
				if (scene->player->isLockNum < 10)//���b�N�I���͍ő�10�܂�
					scene->colliderManager->CheckAllCollisions2();
			}
		}

		//�{�X��̎���bgm�ς���
		if (scene->bossNum && !oldBossNum)
		{
			SoundStopWave(soundData[2]);
			SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[6], 0.5f, true);
		}
		else if (!scene->bossNum && oldBossNum)
		{
			SoundStopWave(soundData[6]);
			SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[2], 0.5f, true);
		}

		scene->back.phase = scene->enemyManager.phase;

		scene->pManager.Update();
		scene->iManager.Update();

		scene->back.Update(scene->bossNum);
	}
	isHit[0] = scene->enemyManager.isBossDead;
	if(isHit[0])
	{
		if (!isHit[1])
		{
			scene->hitStopTimer = scene->hitStoptmp;
		}
		scene->hitStopTimer--;

		if (scene->hitStopTimer <= 0)
		{
			isHit[0] = false;
			scene->enemyManager.isBossDead = false;
		}
	}

	//�V�[���J��
	if (scene->enemyManager.isEnd[1]) scene->ChangeState(new SceneEnd);

	if (scene->player->isDead) gameOverTimer++;
	if (scene->player->isDead && gameOverTimer >= 150) scene->ChangeState(new SceneTitle);

	
}

void SceneGame::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{
	//Enemy
	scene->bulletManager.DrawEnemyBullet(scene->viewMat, scene->projectionMat, textureHandle);
	scene->enemyManager.Draw(scene->viewMat, scene->projectionMat, textureHandle);

	scene->pManager.Draw(scene->viewMat, scene->projectionMat, textureHandle[0]);

	scene->iManager.Draw(scene->viewMat, scene->projectionMat, textureHandle);

	//player
	scene->player->Draw(scene->viewMat, scene->projectionMat, textureHandle, textureNumHundle);//player����ɂ��Ȃ��Ɠ��߂���Ȃ��I
}



//---------------------------------------------------------------------------------------
//�I�����
void SceneEnd::Initialize()
{
	timer = 0;

	count = 0;

	scene->bossNum = 0;

	scene->pManager.Initialize();
	scene->bulletManager.Initialize(&scene->pManager);
	scene->iManager.Initialize(scene->player, scene->soundData, &scene->pManager, &scene->viewMat, &scene->projectionMat);
	scene->player->Initialize(scene->soundData);
	scene->enemyManager.Initialize(scene->player, &scene->bulletManager, scene->soundData, &scene->pManager, &scene->iManager);
	scene->colliderManager->Initialize();

	SoundStopWave(scene->soundData[6]);
}

void SceneEnd::Update(SoundData* soundData)
{
	timer++;

	scene->back.Update(scene->bossNum);
	if (timer >= 260) scene->ChangeState(new SceneTitle);
}

void SceneEnd::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{
	count += 0.02f;

	title.DrawBoxSprite({ pos.x ,pos.y + sinf(count) * 10.0f,pos.z}, 0.8f, { 1.0f,1.0f,1.0f,1.0f }, textureHandle[9], { 0.5f,0.5f });
}



//---------------------------------------------------------------------------------------
//��ɍs������
Scene::~Scene()
{
	delete player;
	delete state;
}

void Scene::ChangeState(SceneState* state)
{
	delete this->state;
	this->state = state;
	state->SetScene(this);
	state->Initialize();
}

void Scene::Initialize(SoundData* soundData)
{
	this->soundData = soundData;

	pManager.Initialize();
	bulletManager.Initialize(&pManager);
	iManager.Initialize(player, soundData, &pManager, &viewMat, &projectionMat);
	player->Initialize(soundData);
	enemyManager.Initialize(player, &bulletManager, soundData, &pManager, &iManager);
	colliderManager->Initialize();
	back.Initialize();

	ChangeState(new SceneTitle);
	state->SetScene(this);

	//SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[2], 0.5f, true);
}


void Scene::Update(SoundData* soundData)
{
	state->Update(soundData);

#ifdef _DEBUG
	if (KeyboardInput::GetInstance().keyTrigger(DIK_E)) ChangeState(new SceneTitle);
#endif 
	
}

void Scene::Draw(UINT64* textureHandle, UINT64* textureNumHundle)
{
	//�w�i
	back.Draw(viewMat, projectionMat, textureHandle, bossNum);
	state->Draw(textureHandle,textureNumHundle);
}

