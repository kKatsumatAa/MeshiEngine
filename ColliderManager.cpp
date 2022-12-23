#include "ColliderManager.h"
#include "Util.h"


void ColliderManager::Initialize()
{
	colliders_.clear();

	this->audio = &Sound::GetInstance();
}

void ColliderManager::Update(Player* player, EnemyManager* enemyM, PlayerBulletManager* playerBulletM, Stage* stage)
{
	//bullet‚Í‚»‚ê©‘Ì‚ªlist‚È‚Ì‚Å“Á•Ê
	std::list<std::unique_ptr<Enemy>>& enemies = enemyM->enemies;
	std::list<std::unique_ptr<PlayerBullet>>& bullets = playerBulletM->playerBullets_;

	//ƒXƒe[ƒW‚Æ‚Ì“–‚½‚è”»’è
	{
		//player
		stage->CollisionMap(player, player->GetIsGround());
		//“G
		bool demo;
		for (std::unique_ptr<Enemy>& enemy : enemies)
		{
			stage->CollisionMap(enemy.get(), demo);
		}
		//’e
		for (std::unique_ptr<PlayerBullet>& bullet : bullets)
		{
			stage->CollisionMap(bullet.get(), bullet->GetIsDead(), true);
		}
	}


	//“G‚Æplayer‚Ì‚İ
	for (std::unique_ptr<Enemy>& enemy : enemyM->enemies)
	{
		if (CollisionCircleCircle(player->GetWorldPos(), player->GetRadius(), enemy->GetWorldPos(), enemy->GetRadius()))
		{
			//player‚ª“G‚ÌÔ‚¢‚Æ‚±‚ë‚æ‚èã‚É‚¢‚½‚ç
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

	//’e‚Æ‚©‚Æ‚Ì“–‚½‚è”»’è
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
		return;//”»’èAÕ“Ëˆ—‚¹‚¸”²‚¯‚é
	}


	Vec3 posA = colliderA->GetWorldPos();
	Vec3 posB = colliderB->GetWorldPos();

	float rA = colliderA->GetRadius();
	float rB = colliderB->GetRadius();

	if (CollisionCircleCircle(posA, rA, posB, rB))
	{
		//enemy“¯m‚Ì”»’è
		if (colliderA->GetCollisionAttribute() & colliderB->GetCollisionAttribute())
		{
			colliderA->OnCollision2(*colliderB);
			colliderB->OnCollision2(*colliderA);
		}
		//“G‚Æ’e
		if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionAttribute()))
		{
			colliderA->OnCollision(*colliderB);
			colliderB->OnCollision(*colliderA);
		}
	}
}

void ColliderManager::CheckAllCollisions()
{
	//ƒŠƒXƒg“à‚ÌƒyƒA‚ğ‘“–‚½‚è
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{
		//itrB‚ÍitrA‚ÌŸ‚Ì—v‘f‚©‚ç‰ñ‚·id•¡”»’è‚ğ‰ñ”ğj
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
