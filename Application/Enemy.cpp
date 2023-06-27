#include "Enemy.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

using namespace DirectX;


std::unique_ptr<Enemy> Enemy::Create()
{
	std::unique_ptr<Enemy> instance = std::make_unique<Enemy>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//������
	if (!instance->Initialize())
	{
		assert(0);
	}

	return std::move(instance);
}

bool Enemy::Initialize()
{
	if (!Object::Initialize())
	{
		return false;
	}

	//���O
	SetObjName("enemy");

	//�R���C�_�[�̒ǉ�
	//float radius = 0.6f;
	//���a�������������畂�������W�����̒��S�ɂ���

	SetCollider(std::make_unique<SphereCollider>());

	return true;
}

void Enemy::Move()
{
	//�����Ȃ��������΂�
	if (isCantMove)
	{
		isCantMove = false;
		return;
	}

	//�v���C���[�Ɍ����킹��(�Ƃ肠�����J��������ʒu���炤)
	Vec3 playerPos = CameraManager::GetInstance().GetCamera("debugCamera")->GetEye();
	//�G����v���C���[�ւ̃x�N�g��
	Vec3 directionVec = playerPos - GetTrans();
	//���K��
	directionVec.Normalized();

	//�Q�[���X�s�[�h��������
	velocity_ = directionVec * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity();

	SetTrans(GetTrans() + velocity_);

	//�v���C���[�̕�����������
	/*directionRotTime += GameVelocityManager::GetInstance().GetVelocity();
	if (directionRotTime > DIRCTION_ROT_TIME_) { directionRotTime = 0; }
	float t = directionRotTime / DIRCTION_ROT_TIME_;*/

	//CulcFrontVec();

	Quaternion q = Quaternion::DirectionToDirection(
		GetFrontVec().GetNormalized(), directionVec.GetNormalized());

	//�p�x���Z�b�g
	//Vec3 rot = GetRotFromQuaternion(Slerp(GetQuaternion(), q, 1.0f));
	Vec3 rot = GetRotFromQuaternion(q);
	//Vec3 rot = GetRotFromMat(q.MakeRotateMatrix());
	Vec3 rot1 = Object::GetRot();
	SetRot(/*rot1 +*/ rot);
}

void Enemy::Update()
{
	//Move();

	Object::Update();
}


void Enemy::OnCollision(const CollisionInfo& info)
{
	//���Ńv���C���[�ɓ��������琶���Ă�t���O�I�t
	if (info.object_->GetObjName() == "player")
	{
		SetIsAlive(false);

		for (int32_t i = 0; i < 30; ++i)
		{
			const float MD_VEL = 0.1f;
			Vec3 vel{};
			vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

			Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

			ParticleManager::GetInstance()->Add(180, pos, vel, { 0,0,0 }, 1.0f, 0.0f);
		}
	}
	//�G���m�œ���������߂荞�܂Ȃ��悤�ɂ���
	else if (info.object_->GetObjName() == "enemy")
	{
		//����
		float length = (info.object_->GetScale().x_ + GetScale().x_);
		//�����̃x�N�g��
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		distanceVec.Normalized();

		//�ʒu�Z�b�g(���a�{���a�̒������x�N�g���̕������g���đ���)
		SetTrans(info.object_->GetTrans() + distanceVec * length * 1.001f);
		//�����Ȃ��悤�ɂ���
		isCantMove = true;
	}
}
