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
	//親の行列も計算
	void SetParentWorld(WorldMat* parent);
	//再帰,親があれば行列を掛ける
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
	//行列計算
	void CalcScaleMat();
	void CalcRotMat();
	void CalcQuaternionRotMat();
	void CalcTransMat();
	void CalcWorldMat();
	//親の行列も考慮した行列計算
	void CalcAllTreeMat();
	//親から上のみ（自分は含まない）の行列計算をして取得
	M4 GetOnlyParentALLTreeMat();

	Quaternion GetQuaternion();

	void SetIsUseQuaMatRot(bool is) { isUseQMat_ = is; }
	bool GetIsUseQuaMatRot() { return isUseQMat_; }

	void SetRotMat(const M4& m) { matRot_ = m; }
	const M4& GetRotMat() { return matRot_; }

	//親子関係を考慮した位置を得る
	 Vec3 GetWorldTrans();
};