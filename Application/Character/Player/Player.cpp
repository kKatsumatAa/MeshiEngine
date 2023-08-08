#include "Player.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerAttackState.h"
#include "PlayerState.h"
#include "Bullet.h"


using namespace DirectX;


std::unique_ptr<Player> Player::Create(std::unique_ptr<WorldMat> worldMat, Weapon* weapon)
{
	std::unique_ptr<Player> instance = std::make_unique<Player>();
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

bool Player::Initialize(std::unique_ptr<WorldMat> worldMat, Weapon* weapon)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}

	//�J�����̈ʒu�ƍ��킹��
	SetTrans(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());

	//��
	handManager_.reset();
	handManager_ = std::make_unique<PlayerHandManager>();
	handManager_->Initialize(this);

	//hp
	hp_ = HP_TMP_;

	if (weapon)
	{
		weapon_ = weapon;
		//����̏��X�ݒ�
		PickUpWeapon(weapon_);
	}

	//���펝���Ă���X�e�[�g������
	if (weapon_ == nullptr)
	{
		ChangePlayerState(std::make_unique<PlayerStateBareHands>());
	}
	else
	{
		ChangePlayerState(std::make_unique<PlayerStateHaveWeapon>());
	}
	return true;
}

void Player::ChangePlayerState(std::unique_ptr<PlayerState> state)
{
	state_.reset();
	state_ = std::move(state);
	state_->SetPlayer(this);
	state_->Initialize();
}

void Player::DirectionUpdate()
{
	//�J�����擾�i�؂�Ă邾���j
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	//�}�E�X�̓����ŃJ�����p�x�ύX
	Vec2 vel = MouseInput::GetInstance().GetCursorVelocity() * MOUSE_VELOCITY_TMP_;

	//�}�E�X�̓����ŃQ�[���X�s�[�h�𑫂�
	GameVelocityManager::GetInstance().AddGameVelocity(vel.GetLength() * MOUSE_GAME_VEL_EXTEND_, "mouse");

	//��]
	Vec3 rotMove = {
		vel.y_ * ANGLE_VEL_EXTEND_,
		vel.x_ * ANGLE_VEL_EXTEND_,
		0
	};
	//�p�x�𑫂�
	cameraRot_ = (cameraRot_ + rotMove);
	cameraRot_.x_ = (min(cameraRot_.x_, PI / 2.0f * 0.9f));
	cameraRot_.x_ = (max(cameraRot_.x_, -PI / 2.0f * 0.9f));

	frontVec_ = GetFrontVecTmp();
	//��]
	frontVec_ = GetTurnVec3UseQuaternionAndRot(frontVec_, cameraRot_);

	//���ʃx�N�g���Z�b�g
	SetFrontVec(frontVec_);

	//�p�x
	SetRot(cameraRot_);


	//�J�����̉E�����x�N�g�����o��
	rightVec_ = upVec_.Cross(frontVec_);
	//����������������̂�
	//upVec = frontVec_.Cross(rightVec);

	//�J�����̏�����Z�b�g
	//camera->SetUp(upVec);
}

void Player::Move()
{
	//�L�[����
	KeyboardInput* input = &KeyboardInput::GetInstance();

	//�J�����擾�i�؂�Ă邾���j
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	Vec3 velocity_ = { 0,0,0 };
	//�����Ă�����Ɉړ�
	if (input->KeyPush(DIK_UPARROW) || input->KeyPush(DIK_W) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		velocity_ += { frontVec_.x_, 0, frontVec_.z_ };
	}
	if (input->KeyPush(DIK_DOWNARROW) || input->KeyPush(DIK_S) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		velocity_ += { -frontVec_.x_, 0, -frontVec_.z_ };
	}
	if (input->KeyPush(DIK_LEFTARROW) || input->KeyPush(DIK_A) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		velocity_ += { -rightVec_.x_, 0, -rightVec_.z_ };
	}
	if (input->KeyPush(DIK_RIGHTARROW) || input->KeyPush(DIK_D) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		velocity_ += { rightVec_.x_, 0, rightVec_.z_ };
	}

	//�Q�[���X�s�[�h���ړ��ő���(�W�����v���ŃX�y�[�X�����ĂȂ����)
	if (!(!isOnGround_ && KeyboardInput::GetInstance().KeyPush(DIK_SPACE)))
	{
		GameVelocityManager::GetInstance().AddGameVelocity(velocity_.GetLength() * MOVE_ADD_VEL_EXTEND_);
	}

	//�W�����v���ŃX�y�[�X�������ςȂ���������
	if (!isOnGround_ && KeyboardInput::GetInstance().KeyPush(DIK_SPACE) && velocity_.GetLength())
	{
		GameVelocityManager::GetInstance().AddGameVelocity(-0.001f);
	}

	//�ʒu�Z�b�g(�Q�[���X�s�[�h��������)
	SetTrans(GetTrans() + velocity_ * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity());

	//�n�ʂƂ̔���
	std::function<void()>gameSpeedAddFunc = [=]() {GameVelocityManager::GetInstance().AddGameVelocity(1.0f); };
	OnGroundAndWallUpdate(4.0f, GameVelocityManager::GetInstance().GetVelocity(), KeyboardInput::GetInstance().KeyPush(DIK_SPACE), gameSpeedAddFunc);

	//�R���C�_�[�X�V
	Object::WorldMatColliderUpdate();

	//�J�������v���C���[�Ɠ����ʒu��
	camera->SetEye(GetTrans());

	//�J�����̒����_�ɉ�]�����x�N�g���Z�b�g
	camera->SetTarget(GetTrans() + frontVec_);
}

void Player::Update()
{
	//�Q�[���I�[�o�[�܂ł�
	if (isDead_)
	{
		SetScale({ 0,0,0 });

		if (deadTimer_ <= 0)
		{
			FallWeapon({ 0,0,0 });
			SetIsAlive(false);
		}

		deadTimer_--;

		//�p�[�e�B�N��
		for (int32_t i = 0; i < 1; ++i)
		{
			const float MD_VEL = 0.6f;
			Vec3 vel{};
			vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

			float scale = (float)rand() / RAND_MAX * GetScale().x_;

			ParticleManager::GetInstance()->Add(30, GetTrans(), vel, { 0,0,0 }, scale, 0, { 2.0f,2.0f,2.0f,1.5f }, { 0,0,0,0.0f });
		}

		return;
	}

	//�J�����̌����ύX
	DirectionUpdate();

	//�ړ�
	Move();

	//�f���e�Ȃǂ̃X�e�[�g
	state_->Update();

	Object::Update();

	//��̃A�b�v�f�[�g
	handManager_->Update();
}

void Player::Draw()
{
	if (isDead_)
	{
		return;
	}

	//��̕`��
	handManager_->Draw();
}

void Player::DrawImGui()
{
	ImGui::Begin("Player");
	ImGui::Checkbox("ValidDamage", &isValidDamage_);
	ImGui::End();
}


void Player::Dead(const CollisionInfo& info)
{
	//hp��0�ɂȂ�����
	isDead_ = true;

	Bullet* bullet = dynamic_cast<Bullet*>(info.object_);

	////�e�̕����ւ̃x�N�g��
	Vec3 directionVec;
	//���L�҂��܂������Ă�����
	//if (!bullet->GetOwner()->GetIsAlive())
	{
		directionVec = bullet->GetOwnerPos() - GetTrans();
	}

	//�������ʃx�N�g���ƃv���C���[�ւ̃x�N�g��
	Vec3 fVTmp = GetFrontVecTmp().GetNormalized();
	Vec3 pDVTmp = directionVec.GetNormalized();

	//���ʃx�N�g�������e�����x�N�g���ւ̉�]�N�H�[�^�j�I��
	Quaternion q = Quaternion::DirectionToDirection(fVTmp, pDVTmp);
	//��]��̃x�N�g��
	fVTmp = q.GetRotateVector(fVTmp);
	//���ʃx�N�g���Z�b�g
	SetFrontVec(fVTmp);

	//�p�x����Ȃ��čs������̂܂܎g��
	SetIsUseQuaternionMatRot(true);
	SetMatRot(q.MakeRotateMatrix());

	//�s��X�V
	Object::WorldMatColliderUpdate();

	//�J�����̒����_�ɉ�]�����x�N�g���Z�b�g
	CameraManager::GetInstance().GetCamera("playerCamera")->SetTarget(GetTrans() + fVTmp.GetNormalized());
	/*CameraManager::GetInstance().GetCamera("playerCamera")->Update();*/
}

void Player::ThrowWeapon()
{
	FallWeapon(GetFrontVec() * FALL_VEL_POW_);

	//�Q�[���X�s�[�h���Z
	GameVelocityManager::GetInstance().AddGameVelocity(0.9f);

	//�X�e�[�g�ύX(�f��)
	ChangePlayerState(std::make_unique<PlayerStateBareHands>());
}

void Player::OnCollision(const CollisionInfo& info)
{
	//�e�ɓ���������_���[�W
	if (info.object_->GetObjName() == "bullet")
	{
		//�_���[�W
		Damaged(hp_, [=]() { Dead(info); });
	}
}
