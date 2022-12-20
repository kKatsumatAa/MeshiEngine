#include "ColliderManager.h"
#include "Util.h"


void ColliderManager::Initialize()
{
	colliders_.clear();

	this->audio = &Sound::GetInstance();
}

void ColliderManager::Update(Player* player, EnemyManager* enemyM, PlayerBulletManager* playerBulletM, Stage* stage)
{
	//敵とplayerのみ
	for (std::unique_ptr<Enemy>& enemy : enemyM->enemies)
	{
		if (CollisionCircleCircle(player->GetWorldPos(), player->GetRadius(), enemy->GetWorldPos(), enemy->GetRadius()))
		{
			//playerが敵の赤いところより上にいたら
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

	//弾とかとの当たり判定
	ClearList();
	//bulletはそれ自体がlistなので特別
	std::list<std::unique_ptr<Enemy>>& enemies = enemyM->enemies;
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		SetListCollider(enemy.get());
	}
	std::list<std::unique_ptr<PlayerBullet>>& bullets = playerBulletM->playerBullets_;
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		SetListCollider(bullet.get());
	}

	CheckAllCollisions();


	//ステージとの当たり判定
	{
		stage->CollisionMap(player->GetWorldPos(), player->GetVelocity(), player->GetRadius(), player->GetIsGround());
	}
}

//---------------------------------------------------------------------------------------------
void ColliderManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	if ((colliderA->GetIsDead() || colliderB->GetIsDead()))
	{
		return;//判定、衝突処理せず抜ける
	}


	Vec3 posA = colliderA->GetWorldPos();
	Vec3 posB = colliderB->GetWorldPos();

	float rA = colliderA->GetRadius();
	float rB = colliderB->GetRadius();

	if (CollisionCircleCircle(posA, rA, posB, rB))
	{
		//enemy同士の判定
		if (colliderA->GetCollisionAttribute() & colliderB->GetCollisionAttribute())
		{
			colliderA->OnCollision2(*colliderB);
		}
		//敵と弾
		else
		{
			colliderA->OnCollision(*colliderB);
			colliderB->OnCollision(*colliderA);
		}
	}
}

void ColliderManager::CheckAllCollisions()
{
	//リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{
		//itrBはitrAの次の要素から回す（重複判定を回避）
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
