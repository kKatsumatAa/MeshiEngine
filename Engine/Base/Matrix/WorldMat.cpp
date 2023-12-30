#include "WorldMat.h"
#include "Util.h"

WorldMat::WorldMat()
{
	matWorld_ = M4::NORMAL_M;

	localParentMat_ = M4::NORMAL_M;

	matScale_ = M4::NORMAL_M;

	matRot_ = M4::NORMAL_M;

	matTrans_ = M4::NORMAL_M;
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
		Quaternion qZ = Quaternion::MakeAxisAngle(Constant::AXIS_Z_, rot_.z);
		matRot_ *= qZ.MakeRotateMatrix();
		//x
		Quaternion qX = Quaternion::MakeAxisAngle(Constant::AXIS_X_, rot_.x);
		matRot_ *= qX.MakeRotateMatrix();
		//y
		Quaternion qY = Quaternion::MakeAxisAngle(Constant::AXIS_Y_, rot_.y);
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
		CalcParentWorld(parent_);
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

void WorldMat::CalcParentWorld(WorldMat* parent)
{
	//親の行列計算
	parent->CalcWorldMat();

	//親に親があったら
	if (parent->parent_)
	{
		CalcParentWorld(parent->parent_);
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

	//直属の親行列をかける
	matWorld_ *= localParentMat_;
}

Quaternion WorldMat::GetQuaternion()
{
	//z
	Quaternion qZ = Quaternion::MakeAxisAngle(Constant::AXIS_Z_, rot_.z);
	//x
	Quaternion qX = Quaternion::MakeAxisAngle(Constant::AXIS_X_, rot_.x);
	//y
	Quaternion qY = Quaternion::MakeAxisAngle(Constant::AXIS_Y_, rot_.y);

	//合成(あってるかわからない)
	qZ = qZ * qY * qX;

	return Quaternion(qZ);
}

Vec3 WorldMat::GetWorldTrans()
{
	CalcAllTreeMat();

	return Vec3((float)matWorld_.m_[3][0], (float)matWorld_.m_[3][1], (float)matWorld_.m_[3][2]);
}

Vec3 WorldMat::GetWorldScale()
{
	CalcAllTreeMat();

	Vec3 scale;
	scale.x = GetVec3xM4({ 1,0,0 }, matWorld_, 0).GetLength();
	scale.y = GetVec3xM4({ 0,1,0 }, matWorld_, 0).GetLength();
	scale.z = GetVec3xM4({ 0,0,1 }, matWorld_, 0).GetLength();

	return scale;
}
