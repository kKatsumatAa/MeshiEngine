#include "LandShapePapierMache.h"
#include "GameVelocityManager.h"

using namespace Util;


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
		assert(false);
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
	SetDissolveRatio(0);
	SetDissolveTexHandle(TextureManager::GetInstance().LoadGraph("dissolveMask.dds"));

	model->SetMaterialExtend(MODEL_MATERIAL_EXTEND_);

	return true;
}

void LandShapePapierMache::Draw(float vel)
{
	//見た目だけで判定などはない
	SetColor({ COLOR_RGB_.x,COLOR_RGB_.y,COLOR_RGB_.z, Lerp(COLOR_A_MIN_,COLOR_A_MAX_,EaseIn(vel)) });
	//軽量化するために不透明度が一定以上の時
	if (GetColor().w >= 0.0f)
	{
		Object::DrawModel(nullptr, nullptr, nullptr, Object::PipelineStateNumObj::NORMAL_OBJ);
	}
}
