#include "PlayerBulletManager.h"

void PlayerBulletManager::Initialize(Model* model, CartridgeEffectManager* cartridgeEffectManager)
{
	assert(model);

	if (texhandle == NULL)
	{
		TextureManager::LoadGraph(L"Resources/image/white.png", texhandle);
	}

	this->cartridgeEffectM = cartridgeEffectManager;
	this->model = model;
	playerBullets_.clear();

	bulletType = ITEM_TYPE::INORMAL;
	bulletNum = bulletNumMax;

	isChange = false;

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
		if      (bulletType == ITEM_TYPE::INORMAL)  { ChangeState(new BulletNormal); }
		else if (bulletType == ITEM_TYPE::ISHOTGUN) { ChangeState(new BulletShotGun); }
		else if (bulletType == ITEM_TYPE::ILASER)   { ChangeState(new BulletLayser); }

		isChange = false;
	}
}

void PlayerBulletManager::Draw(ViewMat& view, ProjectionMat& projection)
{
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_)
	{
		bullet->Draw(view, projection);
	}
}

void PlayerBulletManager::DrawSprite()
{
	gauge[1].DrawClippingBoxSprite({ 1100,100,0 }, 1.0f, { 0,0 }, { 0.5f,2.0f }, { 0.3f,0.3f,0.3f,1.0f },
		texhandle);
	gauge[0].DrawClippingBoxSprite({ 1100,100,0 }, 1.0f, { 0,0 }, { 0.5f,(float)bulletNum / (float)bulletNumMax * 2.0f }, { 0.7f,0.7f,0.7f,0.9f },
		texhandle);
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
	}
}

void BulletLayser::Shot(Vec3 pos, std::function<void()> p)
{
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) && playerBulletM->GetBulletNum() > 0)
	{
		playerBulletM->SetBulletNum(playerBulletM->GetBulletNum() - 5);
		playerBulletM->GeneratePlayerBullet(pos, { 0,-1.0f,0 });

		//重力を0にする
		p();

		//薬莢エフェクト
		playerBulletM->cartridgeEffectM->GenerateCartridgeEffect(pos);
	}
}

void BulletShotGun::Shot(Vec3 pos, std::function<void()> p)
{
	coolTime--;

	if (coolTime <= 0 && KeyboardInput::GetInstance().KeyPush(DIK_SPACE) && playerBulletM->GetBulletNum() > 0)
	{
		for (int i = 0; i < num; i++)
		{
			float radian = pi / (float)(num + 1) * (i + 1);

			Vec3 vec = { cosf(radian), -sinf(radian),0 };

			playerBulletM->SetBulletNum(playerBulletM->GetBulletNum() - 1);
			playerBulletM->GeneratePlayerBullet(pos, vec);

			//薬莢エフェクト
			playerBulletM->cartridgeEffectM->GenerateCartridgeEffect(pos);
		}

		coolTime = coolTimeTmp;

		//重力を0にする
		p();


	}
}
