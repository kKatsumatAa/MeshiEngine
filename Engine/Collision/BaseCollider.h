/*
* @file BaseCollider.h
* @brief コライダーの基底クラス
*/

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


public:
	//コンストラクタ
	BaseCollider() = default;
	//デストラクタ
	virtual ~BaseCollider() = default;

public:
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

public:
	//オブジェクトのポインタセット
	inline void SetObject(IObject* object) {
		object_ = object;
	}
	//セットされたオブジェクトのポインタ取得
	inline IObject3D* GetObject3d() {
		return dynamic_cast<IObject3D*>(object_);
	}

	//形状タイプを取得
	inline const CollisionShapeType& GetShapeType() { return shapeType_; }

	//判定を有効にするかどうかのフラグセット
	inline void SetIsValid(bool isValid) { isValid_ = isValid; }
	//判定を有効にするかどうかのフラグ取得
	inline bool GetIsValid() { return isValid_; }

	//2dかどうか（スプライト用）セット
	inline void SetIs2D(bool is2D) { is2D_ = is2D; }
	//2dかどうか（スプライト用）取得
	inline bool GetIs2D() { return is2D_; }

	//判定属性セット
	inline void SetAttribute(uint16_t attribute) { attribute_ = attribute; }
	//当たり判定の属性を増やす
	inline void AttAttribute(uint16_t attribute) { attribute_ |= attribute; }
	//引数以外の判定属性を取り除く
	inline void RemoveAttribute(uint16_t attribute) { attribute_ &= attribute; }
	//判定属性取得
	inline uint16_t GetAttribute() { return attribute_; }

public:
	/// <summary>
/// 衝突時コールバック関数
/// </summary>
/// <param name="info">衝突情報</param>
	inline void OnCollision(const CollisionInfo& info) {
		object_->OnCollision(info);
	}

};

