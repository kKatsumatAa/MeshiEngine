#pragma once
#include <DirectXMath.h>
#include "Vec3.h"
#include "M4.h"
using namespace DirectX;

class WorldMat
{
private:
	void SetScale();
	void SetRot();
	void SetTrans();
	M4 matScale;
	M4 matRot;
	M4 matTrans;


private:
	//親の行列も計算
	void SetParentWorld(WorldMat* parent);
	//再帰,親があれば行列を掛ける
	void RecursiveCulcParentMat(WorldMat* parent, M4& childMat);

public:
	M4 matWorld;
	Vec3 scale = { 1.0f,1.0f,1.0f };
	Vec3 rot = { 0,0,0 };
	Vec3 trans = { 0,0,0 };
	WorldMat* parent = nullptr;

	//
	WorldMat();
	void SetWorld();
};