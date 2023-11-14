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

	//ディゾルブの画像
	SetisDissolve(true);
	SetDissolveT(0);
	SetDissolveTexHandle(TextureManager::GetInstance().LoadGraph("dissolveMask.png"));

	return true;
}

void LandShape::Update()
{
	Object::Update();

	//デバッグ用で
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_T))
	{
		Vec3 interPos = { 0,0,0 };

		BeginWave(interPos, { GetScale().y / 45.0f,GetScale().GetLength() * 1.5f }, GetScale().GetLength(), 120.0f);
	}
}

void LandShape::DrawShadow()
{
	Object::DrawModel(nullptr, nullptr, nullptr, Object::PipelineStateNumObj::SHADOW_OBJ);
}

void LandShape::Draw()
{
	Object::DrawModel(nullptr, nullptr, nullptr, Object::PipelineStateNumObj::HULL_DOMAIN_OBJ);
}

void LandShape::OnCollision(const CollisionInfo& info)
{
	info;
}
