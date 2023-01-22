#pragma once

#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "Object.h"

/// <summary>
/// コライダー基底クラス（オブジェクトが持つコライダー）
/// </summary>
class BaseCollider
{
public:
	BaseCollider() = default;
	virtual ~BaseCollider() = default;

	inline void SetObject(Object* object) {
		this->object = object;
	}

	inline Object* GetObject() {
		return object;
	}

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	inline CollisionShapeType GetShapeType() { return shapeType; }

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	inline void OnCollision(const CollisionInfo& info) {
		object->OnCollision(info);
	}

protected:
	//持ち主のObjectのポインタを入れる
	Object* object = nullptr;
	// 形状タイプ
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
};

