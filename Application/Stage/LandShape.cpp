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

	//分割数
	SetTessFactor(TESS_FACTOR_MAX_);

	SetColor({ 1.0f,1.0f,1.0f,1.0f });

	return true;
}

void LandShape::Update()
{
	Object::Update();

	//メッシュ波
	waves_.Update(GameVelocityManager::GetInstance().GetVelocity());
}

void LandShape::DrawImGui(std::function<void()> imguiF)
{
	IObject3D::DrawImGui();
}

void LandShape::DrawShadow()
{
	Object::DrawModel(nullptr, nullptr, nullptr, Object::PipelineStateNumObj::SHADOW_HULL_DOMAIN_OBJ);
}

void LandShape::Draw()
{
	Object::DrawModel(nullptr, nullptr, nullptr, Object::PipelineStateNumObj::HULL_DOMAIN_OBJ);
}
