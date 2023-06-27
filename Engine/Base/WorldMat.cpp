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

void WorldMat::CulcScaleMat()
{
	matScale_ = {
		scale_.x_, 0, 0, 0,
			0, scale_.y_, 0, 0,
			0, 0, scale_.z_, 0,
			0, 0, 0, 1
	};

	matWorld_ *= matScale_;
}

void WorldMat::CulcRotMat()
{
	matRot_ = NORMAL_M;
	matRot_ *= {
		cosf(rot_.z_), sinf(rot_.z_), 0, 0,
			-sinf(rot_.z_), cosf(rot_.z_), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	};
	matRot_ *= {
		1, 0, 0, 0,
			0, cosf(rot_.x_), sinf(rot_.x_), 0,
			0, -sinf(rot_.x_), cosf(rot_.x_), 0,
			0, 0, 0, 1
	};
	matRot_ *= {
		cosf(rot_.y_), 0, -sinf(rot_.y_), 0,
			0, 1, 0, 0,
			sinf(rot_.y_), 0, cosf(rot_.y_), 0,
			0, 0, 0, 1
	};
	matWorld_ *= matRot_;
}

void WorldMat::CulcQuaternionRotMat()
{
	matRot_ = NORMAL_M;
	//z
	Quaternion qZ = Quaternion::MakeAxisAngle({ 0,0,1.0f }, rot_.z_);
	//x
	Quaternion qX = Quaternion::MakeAxisAngle({ 1.0f,0,0 }, rot_.x_);
	//y
	Quaternion qY = Quaternion::MakeAxisAngle({ 0,1.0f,0 }, rot_.y_);

	//合成
	qZ = qZ * qY * qX;


	matWorld_ *= qZ.MakeRotateMatrix();
}

void WorldMat::CulcTransMat()
{
	matWorld_ *= {
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			trans_.x_, trans_.y_, trans_.z_, 1
	};
}

void WorldMat::CulcAllTreeMat()
{
	//行列計算
	CulcWorldMat();

	//親がいたら
	if (parent_)
	{
		//親の行列も計算(重いかも)
		SetParentWorld(parent_);
		//親の行列を自分の行列にかけていく
		RecursiveCulcParentMat(parent_, matWorld_);
	}
}

void WorldMat::SetParentWorld(WorldMat* parent)
{
	//親の行列計算
	parent->CulcWorldMat();

	//親に親があったら
	if (parent->parent_)
	{
		SetParentWorld(parent->parent_);
	}
}

void WorldMat::RecursiveCulcParentMat(WorldMat* parent, M4& childMat)
{
	//子の行列に親の行列をかける
	childMat *= parent->matWorld_;

	//まだ親がいれば
	if (parent->parent_)
	{
		RecursiveCulcParentMat(parent->parent_, childMat);
	}
}

void WorldMat::CulcWorldMat()
{
	matWorld_ = NORMAL_M;

	CulcScaleMat();
	//回転にクォータニオン
	CulcQuaternionRotMat();
	CulcTransMat();
}

Quaternion WorldMat::GetQuaternion()
{
	//z
	Quaternion qZ = Quaternion::MakeAxisAngle({ 0,0,1.0f }, rot_.z_);
	//x
	Quaternion qX = Quaternion::MakeAxisAngle({ 1.0f,0,0 }, rot_.x_);
	//y
	Quaternion qY = Quaternion::MakeAxisAngle({ 0,1.0f,0 }, rot_.y_);

	//合成
	qZ = qZ * qY * qX;

	return Quaternion(qZ);
}
