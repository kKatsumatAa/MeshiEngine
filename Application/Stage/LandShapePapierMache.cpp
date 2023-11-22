#include "LandShapePapierMache.h"
#include "GameVelocityManager.h"


std::unique_ptr<LandShapePapierMache> LandShapePapierMache::Create(WorldMat* worldMat, IModel* model)
{
	// オブジェクトのインスタンスを生成
	std::unique_ptr<LandShapePapierMache> instance = std::make_unique<LandShapePapierMache>();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize(worldMat, model)) {
		instance.reset();
		assert(0);
	}

	return instance;
}

bool LandShapePapierMache::Initialize(WorldMat* worldMat, IModel* model)
{
	//ただの張りぼてなのでパラメータだけコピー
	SetWorldMatParam(*worldMat);

	SetModel(model);

	//地形オブジェクト共通処理
	//ディゾルブの画像
	SetisDissolve(true);
	SetDissolveT(0);
	SetDissolveTexHandle(TextureManager::GetInstance().LoadGraph("dissolveMask.png"));

	model->SetMaterialExtend({ 0.45f, 0.4f, 5.0f });

	return true;
}

void LandShapePapierMache::Draw(float vel)
{
	//見た目だけで判定などはない
	SetColor({ 1.0f,1.0f,1.0f,Lerp(-0.3f,1.0f,EaseIn(vel)) });
	if (GetColor().w >= 0.0f)
	{
		Object::DrawModel(nullptr, nullptr, nullptr, Object::PipelineStateNumObj::NORMAL_OBJ);
	}
}
