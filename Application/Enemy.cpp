#include "Enemy.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

using namespace DirectX;


std::unique_ptr<Enemy> Enemy::Create(std::unique_ptr<WorldMat> worldMat, Weapon* weapon)
{
	std::unique_ptr<Enemy> instance = std::make_unique<Enemy>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//������
	if (!instance->Initialize(std::move(worldMat), weapon))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Enemy::Initialize(std::unique_ptr<WorldMat> worldMat, Weapon* weapon)
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
	//���葮��
	GetCollider()->SetAttribute(COLLISION_ATTR_ENEMYS);

	if (weapon)
	{
		weapon_ = weapon;
		//��������X�ݒ�
		PickUpWeapon(weapon_);
	}

	hp_ = HP_TMP_;

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
	if (velocity_.Dot(directionVec) >= 0)
	{
		directionVec = playerPos - GetTrans();
		//������x�߂Â�����~�܂�
		if (directionVec.GetLength() < GetScale().GetLength() * 1.5f)
		{
			velocity_ = { 0,0,0 };
		}
		directionVec.Normalized();

		//�_���[�W�̃N�[�����Z�b�g
		damageCoolTime = 0;
		//�X�s�[�h�̏�������Ȃ�
		velocity_ = velocity_.GetNormalized() * min(length, GameVelocityManager::GetInstance().GetVelocity() * VELOCITY_TMP_ / 1.5f);
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

	//����ōU��
	if (weapon_ != nullptr && weapon_->GetIsAlive())
	{
		Vec3 playerPos = CameraManager::GetInstance().GetCamera("playerCamera")->GetEye();
		Vec3 directionV = playerPos - weapon_->GetWorldTrans();
		weapon_->Attack(directionV.GetNormalized(), 0, this);
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

	//���펝���Ă����痎�Ƃ�
	if (weapon_)
	{
		distanceVec.y_ = 0.2f;
		FallWeapon({ -distanceVec.x_,distanceVec.y_,-distanceVec.z_ });
	}
}

void Enemy::DamageParticle(const CollisionInfo& info)
{
	for (int32_t i = 0; i < 50; ++i)
	{
		Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

		float scaleTmp = GetScale().GetLength();

		Vec3 addPos = Vec3(GetRand(-GetScale().x_, GetScale().x_), GetRand(-GetScale().y_, GetScale().y_), GetRand(-GetScale().z_, GetScale().z_)) / 4.0f;

		pos += addPos;

		const int32_t LIFE_TIME = 100;

		Vec3 vel = /*(GetTrans() - pos) * */Vec3(GetRand(-0.5f, 0.5f), GetRand(-0.5f, 0.5f), GetRand(-0.5f, 0.5f)) / 2.0f;

		float scale = GetRand(scaleTmp / 20.0f, scaleTmp / 15.0f);
		float scale2 = GetRand(0, scaleTmp / 25.0f);

		ParticleManager::GetInstance()->Add(LIFE_TIME, pos, vel, { 0,-0.002f,0 }, scale, scale2, { 3.0f,0.0f,0.0f,0.95f }, { 0,0,0,0.0f },
			PI * 3.0f, -PI * 3.0f);
	}
}

void Enemy::OnCollision(const CollisionInfo& info)
{
	//�v���C���[�ɓ���������
	if (info.object_->GetObjName() == "player")
	{
		////����
		float length = (info.object_->GetScale().x_ + GetScale().x_);
		//�����̃x�N�g��
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		//����y�͓������Ȃ�
		distanceVec.y_ = 0;
		distanceVec.Normalized();

		//�߂荞�܂Ȃ��悤�Ɉʒu�Z�b�g(���a�{���a�̒������x�N�g���̕������g���đ���)
		//Vec3 ansPosP = info.object_->GetTrans() + distanceVec * length * 0.5f;
		Vec3 ansPosE = info.object_->GetTrans() + distanceVec * length * 1.001f;
		//SetTrans(ansPosP);
		SetTrans(ansPosE);

		//info.object_->SetTrans(ansPos);
		SetVelocity({ 0,0,0 });

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
			hp_--;
			//���S
			if (hp_ <= 0)
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
	//�X�e�[�W�Ɠ���������
	if (info.object_->GetObjName() == "stage")
	{
		Vec3 interPos = { info.inter_.m128_f32[0], info.inter_.m128_f32[1], info.inter_.m128_f32[2] };
		Vec3 distanceVec = GetWorldTrans() - interPos;
		distanceVec = distanceVec.GetNormalized() * GetScale().x_;

		SetTrans(interPos + distanceVec);
	}
}
