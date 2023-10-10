#pragma once
#include "BaseCollider.h"

class IObject3D;

/// <summary>
/// 繝ｬ繧､繧ｭ繝｣繧ｹ繝医↓繧医ｋ諠・ｱ繧貞ｾ励ｋ轤ｺ縺ｮ讒矩菴難ｼ・ollisionInfo縺ｮ繧医≧縺ｪ繧ゅ・・・
/// </summary>
struct RaycastHit
{
	// 陦晉ｪ∫嶌謇九・繧ｪ繝悶ず繧ｧ繧ｯ繝・
	IObject3D* object = nullptr;
	// 陦晉ｪ∫嶌謇九・繧ｳ繝ｩ繧､繝繝ｼ
	BaseCollider* collider = nullptr;
	// 陦晉ｪ∫せ
	DirectX::XMVECTOR inter;
	// 陦晉ｪ∫せ縺ｾ縺ｧ縺ｮ霍晞屬
	float distance = 0.0f;
};

