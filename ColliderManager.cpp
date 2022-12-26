#include "ColliderManager.h"
#include "Util.h"


void ColliderManager::Initialize()
{
	colliders_.clear();

	this->audio = &Sound::GetInstance();
}

void ColliderManager::Update(Player* player, EnemyManager* enemyM, PlayerBulletManager* playerBulletM, ItemManager* itemM, Stage* stage)
{
	//bullet�͂��ꎩ�̂�list�Ȃ̂œ���
	std::list<std::unique_ptr<Enemy>>& enemies = enemyM->enemies;
	std::list<std::unique_ptr<PlayerBullet>>& bullets = playerBulletM->playerBullets_;

	//�X�e�[�W�Ƃ̓����蔻��
	{
		bool demo;
		//player
		stage->CollisionMap(player, player->GetIsGround(), demo);
		//�G
		for (std::unique_ptr<Enemy>& enemy : enemies)
		{
			stage->CollisionMap(enemy.get(), demo, demo);
		}
		//�e
		for (std::unique_ptr<PlayerBullet>& bullet : bullets)
		{
			stage->CollisionMap(bullet.get(), demo, bullet->GetIsDead(), true);
		}
	}


	//�G��player�̂�
	for (std::unique_ptr<Enemy>& enemy : enemyM->enemies)
	{
		if (CollisionCircleCircle(player->GetWorldPos(), player->GetRadius(), enemy->GetWorldPos(), enemy->GetRadius()))
		{
			//player���G�̐Ԃ��Ƃ������ɂ�����
			if (enemy.get()->GetWorldPos().y - enemy.get()->GetRadius() + (2 * enemy.get()->GetRadius() * enemy.get()->GetEnemyRedRate())
				<= player->GetWorldPos().y)
			{
				player->OnCollision2(*enemy.get());
				enemy.get()->OnCollision(*player);
			}
			else
			{
				player->OnCollision(*enemy.get());
			}
		}
	}

	//�A�C�e����player�̂�
	for (std::unique_ptr<Item>& item : itemM->items_)
	{
		if (CollisionCircleCircle(player->GetWorldPos(), player->GetRadius(), item->GetWorldPos(), item->GetRadius()))
		{
			//player�ɒe�̎�ނ��Z�b�g
			player->SetBulletType(item.get()->GetItemType());
			//item
			item.get()->OnCollision(*player);
		}
	}

	//�e�Ƃ��Ƃ̓����蔻��
	ClearList();
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		SetListCollider(enemy.get());
	}
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		SetListCollider(bullet.get());
	}

	CheckAllCollisions();
}

//---------------------------------------------------------------------------------------------
void ColliderManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	if ((colliderA->GetIsDead() || colliderB->GetIsDead()))
	{
		return;//����A�Փˏ�������������
	}


	Vec3 posA = colliderA->GetWorldPos();
	Vec3 posB = colliderB->GetWorldPos();

	float rA = colliderA->GetRadius();
	float rB = colliderB->GetRadius();

	if (CollisionCircleCircle(posA, rA, posB, rB))
	{
		//enemy���m�̔���
		if (colliderA->GetCollisionAttribute() & colliderB->GetCollisionAttribute())
		{
			colliderA->OnCollision2(*colliderB);
			colliderB->OnCollision2(*colliderA);
		}
		//�G�ƒe
		if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionAttribute()))
		{
			colliderA->OnCollision(*colliderB);
			colliderB->OnCollision(*colliderA);
		}
	}
}

void ColliderManager::CheckAllCollisions()
{
	//���X�g���̃y�A�𑍓�����
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{
		//itrB��itrA�̎��̗v�f����񂷁i�d�����������j
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB)
		{
			CheckCollisionPair(*itrA, *itrB);
		}
	}
}

void ColliderManager::SetListCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void ColliderManager::ClearList()
{
	colliders_.clear();
}
