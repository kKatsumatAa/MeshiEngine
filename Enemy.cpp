#include "Enemy.h"
#include "Util.h"

const float Enemy::pLength = 50.0f;


void Enemy::ChangeState(EnemyState* state)
{
	delete this->state;
	this->state = state;
	state->SetEnemy(this);
}

void Enemy::Initialize(Model* model, Player* player/*, EffectManager* effectM*//*, Tutorial* tutorial*/)
{
	assert(model);

	model_ = model;
	this->player = player;

	isDead = false;

	this->audio = &Sound::GetInstance();

	//this->tutorial = tutorial;


	worldTransform_.trans = { 0,0.0f,0 };
	worldTransform_.rot = { 0,0.0f,0 };
	worldTransform_.scale = { scaleTmp,scaleTmp,scaleTmp };
	worldTransform_.SetWorld();

	HPp = hptmp;
	radius_ = scaleTmp;

	velocity = { 0,0,0 };

	//�Փˑ���
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionAttributePlayer);


	ChangeState(new NoAttackE);
}

void Enemy::Update()
{
	////�`���[�g���A��
	//if (tutorial != nullptr)
	//{
	//	isDead = false;
	//}

	//�ړ�����
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
	if (HPp <= 0)
	{
		isDead = true;
		HPp = 0;
	}
	//��


}

void Enemy::OnCollision2(Collider& collider)
{
	//�G���m�ŏd�Ȃ�Ȃ��悤��
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
	enemy->draw[0].DrawModel(enemy->GetWorldTransForm(), &view, &projection, model, { 1.0f,0,0,1.0f });
}


//--------------------------------------------------------------------------------------
void AttackE::Update()
{
	Vec3 lengthV = enemy->player->GetWorldPos() - enemy->GetWorldPos();

	if (lengthV.GetLength() < Enemy::pLength)
	{
		//player�Ɍ�����
		Vec3 v = enemy->GetVelocity() * 5.0f + lengthV * 0.1f;
		enemy->SetVelocity(v.GetNormalized() * 0.3f);
	}
}

void AttackE::Draw(ViewMat& view, ProjectionMat& projection, Model* model)
{
	enemy->draw[1].DrawModel(enemy->GetWorldTransForm(), &view, &projection, model, { 1.0f,0,0.5f,1.0f });
}
