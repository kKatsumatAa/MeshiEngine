#include "LandShape.h"
#include "GameVelocityManager.h"


std::unique_ptr<LandShape> LandShape::Create(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	// オブジェクトのインスタンスを生成
	std::unique_ptr<LandShape> instance = std::make_unique<LandShape>();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize(std::move(worldMat), model)) {
		instance.reset();
		assert(0);
	}

	return instance;
}

bool LandShape::Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	if (!TouchableObject::Initialize(std::move(worldMat), model))
	{
		return false;
	}

	//地形オブジェクト共通処理
	//ディゾルブの画像
	SetisDissolve(true);
	SetDissolveT(0);
	SetDissolveTexHandle(TextureManager::GetInstance().LoadGraph("dissolveMask.png"));

	//張りぼて作成
	landShapePapierMache_ = LandShapePapierMache::Create(worldMat_.get(), model);

	return true;
}

void LandShape::Update()
{
	Object::Update();

	//張りぼて更新
	landShapePapierMache_->SetWorldMatParam(*worldMat_);
	landShapePapierMache_->Update();

	//
	float gameVel = GameVelocityManager::GetInstance().GetVelocity();
	float gameVelMin = GameVelocityManager::GetInstance().GAME_VELOCITY_MIN_;
	float t = timer_ / TIMER_MAX_;
	if (gameVel > GameVelocityManager::GetInstance().GAME_VELOCITY_MIN_)
	{
		timer_++;
	}
	else if (gameVel == gameVelMin && changeVel_ > gameVelMin)
	{
		timer_--;
	}
	timer_ = min(max(timer_, 0), TIMER_MAX_);

	changeVel_ = Lerp(gameVelMin, 1.0f, t);
	changeVel_ = min(max(changeVel_, 0), 1.0f);

	//分割数
	SetTessFactor(5.0f + (1.0f - changeVel_ + 0.01f) * TESS_FACTOR_MAX_);
}

void LandShape::DrawShadow()
{
	Object::DrawModel(nullptr, nullptr, nullptr, Object::PipelineStateNumObj::SHADOW_OBJ);
}

void LandShape::Draw()
{
	//張りぼて描画
	landShapePapierMache_->Draw(changeVel_);

	SetColor({ 1.0f,1.0f,1.0f,Lerp(1.0f,-0.1f,EaseInOut(changeVel_)) });
	if (GetColor().w >= 0.0f)
	{
		Object::DrawModel(nullptr, nullptr, nullptr, Object::PipelineStateNumObj::HULL_DOMAIN_OBJ);
	}
}
