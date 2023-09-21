#include "RectangleCollider.h"

void RectangleCollider::Update()
{
	// ワールド行列から座標を抽出
	const M4& matWorld = object_->GetMatWorld();

	// 矩形のメンバ変数を更新
	Vec2 center = {
		(float)matWorld.m_[3][0],
	(float)matWorld.m_[3][1],
	};

	//回転用の行列
	WorldMat rotWM;
	rotWM.rot_.z = object_->GetRot().z;
	rotWM.CalcRotMat();
	//ベクトルを回転させて頂点とする
	Vec2 scale = { object_->GetScale().x,object_->GetScale().y };

	Vec3 leftUp = GetVec3xM4({ -scale.x,-scale.y,0 }, rotWM.GetRotMat(), 0);
	Vec3 leftDown = GetVec3xM4({ -scale.x,scale.y,0 }, rotWM.GetRotMat(), 0);
	Vec3 rightUp = GetVec3xM4({ scale.x,-scale.y,0 }, rotWM.GetRotMat(), 0);
	Vec3 rightDown = GetVec3xM4({ scale.x,scale.y,0 }, rotWM.GetRotMat(), 0);

	//中心座標に足して
	MyRectangle::leftUp = center + Vec2(leftUp.x, leftUp.y);
	MyRectangle::leftDown = center + Vec2(leftDown.x, leftDown.y);
	MyRectangle::rightUp = center + Vec2(rightUp.x, rightUp.y);
	MyRectangle::rightDown = center + Vec2(rightDown.x, rightDown.y);
};


