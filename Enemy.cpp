#include "Enemy.h"
#include "Util.h"
#include "ParticleManager.h"

const float Enemy::pLength = 50.0f;


void Enemy::ChangeState(EnemyState* state)
{
	delete this->state;
	this->state = state;
	state->SetEnemy(this);
}

void Enemy::Initialize(Model* model, float enemyRedRate, Player* player/*, EffectManager* effectM*//*, Tutorial* tutorial*/)
{
	assert(model);

	model_ = model;
	this->player = player;

	isDead = false;

	this->audio = &Sound::GetInstance();

	//this->tutorial = tutorial;

	this->enemyRedRate = enemyRedRate;

	worldTransform_.trans = { 0,0.0f,0 };
	worldTransform_.rot = { 0,0.0f,0 };
	worldTransform_.scale = { scaleTmp,scaleTmp,scaleTmp };
	worldTransform_.SetWorld();

	HPp = hptmp;
	radius_ = scaleTmp;

	velocity = { 0,0,0 };

	//衝突属性
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionAttributePlayer);


	ChangeState(new NoAttackE);
}

void Enemy::Update()
{
	////チュートリアル
	//if (tutorial != nullptr)
	//{
	//	isDead = false;
	//}

	if (count > 0) {
		count--;
	}
	if (count == 0)
	{
		color = { 1.0f,1.0f,1.0f,1.0f };
	}

	//移動制限
	if (worldTransform_.trans.x <= -movingMax)worldTransform_.trans.x = -movingMax;
	if (worldTransform_.trans.x >= movingMax)worldTransform_.trans.x = movingMax;

	state->Update();

	worldTransform_.SetWorld();
}

void Enemy::Draw(ViewMat& view, ProjectionMat& projection)
{
	state->Draw(view, projection, model_);
}

void Enemy::DrawSprite()
{
	//gaugeS->Draw();
}


//--------------------------------------------------------------------------------------
void Enemy::OnCollision(Collider& collider)
{
	HPp--;
	//effect
	ParticleManager::GetInstance()->GenerateRandomParticle(5, 20, 1.5f, GetWorldPos(), 1.0f, 0);

	//
	SetVelocity(collider.GetVelocity() * 3.0f);

	//色変える
	color = { 0.0f,0.0f,1.0f,1.0f };
	count = countMax;

	if (HPp <= 0)
	{
		isDead = true;
		HPp = 0;
	}

	//音


}

void Enemy::OnCollision2(Collider& collider)
{
	//敵同士で重ならないように
	Vec3 vec = collider.GetWorldPos() - GetWorldPos();
	vec.Normalized();

	SetVelocity(-vec);
}


//--------------------------------------------------------------------------------------

void EnemyState::SetEnemy(Enemy* Enemy)
{
	this->enemy = Enemy;
}

//--------------------------------------------------------------------------------------
void NoAttackE::Update()
{
	Vec3 lengthV = enemy->player->GetWorldPos() - enemy->GetWorldPos();

	if (lengthV.GetLength() < Enemy::pLength)
	{
		enemy->ChangeState(new AttackE);
	}
}

void NoAttackE::Draw(ViewMat& view, ProjectionMat& projection, Model* model)
{
	enemy->draw[0].DrawModel(enemy->GetWorldTransForm(), &view, &projection, model, enemy->color);
}


//--------------------------------------------------------------------------------------
void AttackE::Update()
{
	Vec3 lengthV = enemy->player->GetWorldPos() - enemy->GetWorldPos();

	if (lengthV.GetLength() < Enemy::pLength)
	{
		//playerに向かう
		Vec3 v = enemy->GetVelocity() * 5.0f + lengthV * 0.1f;
		enemy->SetVelocity(v.GetNormalized() * 0.3f);
	}
}

void AttackE::Draw(ViewMat& view, ProjectionMat& projection, Model* model)
{
	enemy->draw[1].DrawModel(enemy->GetWorldTransForm(), &view, &projection, model, enemy->color);
}
