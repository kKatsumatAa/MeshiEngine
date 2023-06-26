#include "Player.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

using namespace DirectX;


std::unique_ptr<Player> Player::Create()
{
	std::unique_ptr<Player> instance = std::make_unique<Player>();
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

bool Player::Initialize()
{
	if (!Object::Initialize())
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

	return true;
}

void Player::Move()
{
	//�L�[����
	KeyboardInput* input = &KeyboardInput::GetInstance();

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
	rotWorldMat.rot_ = (rotWorldMat.rot_ + rotMove);
	rotWorldMat.rot_.x_ = (min(rotWorldMat.rot_.x_, PI / 2.0f * 0.9f));
	rotWorldMat.rot_.x_ = (max(rotWorldMat.rot_.x_, -PI / 2.0f * 0.9f));

	//�ړ��x�N�g����Y������̊p�x�ŉ�]
	Vec3 frontVec = { 0,0,1.0f };
	Vec3 upVec = { 0,1.0f,0 };
	Vec3 rightVec = { 0,0,0 };
	rotWorldMat.CulcWorldMat();
	//��]
	Vec3xM4(frontVec, rotWorldMat.matWorld_, 0);

	//�J�����̉E�����x�N�g�����o��
	rightVec = upVec.Cross(frontVec);
	//����������������̂�
	//upVec = frontVec.Cross(rightVec);

	//�J�����̏�����Z�b�g
	//camera->SetUp(upVec);


	Vec3 velocity_ = { 0,0,0 };
	//�����Ă�����Ɉړ�
	if (input->KeyPush(DIK_UPARROW) || input->KeyPush(DIK_W) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		velocity_ += { frontVec.x_, 0, frontVec.z_ };
	}
	if (input->KeyPush(DIK_DOWNARROW) || input->KeyPush(DIK_S) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		velocity_ += { -frontVec.x_, 0, -frontVec.z_ };
	}
	if (input->KeyPush(DIK_LEFTARROW) || input->KeyPush(DIK_A) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		velocity_ += { -rightVec.x_, 0, -rightVec.z_ };
	}
	if (input->KeyPush(DIK_RIGHTARROW) || input->KeyPush(DIK_D) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		velocity_ += { rightVec.x_, 0, rightVec.z_ };
	}

	//�Q�[���X�s�[�h���ړ��ő���
	GameVelocityManager::GetInstance().AddGameVelocity(velocity_.GetLength() * 10.0f);

	//�ʒu�Z�b�g(�Q�[���X�s�[�h��������)
	SetTrans(GetTrans() + velocity_ * GameVelocityManager::GetInstance().GetVelocity());

	//�J�������v���C���[�Ɠ����ʒu��
	camera->SetEye(GetTrans());

	//�J�����̒����_�ɉ�]�����x�N�g���Z�b�g
	camera->SetTarget(GetTrans() + frontVec);
}

void Player::Update()
{
	//���ŏ���������
	SetScale({ 0,0,0 });

	Move();

	Object::Update();
}


void Player::OnCollision(const CollisionInfo& info)
{
	for (int32_t i = 0; i < 1; ++i)
	{
		const float MD_VEL = 0.5f;
		Vec3 vel{};
		vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

		Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

		ParticleManager::GetInstance()->Add(180, pos, vel, { 0,0,0 }, 1.0f, 0.0f);
	}
}
