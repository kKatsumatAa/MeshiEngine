#pragma once
#include <DirectXMath.h>
#include "Vec3.h"
#include "M4.h"
using namespace DirectX;

class WorldMat
{
private:
	M4 matScale_;
	M4 matRot_;
	M4 matTrans_;


private:
	//�e�̍s����v�Z
	void SetParentWorld(WorldMat* parent);
	//�ċA,�e������΍s����|����
	void RecursiveCulcParentMat(WorldMat* parent, M4& childMat);
	//�s��v�Z�̂�
	void CulcWorldMat();

public:
	M4 matWorld_;
	Vec3 scale_ = { 1.0f,1.0f,1.0f };
	Vec3 rot_ = { 0,0,0 };
	Vec3 trans_ = { 0,0,0 };
	WorldMat* parent_ = nullptr;

	//
	WorldMat();
	void SetScaleMat();
	void SetRotMat();
	void SetTransMat();
	void SetWorldMat();
};