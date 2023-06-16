#include "Player.h"
#include "KeyboardInput.h"
#include "DebugText.h"

#include "ParticleManager.h"
#include "PadInput.h"

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

	//�R���C�_�[�̒ǉ�
	float radius = 0.6f;
	//���a�������������畂�������W�����̒��S�ɂ���

	SetCollider(std::make_unique<SphereCollider>(XMVECTOR({ 0,radius,0,0 }), radius));

	return true;
}

void Player::Update()
{
	KeyboardInput* input = &KeyboardInput::GetInstance();

	if (input->KeyPush(DIK_LEFTARROW) || PadInput::GetInstance().GetRightStickTilt().x_ < 0)
	{
		SetRot({ GetRot().x_,GetRot().y_ - AngletoRadi(2.0f), GetRot().z_ });
	}
	if (input->KeyPush(DIK_RIGHTARROW) || PadInput::GetInstance().GetRightStickTilt().x_ > 0)
	{
		SetRot({ GetRot().x_,GetRot().y_ + AngletoRadi(2.0f), GetRot().z_ });
	}

	//�ړ��x�N�g����Y������̊p�x�ŉ�]
	Vec3 move = { 0,0,1.0f };
	CulcWorldMat();
	Vec3xM4(move, GetMatWorld(), 0);

	//�����Ă�����Ɉړ�
	if (input->KeyPush(DIK_UPARROW) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		SetTrans({ GetTrans().x_ - move.x_, GetTrans().y_, GetTrans().z_ });
		SetTrans({ GetTrans().x_ ,GetTrans().y_ - move.y_, GetTrans().z_ });
		SetTrans({ GetTrans().x_ ,GetTrans().y_, GetTrans().z_ - move.z_ });
	}
	if (input->KeyPush(DIK_DOWNARROW) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		SetTrans({ GetTrans().x_ + move.x_, GetTrans().y_, GetTrans().z_ });
		SetTrans({ GetTrans().x_ ,GetTrans().y_ + move.y_, GetTrans().z_ });
		SetTrans({ GetTrans().x_ ,GetTrans().y_, GetTrans().z_ + move.z_ });
	}

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
