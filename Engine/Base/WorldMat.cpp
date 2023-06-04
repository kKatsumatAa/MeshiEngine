#include "WorldMat.h"
#include "Util.h"

WorldMat::WorldMat()
{
	matWorld =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matScale =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matRot =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };

	matTrans =
	{ 1,0,0,0,
	  0,1,0,0,
	  0,0,1,0,
	  0,0,0,1 };
}

void WorldMat::SetScale()
{
	matScale = {
		scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1
	};

	matWorld *= matScale;
}

void WorldMat::SetRot()
{
	matRot = normalM;
	matRot *= {
		cosf(rot.z), sinf(rot.z), 0, 0,
			-sinf(rot.z), cosf(rot.z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	};
	matRot *= {
		1, 0, 0, 0,
			0, cosf(rot.x), sinf(rot.x), 0,
			0, -sinf(rot.x), cosf(rot.x), 0,
			0, 0, 0, 1
	};
	matRot *= {
		cosf(rot.y), 0, -sinf(rot.y), 0,
			0, 1, 0, 0,
			sinf(rot.y), 0, cosf(rot.y), 0,
			0, 0, 0, 1
	};
	matWorld *= matRot;
}

void WorldMat::SetTrans()
{
	matWorld *= {
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			trans.x, trans.y, trans.z, 1
	};
}

void WorldMat::SetWorld()
{
	matWorld = normalM;

	SetScale();
	SetRot();
	SetTrans();

	//�e��������
	if (parent)
	{
		//�e�̍s����v�Z(�d������)
		SetParentWorld(parent);
		//�e�̍s��������̍s��ɂ����Ă���
		RecursiveCulcParentMat(parent, matWorld);
	}
}

void WorldMat::SetParentWorld(WorldMat* parent)
{
	//�e�̍s��v�Z
	parent->matWorld = normalM;

	parent->SetScale();
	parent->SetRot();
	parent->SetTrans();

	//�e�ɐe����������
	if (parent->parent)
	{
		SetParentWorld(parent->parent);
	}
}

void WorldMat::RecursiveCulcParentMat(WorldMat* parent, M4& childMat)
{
	//�q�̍s��ɐe�̍s���������
	childMat *= parent->matWorld;

	//�܂��e�������
	if (parent->parent)
	{
		RecursiveCulcParentMat(parent->parent, childMat);
	}
}
