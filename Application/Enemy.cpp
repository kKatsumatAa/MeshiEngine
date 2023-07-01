#include "Enemy.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

using namespace DirectX;


std::unique_ptr<Enemy> Enemy::Create(std::unique_ptr<WorldMat> worldMat, Gun* gun)
{
	std::unique_ptr<Enemy> instance = std::make_unique<Enemy>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//������
	if (!instance->Initialize(std::move(worldMat), gun))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Enemy::Initialize(std::unique_ptr<WorldMat> worldMat, Gun* gun)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}

	//���O
	SetObjName("enemy");

	//�R���C�_�[�̒ǉ�
	//float radius = 0.6f;
	//���a�������������畂�������W�����̒��S�ɂ���

	SetCollider(std::make_unique<SphereCollider>());

	gun_ = gun;

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
	Vec3 playerPos = CameraManager::GetInstance().GetCamera("playerCamera")->GetEye();
	//�G����v���C���[�ւ̃x�N�g��
	directionVec = playerPos - GetTrans();
	//���K��
	//directionVec.y_ = 0;
	directionVec.Normalized();

	//���݂̃X�s�[�h�ɕ����x�N�g�������A�Q�[���X�s�[�h��������
	velocity_ = GetVelocity() + directionVec * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity();
	//�X�s�[�h�̏���͒����Ȃ��悤��
	float length = velocity_.GetLength();
	//�X�s�[�h���v���X�ɂȂ��Ă���
	if (velocity_.Dot(directionVec) > 0)
	{
		//�_���[�W�̃N�[�����Z�b�g
		damageCoolTime = 0;
		//�X�s�[�h�̏�������Ȃ�
		velocity_ = velocity_.GetNormalized() * min(length, GameVelocityManager::GetInstance().GetVelocity() * VELOCITY_TMP_);
	}
	//�����蔻��p�ɃZ�b�g
	SetVelocity(velocity_);

	//�ʒu�Z�b�g
	SetTrans(GetTrans() + velocity_);

	////�v���C���[�̕�����������
	//directionRotTime += GameVelocityManager::GetInstance().GetVelocity();
	//if (directionRotTime > DIRCTION_ROT_TIME_) { directionRotTime = 0; }
	//float t = directionRotTime / DIRCTION_ROT_TIME_;

	//�G����v���C���[�ւ̃x�N�g��
	directionVec = playerPos - GetTrans();
	//�������ʃx�N�g���ƃv���C���[�ւ̃x�N�g��
	Vec3 fVTmp = GetFrontVecTmp().GetNormalized();
	Vec3 pDVTmp = directionVec.GetNormalized();

	//���ʃx�N�g������v���C���[�̕����x�N�g���ւ̉�]�N�H�[�^�j�I��
	Quaternion q = Quaternion::DirectionToDirection(fVTmp, pDVTmp);
	//��]��̃x�N�g��
	fVTmp = q.GetRotateVector(fVTmp);
	//���ʃx�N�g���Z�b�g
	SetFrontVec(fVTmp);

	//�p�x����Ȃ��čs������̂܂܎g��
	SetIsUseQuaternionMatRot(true);
	SetMatRot(q.MakeRotateMatrix());
}

void Enemy::Update()
{
	//�ړ�
	Move();

	//�e�ōU��
	if (gun_)
	{
		Vec3 playerPos = CameraManager::GetInstance().GetCamera("playerCamera")->GetEye();
		Vec3 directionV = playerPos - gun_->GetWorldTrans();
		gun_->Shot(directionV, 0);
	}

	//�_���[�W�󂯂�N�[���^�C�����Q�[���X�s�[�h��������
	damageCoolTime -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	Object::Update();
}


//----------------------------------------------------------------------------------
void Enemy::KnockBack(const CollisionInfo& info)
{
	//����
	float length = (info.object_->GetScale().x_ + GetScale().x_);
	//�����̃x�N�g��
	Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
	//��
	distanceVec.y_ = 0;
	velocity_.y_ = 0;
	distanceVec.Normalized();
	//�m�b�N�o�b�N
	velocity_ += distanceVec * length * 0.23f;
	SetVelocity(velocity_);
	//�_���[�W���󂯂�N�[���^�C��
	damageCoolTime = 20;

	//�e�����Ă����痎�Ƃ�
	if (gun_)
	{
		gun_->SetFallVec(-distanceVec);
		gun_->ChangeOwner(nullptr);
		gun_ = nullptr;
	}
}

void Enemy::DamageParticle(const CollisionInfo& info)
{
	for (int32_t i = 0; i < 30; ++i)
	{
		const float MD_VEL = 0.1f;
		Vec3 vel{};
		vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

		Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

		float scale = (float)rand() / RAND_MAX;
		float scale2 = (float)rand() / RAND_MAX;

		ParticleManager::GetInstance()->Add(100, pos, vel, { 0,0,0 }, scale, scale2, { 2.0f,0,0,1.5f }, { 0,0,0,0.0f });
	}
}

void Enemy::OnCollision(const CollisionInfo& info)
{
	//�v���C���[�ɓ���������
	if (info.object_->GetObjName() == "player")
	{
		////����
		float length = (/*info.object_->GetScale().x_*/ +GetScale().x_);
		//�����̃x�N�g��
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		//����y�͓������Ȃ�
		distanceVec.y_ = 0;
		distanceVec.Normalized();

		//�����Ȃ��悤�ɂ���
		isCantMove = true;
	}
	//�v���C���[�̍U���Ƃ̔���
	else if (info.object_->GetObjName() == "playerAttack")
	{
		if (damageCoolTime <= 0)
		{
			//�m�b�N�o�b�N
			KnockBack(info);

			//hp���炷
			hp--;
			//���S
			if (hp <= 0)
			{
				SetIsAlive(false);
			}

			//�p�[�e�B�N��
			DamageParticle(info);
		}
	}
	//�e�ɓ���������
	else if (info.object_->GetObjName() == "bullet")
	{
		SetIsAlive(false);

		//�m�b�N�o�b�N
		KnockBack(info);

		//�p�[�e�B�N��
		DamageParticle(info);
	}
	//�e�ɓ���������
	else if (info.object_->GetObjName() == "gun")
	{
		Gun* gun = dynamic_cast<Gun*>(info.object_);

		//�e������i�����Ă�l������Ƃ��̂݁j
		if (gun->GetParent() && gun->GetFallVelocity().GetLength() == 0)
		{
			return;
		}

		//
		KnockBack(info);
	}
	//�G���m�œ���������߂荞�܂Ȃ��悤�ɂ���
	else if (info.object_->GetObjName() == "enemy")
	{
		//����
		float length = (info.object_->GetScale().x_ + GetScale().x_);
		//�����̃x�N�g��
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		//��
		distanceVec.y_ = 0;
		distanceVec.Normalized();
		//�ʒu�Z�b�g(���a�{���a�̒������x�N�g���̕������g���đ���)
		SetTrans(info.object_->GetTrans() + distanceVec * length * 1.001f);
		////�����Ȃ��悤�ɂ���
		//isCantMove = true;

		//���͓����Ȃ��悤�ɂ���
		SetVelocity({ GetVelocity().x_,0,GetVelocity().z_ });
		info.object_->SetVelocity({ info.object_->GetVelocity().x_,0,info.object_->GetVelocity().z_ });
		//��̃x�N�g���̍��v�̒���
		float addLength = GetVelocity().GetLength() + info.object_->GetVelocity().GetLength();
		//�����̃X�s�[�h�̃x�N�g���̒����̊����i���v�̒����Ŋ���i0�`1.0f�j�j
		float myLengthRatio = GetVelocity().GetLength() / addLength;

		//�Փˌ�̎����̃X�s�[�h�x�N�g����[���݂̃X�s�[�h�x�N�g��]+[���肩�玩���ւ̃x�N�g��]*[����̒����̊���]
		SetVelocity((GetVelocity() + distanceVec.GetNormalized() * addLength * (1.0f - myLengthRatio)) * 0.63f);
		//�Փˌ�̑���̃X�s�[�h�x�N�g����[���݂̃X�s�[�h�x�N�g��]+[���̃C���X�^���X���瑊��ւ̃x�N�g��]*[���̃C���X�^���X�̒����̊���]
		info.object_->SetVelocity((info.object_->GetVelocity() - distanceVec.GetNormalized() * addLength * (myLengthRatio)) * 0.63f);
	}
}
