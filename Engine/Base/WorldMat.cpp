#include "WorldMat.h"
#include "Util.h"

WorldMat::WorldMat()
{
	matWorld_ =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matScale_ =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matRot_ =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matTrans_ =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };
}

void WorldMat::CalcScaleMat()
{
	matScale_ = {
		scale_.x, 0, 0, 0,
			0, scale_.y, 0, 0,
			0, 0, scale_.z, 0,
			0, 0, 0, 1
	};

	matWorld_ *= matScale_;
}

void WorldMat::CalcRotMat()
{
	matRot_ = M4::NORMAL_M;
	matRot_ *= {
		cosf(rot_.z), sinf(rot_.z), 0, 0,
			-sinf(rot_.z), cosf(rot_.z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	};
	matRot_ *= {
		1, 0, 0, 0,
			0, cosf(rot_.x), sinf(rot_.x), 0,
			0, -sinf(rot_.x), cosf(rot_.x), 0,
			0, 0, 0, 1
	};
	matRot_ *= {
		cosf(rot_.y), 0, -sinf(rot_.y), 0,
			0, 1, 0, 0,
			sinf(rot_.y), 0, cosf(rot_.y), 0,
			0, 0, 0, 1
	};
	matWorld_ *= matRot_;
}

void WorldMat::CalcQuaternionRotMat()
{
	//角度じゃなく、回転行列そのまま使う
	if (isUseQMat_)
	{
		matWorld_ *= matRot_;
	}
	else
	{
		matRot_ = M4::NORMAL_M;

		//z
		Quaternion qZ = Quaternion::MakeAxisAngle({ 0,0,1.0f }, rot_.z);
		matRot_ *= qZ.MakeRotateMatrix();
		//x
		Quaternion qX = Quaternion::MakeAxisAngle({ 1.0f,0,0 }, rot_.x);
		matRot_ *= qX.MakeRotateMatrix();
		//y
		Quaternion qY = Quaternion::MakeAxisAngle({ 0,1.0f,0 }, rot_.y);
		matRot_ *= qY.MakeRotateMatrix();

		matWorld_ *= matRot_;
	}
}

void WorldMat::CalcTransMat()
{
	matWorld_ *= {
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			trans_.x, trans_.y, trans_.z, 1
	};
}

void WorldMat::CalcAllTreeMat()
{
	//行列計算
	CalcWorldMat();

	//親がいたら
	if (parent_ != nullptr)
	{
		//親の行列も計算(重いかも)
		SetParentWorld(parent_);
		//親の行列を自分の行列にかけていく
		RecursiveCalcParentMat(parent_, matWorld_);
	}
}

M4 WorldMat::GetOnlyParentALLTreeMat()
{
	if (parent_)
	{
		//親から上のみの行列計算
		parent_->CalcAllTreeMat();

		return parent_->matWorld_;
	}

	return M4::NORMAL_M;
}

void WorldMat::SetParentWorld(WorldMat* parent)
{
	//親の行列計算
	parent->CalcWorldMat();

	//親に親があったら
	if (parent->parent_)
	{
		SetParentWorld(parent->parent_);
	}
}

void WorldMat::RecursiveCalcParentMat(WorldMat* parent, M4& childMat)
{
	//子の行列に親の行列をかける
	childMat *= parent->matWorld_;

	//まだ親がいれば
	if (parent->parent_)
	{
		RecursiveCalcParentMat(parent->parent_, childMat);
	}
}

void WorldMat::CalcWorldMat()
{
	matWorld_ = M4::NORMAL_M;

	CalcScaleMat();
	//回転にクォータニオン
	CalcQuaternionRotMat();
	CalcTransMat();
}

Quaternion WorldMat::GetQuaternion()
{
	//z
	Quaternion qZ = Quaternion::MakeAxisAngle({ 0,0,1.0f }, rot_.z);
	//x
	Quaternion qX = Quaternion::MakeAxisAngle({ 1.0f,0,0 }, rot_.x);
	//y
	Quaternion qY = Quaternion::MakeAxisAngle({ 0,1.0f,0 }, rot_.y);

	//合成(あってるかわからない)
	qZ = qZ * qY * qX;

	return Quaternion(qZ);
}

Vec3 WorldMat::GetWorldTrans()
{
	CalcAllTreeMat();

	return Vec3((float)matWorld_.m_[3][0], (float)matWorld_.m_[3][1], (float)matWorld_.m_[3][2]);
}
