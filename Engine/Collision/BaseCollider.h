#pragma once
#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "IObject3D.h"

/// <summary>
/// 繧ｳ繝ｩ繧､繝繝ｼ蝓ｺ蠎輔け繝ｩ繧ｹ・医が繝悶ず繧ｧ繧ｯ繝医′謖√▽繧ｳ繝ｩ繧､繝繝ｼ・・
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
	/// 譖ｴ譁ｰ
	/// </summary>
	virtual void Update() = 0;

	inline const CollisionShapeType& GetShapeType() { return shapeType_; }

	//蛻､螳壹ｒ譛牙柑縺ｫ縺吶ｋ縺九←縺・°縺ｮ繝輔Λ繧ｰ
	inline void SetIsValid(bool isValid) { isValid_ = isValid; }
	inline bool GetIsValid() { return isValid_; }

	//2d縺九←縺・°・医せ繝励Λ繧､繝育畑・・
	inline void SetIs2D(bool is2D) { is2D_ = is2D; }
	inline bool GetIs2D() { return is2D_; }

	/// <summary>
	/// 陦晉ｪ∵凾繧ｳ繝ｼ繝ｫ繝舌ャ繧ｯ髢｢謨ｰ
	/// </summary>
	/// <param name="info">陦晉ｪ∵ュ蝣ｱ</param>
	inline void OnCollision(const CollisionInfo& info) {
		object_->OnCollision(info);
	}

	//蛻､螳壼ｱ樊ｧ繧ｻ繝・ヨ
	inline void SetAttribute(uint16_t attribute) { attribute_ = attribute; }
	//蠖薙◆繧雁愛螳壹・螻樊ｧ繧貞｢励ｄ縺・
	inline void AttAttribute(uint16_t attribute) { attribute_ |= attribute; }
	//
	inline void RemoveAttribute(uint16_t attribute) { attribute_ &= attribute; }
	//
	inline uint16_t GetAttribute() { return attribute_; }


protected:
	//謖√■荳ｻ縺ｮObject縺ｮ繝昴う繝ｳ繧ｿ繧貞・繧後ｋ
	IObject* object_ = nullptr;
	// 蠖｢迥ｶ繧ｿ繧､繝・
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
	//蛻､螳壼叙繧九°蜿悶ｉ縺ｪ縺・°
	bool isValid_ = true;
	//繧ｹ繝励Λ繧､繝育畑縺ｮ莠梧ｬ｡蜈・°縺ｩ縺・°繝輔Λ繧ｰ
	bool is2D_ = false;
	//蠖薙◆繧雁愛螳壼ｱ樊ｧ
	uint16_t attribute_ = 0b111111111111111;
};

