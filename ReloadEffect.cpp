#include "ReloadEffect.h"

void ReloadEffect::Initialize(Vec3 startPos, Vec3 endPos, int time, float width)
{
	this->worldTransform.trans = startPos;
	this->startPos = startPos;
	this->endPos = endPos;
	this->lifeTime = time;

	this->worldTransform.scale = { width,0.7f,1.0f };
}

void ReloadEffect::Update()
{
	deathTimer_++;

	worldTransform.trans = LerpVec3(startPos, endPos, EaseOut((float)deathTimer_ / (float)lifeTime));
	worldTransform.SetWorld();

	if (deathTimer_ >= lifeTime)
	{
		isDead = true;
	}
}

void ReloadEffect::Draw(ViewMat& view, ProjectionMat& projection)
{
	float color = LerpVec3({1.0f,0,0}, {0,0,0}, EaseOut((float)deathTimer_ / (float)lifeTime)).x;

	object.DrawBox(&worldTransform, &view, &projection, { color,color,color,color });
}




//-------------------------------------------------------------------------------------
ReloadEffectManager::~ReloadEffectManager()
{
	reloadEffects_.clear();
}

ReloadEffectManager::ReloadEffectManager()
{
}


void ReloadEffectManager::Initialize()
{
	reloadEffects_.clear();
}

void ReloadEffectManager::GenerateReloadEffect(Vec3 startPos, Vec3 endPos, int time, float width)
{
	//生成、初期化
	std::unique_ptr<ReloadEffect> reloadEffect = std::make_unique<ReloadEffect>();
	reloadEffect->Initialize(startPos, endPos, time, width);
	//アイテムを登録
	reloadEffects_.push_back(std::move(reloadEffect));
}

void ReloadEffectManager::Update()
{
	for (std::unique_ptr<ReloadEffect>& reloadEffect : reloadEffects_)
	{
		reloadEffect->Update();
	}

	reloadEffects_.remove_if([](std::unique_ptr<ReloadEffect>& reloadEffect)
		{
			return reloadEffect->GetIsDead();
		}
	);
}

void ReloadEffectManager::Draw(ViewMat& view, ProjectionMat& projection)
{
	for (std::unique_ptr<ReloadEffect>& reloadEffect : reloadEffects_)
	{
		reloadEffect->Draw(view, projection);
	}
}

ReloadEffectManager& ReloadEffectManager::GetInstance()
{
	static ReloadEffectManager inst; // private なコンストラクタを呼び出す。
	return inst;
}
