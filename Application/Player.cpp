#include "Player.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerAttackState.h"


using namespace DirectX;


std::unique_ptr<Player> Player::Create(std::unique_ptr<WorldMat> worldMat)
{
	std::unique_ptr<Player> instance = std::make_unique<Player>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//������
	if (!instance->Initialize(std::move(worldMat)))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Player::Initialize(std::unique_ptr<WorldMat> worldMat)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}

	SetObjName("player");

	//�R���C�_�[�̒ǉ�
	float radius = 0.6f;
	//���a�������������畂�������W�����̒��S�ɂ���

	SetCollider(std::make_unique<SphereCollider>(XMVECTOR({ 0,radius,0,0 }), radius));

	//�J�����̈ʒu�ƍ��킹��
	SetTrans(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());

	//��
	handManager_.reset();
	handManager_ = std::make_unique<PlayerHandManager>();
	handManager_->Initialize(this);

	return true;
}


void Player::DirectionUpdate()
{
	//�J�����擾�i�؂�Ă邾���j
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	//�}�E�X�̓����ŃJ�����p�x�ύX
	Vec2 vel = MouseInput::GetInstance().GetCursorVelocity();

	//�}�E�X�̓����ŃQ�[���X�s�[�h�𑫂�
	GameVelocityManager::GetInstance().AddGameVelocity(vel.GetLength(), "mouse");

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

	{
		//���ƂȂ鐳�ʃx�N�g������v���C���[�̕����x�N�g���ւ̉�]�N�H�[�^�j�I��
		Quaternion q = Quaternion::DirectionToDirection(GetFrontVecTmp(), frontVec_);
		//�p�x����Ȃ��čs������̂܂܎g��
		SetIsUseQuaternionMatRot(true);
		SetMatRot(q.MakeRotateMatrix());
	}


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

	//�Q�[���X�s�[�h���ړ��ő���
	GameVelocityManager::GetInstance().AddGameVelocity(velocity_.GetLength() * 10.0f);

	//�ʒu�Z�b�g(�Q�[���X�s�[�h��������)
	SetTrans(GetTrans() + velocity_ * GameVelocityManager::GetInstance().GetVelocity());

	//�J�������v���C���[�Ɠ����ʒu��
	camera->SetEye(GetTrans());

	//�J�����̒����_�ɉ�]�����x�N�g���Z�b�g
	camera->SetTarget(GetTrans() + frontVec_);
}

void Player::Update()
{
	//���ŏ���������
	//SetScale({ 0,0,0 });

	//�J�����̌����ύX
	DirectionUpdate();

	//�ړ�
	Move();

	Object::Update();

	//��̃A�b�v�f�[�g
	handManager_->Update();
}

void Player::Draw()
{
	XMFLOAT4 col = { 1.0f,1.0f,1.0f,0.9f };

	if (isCanAttack_)
	{
		col = { 1.0f,0,0,1.0f };
	}

	Object::DrawBoxSprite({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f, WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f },
		0.05f, col, TextureManager::GetInstance().sWhiteTexHandle_, { 0.5f,0.5f });

	//��̕`��
	handManager_->Draw();
}

void Player::OnCollision(const CollisionInfo& info)
{
	if (info.object_->GetObjName() == "enemy")
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
		Vec3 ansPosE = GetTrans() - distanceVec * length * 1.01f;
		//SetTrans(ansPosP);
		info.object_->SetTrans(ansPosE);
		
		//info.object_->SetTrans(ansPos);
		info.object_->SetVelocity({ 0,0,0 });
	}
}
