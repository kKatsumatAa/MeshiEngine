#include "BreakEffectManager.h"
static std::uniform_real_distribution<float> velocityRand(-0.5f, 0.5f);
static std::uniform_int_distribution<int> leftRand(0, 1);


void BreakEffect::Initialize(Vec3 pos, Vec3 velocity, float gravity)
{
	this->velocity = velocity;

	this->worldTransform.trans = pos;
	this->worldTransform.scale = { 1.0f,1.0f,1.0f };
	this->gravity = gravity;
}

void BreakEffect::Update()
{
	//移動
	velocity += Vec3(0, -gravity, 0);

	worldTransform.trans += velocity;

	worldTransform.SetWorld();

	//自然消滅までのタイマー
	deathTimer_--;
	if (deathTimer_ <= 0)
	{
		isDead = true;
	}
}

void BreakEffect::Draw(ViewMat& view, ProjectionMat& projection, UINT64 texHandle)
{
	object.DrawBox(&worldTransform, &view, &projection, { 0.2f,0.2f,0.2f,0.7f }, texHandle);
}



//-----------------------------------------------------------------
void BreakEffectManager::Initialize()
{
	breakEffects_.clear();
}

void BreakEffectManager::GenerateBreakEffect(Vec3 pos)
{
	//生成、初期化
	std::unique_ptr<BreakEffect> breakEffect = std::make_unique<BreakEffect>();
	breakEffect->Initialize(pos, { velocityRand(engine),velocityRand(engine),velocityRand(engine) },
		0.01f);
	//アイテムを登録
	breakEffects_.push_back(std::move(breakEffect));

	if (texHandle[0] == NULL) { TextureManager::GetInstance().LoadGraph(L"Resources/image/block.png", texHandle[0]); }
}

void BreakEffectManager::Update()
{
	for (std::unique_ptr<BreakEffect>& BreakEffect : breakEffects_)
	{
		BreakEffect->Update();
	}

	breakEffects_.remove_if([](std::unique_ptr<BreakEffect>& BreakEffect)
		{
			return BreakEffect->GetIsDead();
		}
	);
}

void BreakEffectManager::Draw(ViewMat& view, ProjectionMat& projection)
{
	for (std::unique_ptr<BreakEffect>& BreakEffect : breakEffects_)
	{
		BreakEffect->Draw(view, projection,texHandle[0]);
	}
}

void BreakEffectManager::GenerateRandomEffect(Vec3 pos, int num)
{
	for (int i = 0; i < num; i++)
	{
		GenerateBreakEffect(pos);
	}
}
