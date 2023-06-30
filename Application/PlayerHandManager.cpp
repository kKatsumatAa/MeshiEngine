#include "PlayerHandManager.h"
#include "Player.h"
#include "CollisionManager.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerAttackState.h"



void PlayerHandManager::Initialize(Player* player)
{
	handL.reset();
	handR.reset();

	//�ʒu(���[�J�����W���Z�b�g)
	Vec3 playerPos = player->GetTrans();
	Vec3 distance = player->GetScale();
	//�����i���j
	handL = handL->Create(player, { -distance.x_ * 2.0f,0 - distance.y_,-distance.z_ });
	handR = handR->Create(player, { +distance.x_ * 2.0f,0 - distance.y_,-distance.z_ });

	player_ = player;
}

void PlayerHandManager::HandAttack(PlayerHand* hand, const RaycastHit& info)
{
	if (hand == nullptr)
	{
		return;
	}

	//�N���b�N������U��
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) && info.collider->GetObject3d()->GetObjName() == "enemy")
	{
		//�Փ˓_�܂ł̋���
		Vec3 lengthV = Vec3(info.inter.m128_f32[0], info.inter.m128_f32[1], info.inter.m128_f32[2]) - player_->GetTrans();
		hand->SetInterLength(lengthV.GetLength());

		//�X�e�[�g�ύX
		hand->ChangeAttackState(std::make_unique<PlayerAttackStateDoing>());

		//�X�e�[�g���ŌĂяo��
		std::function<void()>f = [=]() {
			//�����蔻��Ăяo�������ŁA�v���C���[�̈ʒu�����g���A�G�ɍU����e���̏����������邽��
			CollisionInfo c = CollisionInfo(player_, player_->GetCollider(), info.inter);
			c.object_->SetObjName("playerAttack");
			info.collider->OnCollision(c);
			c.object_->SetObjName("player");
		};
		//�G�̔�e�����Z�b�g
		hand->GetAttackState()->SetEnemyDamageFunc(f);

		//�Q�[���̃X�s�[�h(��)
		GameVelocityManager::GetInstance().AddGameVelocity(0.5f);
	}
}

bool PlayerHandManager::GetIsUseWitchHand()
{
	//��̂ǂ��炩���g������U��
	if (!handR->GetIsAttacking() || !handL->GetIsAttacking())
	{
		return true;
	}

	return false;
}

PlayerHand* PlayerHandManager::GetWitchUseHand()
{
	if (!handR->GetIsAttacking())
	{
		return handR.get();
	}
	if (!handL->GetIsAttacking())
	{
		return handL.get();
	}

	return nullptr;
}

void PlayerHandManager::Update()
{
	//�U���ł��邩�ǂ���
//���C�Ƀv���C���[�̐��ʃx�N�g�������
	Ray ray;
	ray.dir = { player_->GetFrontVec().x_,player_->GetFrontVec().y_,player_->GetFrontVec().z_ };
	ray.start = { player_->GetTrans().x_,player_->GetTrans().y_,player_->GetTrans().z_ };

	RaycastHit info;
	//���ʃx�N�g���ɓG�����邩
	bool isRayHit = CollisionManager::GetInstance()->Raycast(ray, "player", &info, player_->GetAttackLength());

	if (isRayHit)
	{
		//�U���ł���t���O�I��
		player_->SetIsCanAttack(true);

		//��̂ǂ��炩���g������U��
		if (GetIsUseWitchHand())
		{
			//�U��
			HandAttack(GetWitchUseHand(), info);
		}
	}
	//�U���ł���t���O�I�t
	else
	{
		player_->SetIsCanAttack(false);
	}


	//��
	handL->Update();
	handR->Update();
}

void PlayerHandManager::Draw()
{
	handL->Draw();
	handR->Draw();
}
