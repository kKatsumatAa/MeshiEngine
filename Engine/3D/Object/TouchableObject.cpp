#include "TouchableObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"



std::unique_ptr<TouchableObject> TouchableObject::Create(
	std::unique_ptr<WorldMat>worldMat, IModel * model)
{
	// オブジェクトのインスタンスを生成
	std::unique_ptr<TouchableObject> instance = std::make_unique<TouchableObject>();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize(std::move(worldMat),model)) {
		instance.reset();
		assert(0);
	}

	return instance;
}

bool TouchableObject::Initialize(std::unique_ptr<WorldMat>worldMat, IModel* model)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}

	SetModel(model);

	// コライダーの追加
	std::unique_ptr<MeshCollider> collider = std::make_unique<MeshCollider>();
	collider->ConstructTriangles(model);
	SetCollider(std::move(collider));

	return true;
}
