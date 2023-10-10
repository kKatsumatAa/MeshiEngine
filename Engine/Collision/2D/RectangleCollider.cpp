#include "RectangleCollider.h"

void RectangleCollider::Update()
{
	// 繝ｯ繝ｼ繝ｫ繝芽｡悟・縺九ｉ蠎ｧ讓吶ｒ謚ｽ蜃ｺ
	const M4& matWorld = object_->GetMatWorld();

	// 遏ｩ蠖｢縺ｮ繝｡繝ｳ繝仙､画焚繧呈峩譁ｰ
	Vec2 center = {
		(float)matWorld.m_[3][0],
	(float)matWorld.m_[3][1],
	};

	//蝗櫁ｻ｢逕ｨ縺ｮ陦悟・
	WorldMat rotWM;
	rotWM.rot_.z = object_->GetRot().z;
	rotWM.CalcRotMat();
	//繝吶け繝医Ν繧貞屓霆｢縺輔○縺ｦ鬆らせ縺ｨ縺吶ｋ
	Vec2 scale = { object_->GetScale().x,object_->GetScale().y };

	Vec3 leftUp = GetVec3xM4({ -scale.x,-scale.y,0 }, rotWM.GetRotMat(), 0);
	Vec3 leftDown = GetVec3xM4({ -scale.x,scale.y,0 }, rotWM.GetRotMat(), 0);
	Vec3 rightUp = GetVec3xM4({ scale.x,-scale.y,0 }, rotWM.GetRotMat(), 0);
	Vec3 rightDown = GetVec3xM4({ scale.x,scale.y,0 }, rotWM.GetRotMat(), 0);

	//荳ｭ蠢・ｺｧ讓吶↓雜ｳ縺励※
	MyRectangle::iLeftUp = center + Vec2(leftUp.x, leftUp.y);
	MyRectangle::iLeftDown = center + Vec2(leftDown.x, leftDown.y);
	MyRectangle::iRightUp = center + Vec2(rightUp.x, rightUp.y);
	MyRectangle::iRightDown = center + Vec2(rightDown.x, rightDown.y);
};


