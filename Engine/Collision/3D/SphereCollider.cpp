#include "SphereCollider.h"


using namespace DirectX;

void SphereCollider::Update()
{
	// 繝ｯ繝ｼ繝ｫ繝芽｡悟・縺九ｉ蠎ｧ讓吶ｒ謚ｽ蜃ｺ
	const M4& matWorld = object_->GetMatWorld();

	// 逅・・繝｡繝ｳ繝仙､画焚繧呈峩譁ｰ
	Sphere::center = {
		(float)matWorld.m_[3][0] + offset_.m128_f32[0],
	(float)matWorld.m_[3][1] + offset_.m128_f32[1],
	(float)matWorld.m_[3][2] + offset_.m128_f32[2],
	(float)matWorld.m_[3][3] + offset_.m128_f32[3],
	};
	//繝ｯ繝ｼ繝ｫ繝芽｡悟・縺ｮ繧ｹ繧ｱ繝ｼ繝ｫ縺九ｉ蜊雁ｾ・
	Sphere::iRadius =
		(object_->GetScale().x + object_->GetScale().y + object_->GetScale().z) / 3.0f;
}
