/*
* @file WorldMat.h
* @brief ワールド行列
*/

#pragma once
#include "Util.h"

class WorldMat
{
private:
	M4 matScale_;
	M4 matTrans_;

	bool isUseQMat_ = false;
	M4 matRot_;

public:
	M4 matWorld_;
	//ローカルの親行列（一つ上の親の行列）
	M4 localParentMat_;
	Vec3 scale_ = { 1.0f,1.0f,1.0f };
	Vec3 rot_ = { 0,0,0 };
	Vec3 localTrans_ = { 0,0,0 };
	Vec3 worldTrans_ = { 0,0,0 };
	Vec3 trans_ = { 0,0,0 };
	WorldMat* parent_ = nullptr;

	Quaternion quaternion_;


public:
	//コンストラクタ
	WorldMat();
	//デストラクタ
	~WorldMat() { ; }

public:
	//スケール行列計算
	void CalcScaleMat();
	//回転行列計算
	void CalcRotMat();
	//回転（クォータニオン）行列計算
	void CalcQuaternionRotMat();
	//トランス行列計算
	void CalcTransMat();
	//ワールド行列計算（スケール、回転、位置）
	void CalcWorldMat();
	//親の行列も考慮した行列計算
	void CalcAllTreeMat();
	//親から上のみ（自分は含まない）の行列計算をして取得
	M4 GetOnlyParentALLTreeMat();

	//クォータニオン取得
	Quaternion GetQuaternion();

	//クォータニオンの行列使うかセット
	void SetIsUseQuaMatRot(bool is) { isUseQMat_ = is; }
	//クォータニオンの行列使うか取得
	bool GetIsUseQuaMatRot() { return isUseQMat_; }

	//回転行列セット
	void SetRotMat(const M4& m) { matRot_ = m; }
	//回転行列取得
	const M4& GetRotMat()const { return matRot_; }

	//行列をそのままセット
	void SetLocalParentMat(const M4& mat) { localParentMat_ = mat; }

	//親子関係を考慮した位置を得る
	Vec3 GetWorldTrans();
	//親子関係を考慮したスケールを得る
	Vec3 GetWorldScale();

private:
	//親の行列も計算
	void CalcParentWorld(WorldMat* parent);
	//再帰,親があれば行列を掛ける
	void RecursiveCalcParentMat(WorldMat* parent, M4& childMat);
};