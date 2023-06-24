#include "Player.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"

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

void Player::Move()
{
	//�L�[����
	KeyboardInput* input = &KeyboardInput::GetInstance();

	//�J�����擾�i�؂�Ă邾���j
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	//�}�E�X�̓����ŃJ�����p�x�ύX
	Vec2 vel = MouseInput::GetInstance().GetCursorVelocity();
	//��]
	Vec3 rotMove = {
		vel.y_ * ANGLE_VEL_EXTEND_,
		vel.x_ * ANGLE_VEL_EXTEND_,
		0
	};
	//�p�x�𑫂�
	SetRot(GetRot() + rotMove);
	SetRotX(min(GetRot().x_, PI / 2.0f * 0.9f));
	SetRotX(max(GetRot().x_, -PI / 2.0f * 0.9f));

	//�ړ��x�N�g����Y������̊p�x�ŉ�]
	Vec3 frontVec = { 0,0,1.0f };
	Vec3 upVec = { 0,1.0f,0 };
	Vec3 rightVec = { 0,0,0 };
	CulcWorldMat();
	//��]
	Vec3xM4(frontVec, GetMatWorld(), 0);

	//�J�����̉E�����x�N�g�����o��
	rightVec = upVec.Cross(frontVec);
	//����������������̂�
	//upVec = frontVec.Cross(rightVec);

	//�J�����̏�����Z�b�g
	//camera->SetUp(upVec);

	//�����Ă�����Ɉړ�
	if (input->KeyPush(DIK_UPARROW) || input->KeyPush(DIK_W) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		SetTrans({ GetTrans().x_ + frontVec.x_, GetTrans().y_/* + frontVec.y_*/, GetTrans().z_ + frontVec.z_ });
	}
	if (input->KeyPush(DIK_DOWNARROW) || input->KeyPush(DIK_S) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		SetTrans({ GetTrans().x_ - frontVec.x_, GetTrans().y_ /* - frontVec.y_*/, GetTrans().z_ - frontVec.z_ });
	}
	if (input->KeyPush(DIK_LEFTARROW) || input->KeyPush(DIK_A) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		SetTrans({ GetTrans().x_ - rightVec.x_, GetTrans().y_/* - rightVec.y_*/, GetTrans().z_ - rightVec.z_ });
	}
	if (input->KeyPush(DIK_RIGHTARROW) || input->KeyPush(DIK_D) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		SetTrans({ GetTrans().x_ + rightVec.x_, GetTrans().y_/* + rightVec.y_*/, GetTrans().z_ + rightVec.z_ });
	}

	//�J�������v���C���[�Ɠ����ʒu��
	camera->SetEye(GetTrans());

	//�J�����̒����_�ɉ�]�����x�N�g���Z�b�g
	camera->SetTarget(GetTrans() + frontVec);
}

bool Player::Initialize()
{
	if (!Object::Initialize())
	{
		return false;
	}

	//�R���C�_�[�̒ǉ�
	float radius = 0.6f;
	//���a�������������畂�������W�����̒��S�ɂ���

	SetCollider(std::make_unique<SphereCollider>(XMVECTOR({ 0,radius,0,0 }), radius));

	//�J�����̈ʒu�ƍ��킹��
	SetTrans(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());

	return true;
}

void Player::Update()
{
	Move();

	Object::Update();
}


void Player::OnCollision(const CollisionInfo& info)
{
	for (int32_t i = 0; i < 1; ++i)
	{
		const float MD_VEL = 0.5f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.y = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.z = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

		ParticleManager::GetInstance()->Add(20, XMFLOAT3(info.inter_.m128_f32), vel, XMFLOAT3(), 0, 1.0f);
	}
}
