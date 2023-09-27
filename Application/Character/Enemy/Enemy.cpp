#include "Enemy.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "EnemyState.h"
#include "LevelManager.h"
#include "Bullet.h"

using namespace DirectX;


const float Enemy::S_LENGTH_MAX_ = 10000;

void Enemy::EmergeInitialize()
{
	LightManager* lightM = LevelManager::GetInstance().GetLightManager();

	//�g���郉�C�g�������
	if (lightM->GetDoNotUsePointLightIndex(lightIndexTmp_))
	{
		lightM->SetPointLightActive(lightIndexTmp_, true);
		lightM->SetPointLightAtten(lightIndexTmp_, { 0.977f,0.493f,0.458f });
		lightM->SetPointLightColor(lightIndexTmp_, { 5.0f,0,0 });
		lightM->SetPointLightPos(lightIndexTmp_,
			{ GetTrans().x, GetTrans().y, GetTrans().z });
	}
	//hp�n
	HPUpdate();
}

void Enemy::ChangeEnemyState(std::unique_ptr<EnemyState> state)
{
	state_.reset();
	state_ = std::move(state);
	state_->SetEnemy(this);
	state_->Initialize();
}


std::unique_ptr<Enemy> Enemy::Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon, IModel* model)
{
	std::unique_ptr<Enemy> instance = std::make_unique<Enemy>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//������
	if (!instance->Initialize(std::move(worldMat), waveNum, weapon, model))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Enemy::Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon, IModel* model)
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

	//model
	model->SetMaterialExtend({ 1.0f,3.0f,140.0f });

	//�f�B�]���u
	SetisDissolve(true);
	//�f�B�]���u�摜
	uint64_t handle;
	handle = TextureManager::LoadGraph("dissolveMask.png");
	SetDissolveTexHandle(handle);

	//�X�e�[�g�ύX
	ChangeEnemyState(std::make_unique<EnemyStateEmergeEffect>());

	return true;
}

void Enemy::AllMove(const Vec3& targetPos)
{
	WalkToTarget(targetPos);

	CollisionWallAndFloor();
}

void Enemy::Attack(const Vec3& targetPos)
{
	//����ōU��
	if (weapon_ != nullptr && weapon_->GetIsAlive())
	{
		Vec3 directionV = targetPos - weapon_->GetWorldTrans();
		weapon_->Attack(directionV.GetNormalized(), 0, this);
	}
}

void Enemy::WalkToTarget(const Vec3& targetPos)
{
	//�����Ȃ��������΂�
	if (isCantMove)
	{
		isCantMove = false;
		return;
	}

	//���K��
	directionVec_.y = 0;
	directionVec_.Normalized();

	//���݂̃X�s�[�h�ɕ����x�N�g�������A�Q�[���X�s�[�h��������
	velocity_ = GetVelocity() + directionVec_ * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity();
	//�X�s�[�h�̏���͒����Ȃ��悤��
	float length = velocity_.GetLength();
	//�X�s�[�h���v���X�ɂȂ��Ă���
	if (velocity_.Dot(directionVec_) >= 0)
	{
		directionVec_ = targetPos - GetTrans();
		//������x�߂Â�����~�܂�
		if (directionVec_.GetLength() < GetScale().GetLength() * 1.5f)
		{
			velocity_ = { 0,0,0 };
		}
		directionVec_.Normalized();

		//�_���[�W�̃N�[�����Z�b�g
		damageCoolTime_ = 0;
	}
	//�X�s�[�h�̏�������Ȃ�
	velocity_ = velocity_.GetNormalized() * min(fabsf(length), GameVelocityManager::GetInstance().GetVelocity() * VELOCITY_TMP_ / 1.5f);

	//�����蔻��p�ɃZ�b�g
	SetVelocity(velocity_);

	//�ʒu�Z�b�g
	SetTrans(GetTrans() + velocity_);

	//�����ύX
	DirectionUpdate(targetPos);
}

void Enemy::CollisionWallAndFloor()
{
	//�n�ʂƕǂƂ̔���
	OnGroundAndWallUpdate(GetScale().y, GameVelocityManager::GetInstance().GetVelocity());
}

void Enemy::DirectionUpdate(const Vec3& targetPos)
{
	//�G����^�[�Q�b�g�ւ̃x�N�g��
	directionVec_ = targetPos - GetTrans();
	//�������ʃx�N�g���ƃ^�[�Q�b�g�ւ̃x�N�g��
	Vec3 fVTmp = GetFrontVecTmp().GetNormalized();
	Vec3 pDVTmp = directionVec_.GetNormalized();

	//���ʃx�N�g������^�[�Q�b�g�̕����x�N�g���ւ̉�]�N�H�[�^�j�I��
	Quaternion q = Quaternion::DirectionToDirection(fVTmp, pDVTmp, 0.0f);
	//��]��̃x�N�g��
	fVTmp = q.GetRotateVector(fVTmp);
	//���ʃx�N�g���Z�b�g
	SetFrontVec(fVTmp);

	//�p�x����Ȃ��čs������̂܂܎g��
	SetIsUseQuaternionMatRot(true);
	SetMatRot(q.MakeRotateMatrix());
}

void Enemy::HPUpdate()
{
	//hp�ɂ���ăf�B�]���u
	SetDissolveT((1.0f - (float)hp_ / (float)HP_TMP_) * DISSOLVE_POW_);

	//�|���S�����Ƃɓ����悤��
	Mesh::PolygonOffset offsetData;
	offsetData.interval = GetRand(15.0f, 35.0f) * (1.0f - GameVelocityManager::GetInstance().GetVelocity() * 1.5f);
	offsetData.length = GetRand(-GetScale().x, GetScale().x) * 5.5f * max(1.0f - (float)hp_ / (float)HP_TMP_, 0);
	offsetData.ratio = (1.0f - (float)hp_ / (float)HP_TMP_);
	Object::SetMeshPolygonOffsetData(offsetData);
}

//----------------------------------------------------------------
void Enemy::Update()
{
	//�_���[�W�󂯂�N�[���^�C�����Q�[���X�s�[�h��������
	damageCoolTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//�A�j���[�V�������Q�[���X�s�[�h
	SetAnimationSpeed(min(GameVelocityManager::GetInstance().GetVelocity() * 3.0f,
		GameVelocityManager::GetInstance().GAME_VELOCITY_MAX_));

	//�X�e�[�g
	state_->Update();

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
	float length = (info.object_->GetScale().z + GetScale().z);
	//�����̃x�N�g��
	Vec3 distanceVec = GetTrans() - info.object_->GetWorldTrans();
	//��
	distanceVec.y = 0;
	velocity_.y = 0;
	distanceVec.Normalized();
	//�m�b�N�o�b�N
	velocity_ += distanceVec * length * KNOCK_BACK_POW_;
	SetVelocity(velocity_);
	//�_���[�W���󂯂�N�[���^�C��
	damageCoolTime_ = 20;

	//���펝���Ă����痎�Ƃ�
	if (weapon_)
	{
		distanceVec.y = 0.2f;
		FallWeapon(Vec3(-distanceVec.x, distanceVec.y, -distanceVec.z) * length);
	}
}

void Enemy::DamageParticle(const CollisionInfo& info)
{
	for (int32_t i = 0; i < 80; ++i)
	{
		Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

		float scaleTmp = GetScale().GetLength();

		Vec3 addPos = Vec3(GetRand(-GetScale().x, GetScale().x), GetRand(-GetScale().y, GetScale().y), GetRand(-GetScale().z, GetScale().z)) / 2.0f;

		pos += addPos;

		const int32_t LIFE_TIME = 40;

		//����̑��x���g��
		Vec3 infoVec = info.object_->GetVelocity().GetNormalized();

		Vec3 vel = Vec3(infoVec.x * GetRand(-0.1f, 1.0f),
			infoVec.y * GetRand(-0.1f, 1.0f),
			infoVec.z * GetRand(-0.1f, 1.0f));

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
		float length = (info.object_->GetScale().x + GetScale().x);
		//�����̃x�N�g��
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		//����y�͓������Ȃ�
		distanceVec.y = 0;
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
	else if (info.object_->GetObjName().find("hand") != std::string::npos)
	{
		if (damageCoolTime_ <= 0)
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
		//�e���������{�l��������
		if (this == dynamic_cast<Bullet*>(info.object_)->GetOwner())
		{
			return;
		}

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

		if (!(gun != nullptr && gun->GetParent() == nullptr))
		{
			return;
		}

		//�������Ă���Ƃ��̂�
		if (gun->GetIsThrowing() && gun->GetFallVelocity().GetLength() != 0)
		{
			//
			KnockBack(info);
		}
		//�����Ă�������E��
		else
		{
			PickUpWeapon(gun);
		}
	}
	//�G���m�œ���������߂荞�܂Ȃ��悤�ɂ���
	else if (info.object_->GetObjName().find("enemy") != std::string::npos)
	{
		//����
		float length = (info.object_->GetScale().x + GetScale().x);
		//�����̃x�N�g��
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		//��
		distanceVec.y = 0;
		distanceVec.Normalized();
		//�ʒu�Z�b�g(���a�{���a�̒������x�N�g���̕������g���đ���)
		SetTrans(info.object_->GetTrans() + distanceVec * length * 1.001f);

		//���͓����Ȃ��悤�ɂ���
		SetVelocity({ GetVelocity().x,0,GetVelocity().z });
		info.object_->SetVelocity({ info.object_->GetVelocity().x,0,info.object_->GetVelocity().z });
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
