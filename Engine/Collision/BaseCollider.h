#pragma once

#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "IObject3D.h"

/// <summary>
/// コライダー基底クラス（オブジェクトが持つコライダー）
/// </summary>
class BaseCollider
{
public:
	friend class CollisionManager;

public:
	BaseCollider() = default;
	virtual ~BaseCollider() = default;

	inline void SetObject(IObject* object) {
		object_ = object;
	}

	inline IObject3D* GetObject3d() {
		return dynamic_cast<IObject3D*>(object_);
	}

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	inline const CollisionShapeType& GetShapeType() { return shapeType_; }

	//判定を有効にするかどうかのフラグ
	inline void SetIsValid(bool isValid) { isValid_ = isValid; }
	inline bool GetIsValid() { return isValid_; }

	//2dかどうか（スプライト用）
	inline void SetIs2D(bool is2D) { is2D_ = is2D; }
	inline bool GetIs2D() { return is2D_; }

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	inline void OnCollision(const CollisionInfo& info) {
		object_->OnCollision(info);
	}

	//判定属性セット
	inline void SetAttribute(uint16_t attribute) { attribute_ = attribute; }
	//当たり判定の属性を増やす
	inline void AttAttribute(uint16_t attribute) { attribute_ |= attribute; }
	//
	inline void RemoveAttribute(uint16_t attribute) { attribute_ &= attribute; }
	//
	inline uint16_t GetAttribute() { return attribute_; }


protected:
	//持ち主のObjectのポインタを入れる
	IObject* object_ = nullptr;
	// 形状タイプ
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
	//判定取るか取らないか
	bool isValid_ = true;
	//スプライト用の二次元かどうかフラグ
	bool is2D_ = false;
	//当たり判定属性
	uint16_t attribute_ = 0b111111111111111;
};

