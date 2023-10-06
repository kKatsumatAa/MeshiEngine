#pragma once
#include <DirectXMath.h>
#include "Util.h"

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
	void RecursiveCalcParentMat(WorldMat* parent, M4& childMat);

public:
	M4 matWorld_;
	Vec3 scale_ = { 1.0f,1.0f,1.0f };
	Vec3 rot_ = { 0,0,0 };
	Vec3 trans_ = { 0,0,0 };
	WorldMat* parent_ = nullptr;

	Quaternion quaternion_;

	WorldMat();
	~WorldMat() { ; }
	//�s��v�Z
	void CalcScaleMat();
	void CalcRotMat();
	void CalcQuaternionRotMat();
	void CalcTransMat();
	void CalcWorldMat();
	//�e�̍s����l�������s��v�Z
	void CalcAllTreeMat();
	//�e�����̂݁i�����͊܂܂Ȃ��j�̍s��v�Z�����Ď擾
	M4 GetOnlyParentALLTreeMat();

	Quaternion GetQuaternion();

	void SetIsUseQuaMatRot(bool is) { isUseQMat_ = is; }
	bool GetIsUseQuaMatRot() { return isUseQMat_; }

	void SetRotMat(const M4& m) { matRot_ = m; }
	const M4& GetRotMat() { return matRot_; }

	//�e�q�֌W���l�������ʒu�𓾂�
	 Vec3 GetWorldTrans();
};