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


Player::~Player()
{
	Character::~Character();
}

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
	if (!ObjectFBX::Initialize(std::move(worldMat)))
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

	//��]�p�̊p�x�̊�
	cameraRot_ = GetRot();

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
		-vel.y * ANGLE_VEL_EXTEND_,
		vel.x * ANGLE_VEL_EXTEND_,
		0
	};
	//�p�x�𑫂�
	cameraRot_ = (cameraRot_ + rotMove);
	cameraRot_.x = (min(cameraRot_.x, PI / 2.0f * 0.9f));
	cameraRot_.x = (max(cameraRot_.x, -PI / 2.0f * 0.9f));

	frontVec_ = GetFrontVecTmp();
	//��]
	frontVec_ = GetTurnVec3UseQuaternionAndRot(frontVec_, cameraRot_);

	//���ʃx�N�g���Z�b�g
	SetFrontVec(frontVec_);

	//�p�x
	SetRot(cameraRot_);


	//�J�����̉E�����x�N�g�����o��
	rightVec_ = upVec_.Cross(frontVec_);
}

void Player::Move()
{
	//�L�[����
	KeyboardInput* input = &KeyboardInput::GetInstance();

	//�J�����擾�i�؂�Ă邾���j
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	Vec3 velocity_ = { 0,0,0 };
	//�����Ă�����Ɉړ�
	if (input->KeyPush(DIK_UPARROW) || input->KeyPush(DIK_W) || PadInput::GetInstance().GetLeftStickTilt().y < 0)
	{
		velocity_ += { frontVec_.x, 0, frontVec_.z };
	}
	if (input->KeyPush(DIK_DOWNARROW) || input->KeyPush(DIK_S) || PadInput::GetInstance().GetLeftStickTilt().y > 0)
	{
		velocity_ += { -frontVec_.x, 0, -frontVec_.z };
	}
	if (input->KeyPush(DIK_LEFTARROW) || input->KeyPush(DIK_A) || PadInput::GetInstance().GetLeftStickTilt().y < 0)
	{
		velocity_ += { -rightVec_.x, 0, -rightVec_.z };
	}
	if (input->KeyPush(DIK_RIGHTARROW) || input->KeyPush(DIK_D) || PadInput::GetInstance().GetLeftStickTilt().y > 0)
	{
		velocity_ += { rightVec_.x, 0, rightVec_.z };
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
	ObjectFBX::WorldMatColliderUpdate();

	//�J�������v���C���[�Ɠ����ʒu��
	camera->SetEye(GetTrans());

	//�J�����̒����_�ɉ�]�����x�N�g���Z�b�g
	camera->SetTarget(GetTrans() + frontVec_);
}

void Player::Update()
{
	//�N���b�N���O���ō��N���b�N�����������Ƃ��Ƀt���O���Ă�
	isClickLeft_ = (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) || isClickLeft_);

	//�f���e�Ȃǂ̃X�e�[�g
	state_->Update();

	Character::Update();

	//��̃A�b�v�f�[�g
	handManager_->Update();

	//�I�t
	isClickLeft_ = false;
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

void Player::Dead(const CollisionInfo& info)
{
	//hp��0�ɂȂ�����
	isDead_ = true;

	//����폜
	handManager_->DeleteHands();

	Bullet* bullet = dynamic_cast<Bullet*>(info.object_);

	//���o�p�ɒe�������G�̈ʒu�ۑ�
	bulletOwnerEnemyPos_ = bullet->GetOwnerPos();

	//�X�e�[�g��ύX
	ChangePlayerState(std::make_unique<PlayerStateDeadEffect>());
}

void Player::ThrowWeapon()
{
	FallWeapon(GetFrontVec() * FALL_VEL_POW_ + Vec3(0, 0.2f, 0));

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
