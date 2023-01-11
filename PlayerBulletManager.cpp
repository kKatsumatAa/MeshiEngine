#include "PlayerBulletManager.h"
#include "ParticleManager.h"

static std::uniform_real_distribution<float> vecDist(0.99f, 2.0f);
static std::uniform_real_distribution<float> scaleDist(3.0f, 3.7f);


void PlayerBulletManager::Initialize(Model* model, CartridgeEffectManager* cartridgeEffectManager, Camera* camera)
{
	assert(model);

	if (texhandle[0] == NULL)
	{
		TextureManager::LoadGraph(L"Resources/image/amo.png", texhandle[0]);
	}
	if (texhandle[1] == NULL)
	{
		TextureManager::LoadGraph(L"Resources/image/amoBox.png", texhandle[1]);
	}

	this->cartridgeEffectM = cartridgeEffectManager;
	this->model = model;
	this->camera = camera;
	playerBullets_.clear();

	bulletType = ITEM_TYPE::INORMAL;
	bulletNum = bulletNumMax;

	isChange = false;

	shotEffectCount = 0;

	ChangeState(new BulletNormal);
}

void PlayerBulletManager::ChangeState(PlayerBulletState* state)
{
	delete this->state;
	this->state = state;
	state->SetPlayerBulletManager(this);
}

void PlayerBulletManager::SetBulletNum(int num)
{
	bulletNum = num;

	if (bulletNum < 0)
	{
		bulletNum = 0;
	}
}

void PlayerBulletManager::SetBulletType(const int bulletType)
{
	this->bulletType = bulletType;
	isChange = true;
}

void PlayerBulletManager::GeneratePlayerBullet(const Vec3& position, const Vec3& velocity)
{
	//球を生成、初期化
	std::unique_ptr<PlayerBullet> playerBullet = std::make_unique<PlayerBullet>();
	playerBullet->Initialize(position, velocity, model);
	//球を登録
	playerBullets_.push_back(std::move(playerBullet));
}

void PlayerBulletManager::Update()
{
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_)
	{
		bullet->Update();
	}

	playerBullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet)
		{
			return bullet->GetIsDead();
		}
	);

	if (isChange)
	{
		if (bulletType == ITEM_TYPE::INORMAL) { ChangeState(new BulletNormal); }
		else if (bulletType == ITEM_TYPE::ISHOTGUN) { ChangeState(new BulletShotGun); }
		else if (bulletType == ITEM_TYPE::ILASER) { ChangeState(new BulletLayser); }

		isChange = false;
	}
}

void PlayerBulletManager::Draw(ViewMat& view, ProjectionMat& projection)
{
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_)
	{
		bullet->Draw(view, projection);
	}

	//撃った時のエフェクトを描画
	if (shotEffectCount > 0)
	{
		shotEffectCount--;

		shotEffectObj.worldMat->trans = shotEffectPos;
		float scale = scaleDist(engine) * (float)shotEffectCount / (float)shotEffectCountMax;
		shotEffectObj.worldMat->scale = { scale ,scale ,scale };
		shotEffectObj.worldMat->SetWorld();

		shotEffectObj.DrawSphere(shotEffectObj.worldMat, &view, &projection, { 1.0f,0.8f,0.8f,0.9f });
	}
}

void PlayerBulletManager::DrawSprite()
{
	gauge.DrawBoxSprite({ 1100 - 1,100 - 1,0 }, 1.0f, { 1.0f,1.0f,1.0f,1.0f }, texhandle[1]);

	for (int i = 0; i < bulletNum; i++)
	{
		amoGauge[i].DrawBoxSprite({ 1100, (float)(100 + 64 * i),0 }, 1.0f, { 1.0f,1.0f,1.0f,1.0f }, texhandle[0]);
	}
}

void PlayerBulletManager::ShakeCamera()
{
	camera->CameraShake(shakeTime, shakeLength);
}

void PlayerBulletManager::ShotEffect(Vec3 pos)
{
	shotEffectPos = pos;
	shotEffectCount = shotEffectCountMax;
}

void PlayerBulletManager::Shot(Vec3 pos, std::function<void()> p)
{
	state->Shot(pos, p);
}


//-------------------------------------------------------------------------------------------------------
void PlayerBulletState::SetPlayerBulletManager(PlayerBulletManager* playerBulletM)
{
	this->playerBulletM = playerBulletM;
}

void BulletNormal::Shot(Vec3 pos, std::function<void()> p)
{
	coolTime--;

	if (coolTime <= 0 && KeyboardInput::GetInstance().KeyPush(DIK_SPACE) && playerBulletM->GetBulletNum() > 0)
	{
		playerBulletM->SetBulletNum(playerBulletM->GetBulletNum() - 1);
		playerBulletM->GeneratePlayerBullet(pos, { 0,-1.0f,0 });

		coolTime = coolTimeTmp;

		//重力を0にする
		p();

		//薬莢エフェクト
		playerBulletM->cartridgeEffectM->GenerateCartridgeEffect(pos);

		//shake
		playerBulletM->ShakeCamera();

		//
		playerBulletM->ShotEffect({ pos.x,pos.y - 3.0f,pos.z });

		//音
		Sound::GetInstance().PlayWave("shot.wav");
	}
}

void BulletLayser::Shot(Vec3 pos, std::function<void()> p)
{
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) && playerBulletM->GetBulletNum() > 0)
	{
		isShot = true;
		count = 0;


		playerBulletM->SetBulletNum(0);
	}
	if (isShot)
	{
		time--;

		if (time <= 0)
		{
			//shake
			playerBulletM->ShakeCamera();

			playerBulletM->GeneratePlayerBullet(pos, { 0,-1.0f,0 });

			//薬莢エフェクト
			playerBulletM->cartridgeEffectM->GenerateCartridgeEffect(pos);
			//重力を0にする
			p();

			time = timerTmp;
			count++;

			//
			playerBulletM->ShotEffect({ pos.x,pos.y - 3.0f,pos.z });

			//音
			Sound::GetInstance().PlayWave("shot.wav");

			if (count >= countTmp)
			{
				isShot = false;
			}
		}
	}
}

void BulletShotGun::Shot(Vec3 pos, std::function<void()> p)
{
	coolTime--;

	if (coolTime <= 0 && KeyboardInput::GetInstance().KeyPush(DIK_SPACE) && playerBulletM->GetBulletNum() > 0)
	{
		for (int i = 0; i < num; i++)
		{
			//float radian = pi / (float)(num + 1) * (i + 1);
			float radian = pi / 3.0f + pi / 3.0f / (float)(num + 1) * (i + 1);

			Vec3 vec = { cosf(radian), -sinf(radian),0 };
			vec *= vecDist(engine);

			playerBulletM->SetBulletNum(playerBulletM->GetBulletNum() - 1);
			playerBulletM->GeneratePlayerBullet(pos, vec);

			//薬莢エフェクト
			playerBulletM->cartridgeEffectM->GenerateCartridgeEffect(pos);

			//
			playerBulletM->ShotEffect({ pos.x,pos.y - 3.0f,pos.z });

			//音
			Sound::GetInstance().PlayWave("shot.wav");
		}

		coolTime = coolTimeTmp;

		//重力を0にする
		p();

		//shake
		playerBulletM->ShakeCamera();
	}
}
