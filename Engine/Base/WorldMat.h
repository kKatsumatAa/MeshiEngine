#pragma once
#include <DirectXMath.h>
#include "Util.h"
using namespace DirectX;

class WorldMat
{
private:
	M4 matScale_;
	M4 matTrans_;

	bool isUseQMat_ = false;
	M4 matRot_;

private:
	//�e�̍s����v�Z
	void SetParentWorld(WorldMat* parent);
	//�ċA,�e������΍s����|����
	void RecursiveCulcParentMat(WorldMat* parent, M4& childMat);

public:
	M4 matWorld_;
	Vec3 scale_ = { 1.0f,1.0f,1.0f };
	Vec3 rot_ = { 0,0,0 };
	Vec3 trans_ = { 0,0,0 };
	WorldMat* parent_ = nullptr;

	Quaternion quaternion_;

	WorldMat();
	//�s��v�Z
	void CulcScaleMat();
	void CulcRotMat();
	void CulcQuaternionRotMat();
	void CulcTransMat();
	void CulcWorldMat();
	//�e�̍s����l�������s��v�Z
	void CulcAllTreeMat();

	Quaternion GetQuaternion();

	void SetIsUseQuaMatRot(bool is) { isUseQMat_ = is; }
	bool GetIsUseQuaMatRot() { return isUseQMat_; }

	void SetRotMat(const M4& m) { matRot_ = m; }
	const M4& GetRotMat() { return matRot_; }
};