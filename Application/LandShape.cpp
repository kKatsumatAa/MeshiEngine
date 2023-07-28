#include "LandShape.h"
#include "GameVelocityManager.h"


std::unique_ptr<LandShape> LandShape::Create(std::unique_ptr<WorldMat> worldMat, Model* model)
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

bool LandShape::Initialize(std::unique_ptr<WorldMat> worldMat, Model* model)
{
	if (!TouchableObject::Initialize(std::move(worldMat), model))
	{
		return false;
	}

	//ディゾルブの画像
	SetisDissolve(true);
	SetDissolveT(0);
	SetDissolveTexHandle(TextureManager::GetInstance().LoadGraph(L"dissolveMask.png"));

	//ノーマルマップ
	SetIsNormalMap(true);
	SetNormalMapTexHandle(TextureManager::GetInstance().LoadGraph(L"landShapeNormal.jpg"));

	return true;
}

void LandShape::Update()
{
	float t = generateTimer_ / GENERATE_TIMER_MAX_;

	SetDissolveT(1.0f - EaseOut(t));

	generateTimer_ += 1.0f * GetRand(0.001f, 1.5f);
	generateTimer_ = min(generateTimer_, GENERATE_TIMER_MAX_);

	Object::Update();
}

void LandShape::Draw()
{
}
