#include "CartridgeEffectManager.h"
static std::uniform_real_distribution<float> velocityRand(-0.5f, 0.5f);
static std::uniform_int_distribution<int> leftRand(0, 1);


void CartridgeEffect::Initialize(Vec3 pos, float power, float gravity, bool isLeft)
{
	if (isLeft)
	{
		velocity = { -fabsf(velocityRand(engine)),fabsf(velocityRand(engine)),0 };
	}
	else
	{
		velocity = { fabsf(velocityRand(engine)),fabsf(velocityRand(engine)),0 };
	}
	velocity.Normalized();
	velocity *= power;

	this->worldTransform.trans = pos;
	this->worldTransform.scale = { 0.3f,0.8f,1.0f };
	this->gravity = gravity;
	this->isLeft = isLeft;
}

void CartridgeEffect::Update()
{
	//移動
	velocity += Vec3(0, -gravity, 0);

	worldTransform.trans += (velocity);
	if (isLeft)
	{
		worldTransform.rot.z += 0.1f;
	}
	else
	{
		worldTransform.rot.z -= 0.1f;
	}
	worldTransform.SetWorld();

	//自然消滅までのタイマー
	deathTimer_--;
	if (deathTimer_ <= 0)
	{
		isDead = true;
	}
}

void CartridgeEffect::Draw(ViewMat& view, ProjectionMat& projection)
{
	object.DrawBox(&worldTransform, &view, &projection);
}



//-----------------------------------------------------------------
void CartridgeEffectManager::Initialize()
{
	cartridgeEffects_.clear();
}

void CartridgeEffectManager::GenerateCartridgeEffect(Vec3 pos)
{
	//生成、初期化
	std::unique_ptr<CartridgeEffect> cartridgeEffect = std::make_unique<CartridgeEffect>();
	cartridgeEffect->Initialize(pos, fabsf(velocityRand(engine)), 0.01f, leftRand(engine));
	//アイテムを登録
	cartridgeEffects_.push_back(std::move(cartridgeEffect));
}

void CartridgeEffectManager::Update()
{
	for (std::unique_ptr<CartridgeEffect>& cartridgeEffect : cartridgeEffects_)
	{
		cartridgeEffect->Update();
	}

	cartridgeEffects_.remove_if([](std::unique_ptr<CartridgeEffect>& cartridgeEffect)
		{
			return cartridgeEffect->GetIsDead();
		}
	);
}

void CartridgeEffectManager::Draw(ViewMat& view, ProjectionMat& projection)
{
	for (std::unique_ptr<CartridgeEffect>& cartridgeEffect : cartridgeEffects_)
	{
		cartridgeEffect->Draw(view, projection);
	}
}
