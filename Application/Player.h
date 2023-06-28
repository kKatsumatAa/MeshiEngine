#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Player :
	public Object
{
private:
	const float ANGLE_VEL_EXTEND_ = 0.003f;
	Vec3 cameraRot_ = { 0,0,0 };

	Vec3 frontVec_ = { 0,0,1.0f };
	Vec3 upVec_ = { 0,1.0f,0 };
	Vec3 rightVec_ = { 0,0,0 };

public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Player> Create();

private:
	//�ړ�
	void Move();
	//������ς���
	void DirectionUpdate();

public:

	bool Initialize() override;

	void Update() override;

	void OnCollision(const CollisionInfo& info) override;
};

