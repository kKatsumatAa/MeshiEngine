#include "PlaneCollider.h"


using namespace DirectX;

void PlaneCollider::Update()
{
	// ���[���h�s�񂩂���W�𒊏o
	const M4& matWorld = object->GetMatWorld();

	// �ʂ̃����o�ϐ����X�V
	Plane::distance = object->worldMat->trans.GetLength();

	//�@�����v�Z
	WorldMat worldNormal;
	worldNormal.rot = object->worldMat->rot;
	worldNormal.SetWorld();
	//��]�̂�
	Vec3 normal = GetVec3xM4({ 0,0,-1.0f }, worldNormal.matWorld, 0).GetNormalized();
	Plane::normal = { normal.x,normal.y,normal.z,0 };
}
