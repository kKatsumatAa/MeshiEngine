#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Enemy :
	public Object
{
private:
	Vec3 velocity_ = { 0,0,0 };
	const float VELOCITY_TMP_ = 1.0f;
	bool isCantMove = false;

public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Enemy> Create();

private:
	//�ړ�
	void Move();

public:

	bool Initialize() override;

	void Update() override;

	void OnCollision(const CollisionInfo& info) override;
};

