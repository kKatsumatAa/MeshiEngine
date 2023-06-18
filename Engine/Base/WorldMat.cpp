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

void WorldMat::SetScale()
{
	matScale_ = {
		scale_.x_, 0, 0, 0,
			0, scale_.y_, 0, 0,
			0, 0, scale_.z_, 0,
			0, 0, 0, 1
	};

	matWorld_ *= matScale_;
}

void WorldMat::SetRot()
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

void WorldMat::SetTrans()
{
	matWorld_ *= {
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			trans_.x_, trans_.y_, trans_.z_, 1
	};
}

void WorldMat::SetWorld()
{
	//�s��v�Z
	CulcWorldMat();

	//�e��������
	if (parent_)
	{
		//�e�̍s����v�Z(�d������)
		SetParentWorld(parent_);
		//�e�̍s��������̍s��ɂ����Ă���
		RecursiveCulcParentMat(parent_, matWorld_);
	}
}

void WorldMat::SetParentWorld(WorldMat* parent)
{
	//�e�̍s��v�Z
	parent->CulcWorldMat();

	//�e�ɐe����������
	if (parent->parent_)
	{
		SetParentWorld(parent->parent_);
	}
}

void WorldMat::RecursiveCulcParentMat(WorldMat* parent, M4& childMat)
{
	//�q�̍s��ɐe�̍s���������
	childMat *= parent->matWorld_;

	//�܂��e�������
	if (parent->parent_)
	{
		RecursiveCulcParentMat(parent->parent_, childMat);
	}
}

void WorldMat::CulcWorldMat()
{
	matWorld_ = NORMAL_M;

	SetScale();
	SetRot();
	SetTrans();
}
