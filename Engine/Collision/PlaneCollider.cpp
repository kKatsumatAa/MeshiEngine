#include "PlaneCollider.h"


using namespace DirectX;

void PlaneCollider::Update()
{
	// �ʂ̃����o�ϐ����X�V
	Plane::distance = object->GetTrans().GetLength();

	//�@�����v�Z
	WorldMat worldNormal;
	worldNormal.rot = object->GetRot();
	worldNormal.SetWorld();
	//��]�̂�
	Vec3 normal = GetVec3xM4({ 0,0,-1.0f }, worldNormal.matWorld, 1);
	normal.Normalized();
	Plane::normal = { normal.x,normal.y,normal.z,1.0f };
}
