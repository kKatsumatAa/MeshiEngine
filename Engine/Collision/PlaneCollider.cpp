#include "PlaneCollider.h"


using namespace DirectX;

void PlaneCollider::Update()
{
	// �ʂ̃����o�ϐ����X�V
	Plane::distance = object_->GetTrans().GetLength();

	//�@�����v�Z
	WorldMat worldNormal;
	worldNormal.rot_ = object_->GetRot();
	worldNormal.CalcWorldMat();
	//��]�̂�
	Vec3 normal = GetVec3xM4({ 0,0,-1.0f }, worldNormal.matWorld_, 1);
	normal.Normalized();
	Plane::normal = { normal.x_,normal.y_,normal.z_,1.0f };
}
