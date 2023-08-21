#include "Enemy.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

using namespace DirectX;


std::unique_ptr<Enemy> Enemy::Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon)
{
	std::unique_ptr<Enemy> instance = std::make_unique<Enemy>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//������
	if (!instance->Initialize(std::move(worldMat), waveNum, weapon))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Enemy::Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}


	if (weapon)
	{
		weapon_ = weapon;
		//��������X�ݒ�
		PickUpWeapon(weapon_);
	}

	hp_ = HP_TMP_;

	waveNum_ = waveNum;

	//�f�B�]���u
	SetisDissolve(true);
	//�f�B�]���u�摜
	uint64_t handle;
	handle = TextureManager::LoadGraph("dissolveMask.png");
	SetDissolveTexHandle(handle);


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

	//�n�ʂƕǂƂ̔���
	OnGroundAndWallUpdate(GetScale().y_, GameVelocityManager::GetInstance().GetVelocity());


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
	if (GetModel())
	{
		GetModel()->SetMaterialExtend({ 1.0f,8.0f,20.0f });
	}

	//�ړ�
	Move();

	//����ōU��
	if (weapon_ != nullptr && weapon_->GetIsAlive())
	{
		Vec3 playerPos = CameraManager::GetInstance().GetCamera("playerCamera")->GetEye();
		Vec3 directionV = playerPos - weapon_->GetWorldTrans();
		weapon_->Attack(directionV.GetNormalized(), 0, this);
	}

	//hp�ɂ���ăf�B�]���u
	SetDissolveT((1.0f - (float)hp_ / (float)HP_TMP_) * DISSOLVE_POW_);

	//�_���[�W�󂯂�N�[���^�C�����Q�[���X�s�[�h��������
	damageCoolTime -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//�A�j���[�V�������Q�[���X�s�[�h
	SetAnimationSpeed(GameVelocityManager::GetInstance().GetVelocity() * 3.0f);

	Character::Update();
}

void Enemy::Draw()
{
	Object::DrawModel(nullptr, nullptr, { 3.0f,0,0,1.0f });
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
	velocity_ += distanceVec * length * KNOCK_BACK_POW_;
	SetVelocity(velocity_);
	//�_���[�W���󂯂�N�[���^�C��
	damageCoolTime = 20;

	//���펝���Ă����痎�Ƃ�
	if (weapon_)
	{
		distanceVec.y_ = 0.2f;
		FallWeapon({ -distanceVec.x_ * WEAPON_FALL_VEL_EXTEND_,distanceVec.y_,-distanceVec.z_ * WEAPON_FALL_VEL_EXTEND_ });
	}
}

void Enemy::DamageParticle(const CollisionInfo& info)
{
	for (int32_t i = 0; i < 80; ++i)
	{
		Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

		float scaleTmp = GetScale().GetLength();

		Vec3 addPos = Vec3(GetRand(-GetScale().x_, GetScale().x_), GetRand(-GetScale().y_, GetScale().y_), GetRand(-GetScale().z_, GetScale().z_)) / 2.0f;

		pos += addPos;

		const int32_t LIFE_TIME = 40;

		Vec3 vel = /*(GetTrans() - pos) * */Vec3(GetRand(-0.5f, 0.5f), GetRand(-0.5f, 0.5f), GetRand(-0.5f, 0.5f)) / 2.0f;

		float scale = GetRand(scaleTmp / 50.0f, scaleTmp / 15.0f);
		float scale2 = GetRand(0, scaleTmp / 80.0f);

		ParticleManager::GetInstance()->Add(LIFE_TIME, pos, vel, { 0,-0.002f,0 }, scale, scale2, { 3.0f,0.0f,0.0f,0.95f }, { 0,0,0,0.0f },
			PI * 10.0f, -PI * 10.0f);
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
			Damaged(1, [=]() {SetIsAlive(false); });

			//�p�[�e�B�N��
			DamageParticle(info);
		}
	}
	//�e�ɓ���������
	else if (info.object_->GetObjName() == "bullet")
	{
		//����hp���_���[�W�󂯂ē|���
		Damaged(hp_, [=]() {SetIsAlive(false); });

		//�m�b�N�o�b�N
		KnockBack(info);

		//�p�[�e�B�N��
		DamageParticle(info);
	}
	//�e�ɓ���������
	else if (info.object_->GetObjName() == "gun")
	{
		Gun* gun = dynamic_cast<Gun*>(info.object_);

		//�������Ă���Ƃ��̂�
		if (gun != nullptr && gun->GetParent() == nullptr && gun->GetIsThrowing() && gun->GetFallVelocity().GetLength() != 0)
		{
			//
			KnockBack(info);
		}
	}
	//�G���m�œ���������߂荞�܂Ȃ��悤�ɂ���
	else if (info.object_->GetObjName().find("enemy") != std::string::npos)
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
