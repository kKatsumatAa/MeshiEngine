#include "PlayerHandManager.h"
#include "Player.h"
#include "CollisionManager.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerAttackState.h"
#include "CollisionAttribute.h"
#include "ObjectManager.h"
#include "CameraManager.h"



void PlayerHandManager::Initialize(Player* player)
{
	player_ = player;

	//�ʒu(���[�J�����W���Z�b�g)
	Vec3 playerPos = player->GetTrans();
	Vec3 distance = { 1.0f,1.0f,1.0f };
	//�����i���j
	ObjectManager::GetInstance().AddObject(OBJ_GROUP_NAME_,
		std::move(PlayerHand::Create(player, { -distance.x_ * 2.0f,0 - distance.y_,-distance.z_ }, HAND_R_NAME_)));
	ObjectManager::GetInstance().AddObject(OBJ_GROUP_NAME_,
		std::move(PlayerHand::Create(player, { +distance.x_ * 2.0f,0 - distance.y_,-distance.z_ }, HAND_L_NAME_)));

	handL_ = dynamic_cast<PlayerHand*>(*ObjectManager::GetInstance().GetObjs(OBJ_GROUP_NAME_, HAND_L_NAME_).begin());
	handR_ = dynamic_cast<PlayerHand*>(*ObjectManager::GetInstance().GetObjs(OBJ_GROUP_NAME_, HAND_R_NAME_).begin());
}

void PlayerHandManager::HandAttack(PlayerHand* hand, const RaycastHit& info)
{
	if (hand == nullptr)
	{
		return;
	}

	//�N���b�N������U��
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) && info.collider->GetObject3d()->GetObjName() == "enemy" && player_)
	{
		//�Փ˓_�܂ł̋���
		Vec3 lengthV = Vec3(info.inter.m128_f32[0], info.inter.m128_f32[1], info.inter.m128_f32[2]) - player_->GetTrans();
		hand->SetInterLength(lengthV.GetLength());

		//�X�e�[�g�ύX
		hand->ChangeAttackState(std::make_unique<PlayerAttackStateDoing>());

		//�X�e�[�g���ŌĂяo��
		std::function<void()>f = [=]() {
			//�G���e�œ|����Ă�\��������̂ł�����񒲂ׂ�
			//���C�Ƀv���C���[�̐��ʃx�N�g�������
			Ray ray;
			ray.dir = { player_->GetFrontVec().x_,player_->GetFrontVec().y_,player_->GetFrontVec().z_ };
			ray.start = { player_->GetTrans().x_,player_->GetTrans().y_,player_->GetTrans().z_ };

			//���ʃx�N�g���ɉ������邩
			RaycastHit info;
			uint16_t attribute = COLLISION_ATTR_ENEMYS | COLLISION_ATTR_ITEMS;
			bool isRayHit = CollisionManager::GetInstance()->Raycast(ray, attribute, &info, player_->GetAttackLength());

			if (isRayHit)
			{
				//�����蔻��Ăяo�������ŁA�v���C���[�̈ʒu�����g���A�G�ɍU����e���̏����������邽��
				c_ = CollisionInfo(player_, player_->GetCollider(), info.inter);
				c_.object_->SetObjName("playerAttack");
				info.collider->OnCollision(c_);
				c_.object_->SetObjName("player");

				//�J�����V�F�C�N
				CameraManager::GetInstance().GetCamera("playerCamera")->CameraShake(5, 1.05f);
			}
		};

		//�G�̔�e�����Z�b�g
		hand->GetAttackState()->SetEnemyDamageFunc(f);

		//�Q�[���̃X�s�[�h(��)
		GameVelocityManager::GetInstance().AddGameVelocity(1.0f);
	}
}

bool PlayerHandManager::GetIsUseWitchHand()
{
	//��̂ǂ��炩���g������U��
	if (!handR_->GetIsAttacking() || !handL_->GetIsAttacking())
	{
		return true;
	}

	return false;
}

PlayerHand* PlayerHandManager::GetWitchUseHand()
{
	if (!handR_->GetIsAttacking())
	{
		return handR_;
	}
	if (!handL_->GetIsAttacking())
	{
		return handL_;
	}

	return nullptr;
}

void PlayerHandManager::Attack(RaycastHit info)
{
	//��̂ǂ��炩���g������U��
	if (GetIsUseWitchHand())
	{
		//�U��
		HandAttack(GetWitchUseHand(), info);
	}
}

void PlayerHandManager::Update()
{
}

void PlayerHandManager::Draw()
{
}

void PlayerHandManager::DeleteHands()
{
	ObjectManager::GetInstance().ClearGroup(OBJ_GROUP_NAME_);
}
