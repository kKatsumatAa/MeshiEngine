#include "Player.h"
#include "Util.h"


void Player::ChangeState(PlayerAttackState* state)
{
	delete this->state;
	this->state = state;
	state->SetPlayer(this);
}

void Player::Initialize(Model* model, Model* modelAttack/*, EffectManager* effectM*//*, Tutorial* tutorial*/)
{
	assert(model);
	assert(modelAttack);

	model_ = model;
	this->modelAttack = modelAttack;

	isPlayer = true;
	isDead = false;
	this->isGround = false;
	this->isJump = false;

	this->audio = &Sound::GetInstance();

	//this->tutorial = tutorial;

	//シングルトンインスタンスを取得
	input_ = &KeyboardInput::GetInstance();

	worldTransform_.trans = { 0,0.0f,0 };
	worldTransform_.rot = { 0,0.0f,0 };
	worldTransform_.scale = { scaleTmp,scaleTmp,scaleTmp };
	worldTransform_.SetWorld();

	velocityY = 0;

	//無敵時間
	dmageCoolTime = 0;

	HPp = hptmp;
	radius_ = scaleTmp;

	//衝突属性
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy);


	ChangeState(new NoAttackP);
}

void Player::Update()
{
	////チュートリアル
	//if (tutorial != nullptr)
	//{
	//	isDead = false;
	//}

	//無敵時間
	if (dmageCoolTime > 0)
	{
		dmageCoolTime--;

		if (dmageCoolTime % 20 < 10) worldTransform_.scale = { scaleTmp,scaleTmp,scaleTmp };
		else                         worldTransform_.scale = { scaleTmp + 0.5f,scaleTmp + 0.5f,scaleTmp + 0.5f };
	}
	else
	{
		worldTransform_.scale = { scaleTmp,scaleTmp,scaleTmp };
	}

	//入力
	if (input_->KeyPush(DIK_RIGHTARROW) || input_->KeyPush(DIK_LEFTARROW)
		|| input_->KeyPush(DIK_D) || input_->KeyPush(DIK_A)) {
		worldTransform_.trans.x += ((input_->KeyPush(DIK_RIGHTARROW) || input_->KeyPush(DIK_D))
			- (input_->KeyPush(DIK_LEFTARROW) || input_->KeyPush(DIK_A))) * 0.5f;
	}
	//移動制限
	if (worldTransform_.trans.x <= -movingMax)worldTransform_.trans.x = -movingMax;
	if (worldTransform_.trans.x >= movingMax)worldTransform_.trans.x = movingMax;

	state->Update();

	worldTransform_.SetWorld();
}

void Player::Draw(ViewMat& view, ProjectionMat& projection)
{
	state->Draw(view, projection, model_, modelAttack);
}

void Player::DrawSprite()
{
	//gaugeS->Draw();
}


//--------------------------------------------------------------------------------------
void Player::OnCollision(Collider& collider)
{
	if (dmageCoolTime <= 0)
	{
		HPp--;
		//無敵時間
		dmageCoolTime = dmageCoolTimeTmp;
		if (HPp <= 0)
		{
			isDead = true;
			HPp = 0;
		}
		//音

	}
}

void Player::OnCollision2(Collider& collider)
{
	SetJumpPower(1.0f);
	SetIsJump(true);
}


//--------------------------------------------------------------------------------------

void PlayerAttackState::SetPlayer(Player* player)
{
	this->player = player;
}

//--------------------------------------------------------------------------------------
void NoAttackP::Update()
{
	if (player->input_->KeyPush(DIK_SPACE))
	{
		//音

		count++;

		player->SetIsGround(false);
		player->SetJumpPower(LerpVec3({ 0,0,0 }, { 0,player->GetJumpPowerTmp(),0 }, EaseOut((float)count / (float)countMax)).y);
		player->SetWorldPos({ player->GetWorldPos().x, player->GetWorldPos().y + player->GetJumpPower(), player->GetWorldPos().z });

	}
	if (player->input_->KeyReleaseTrigger(DIK_SPACE) || count >= countMax || player->GetIsJump())
	{
		player->SetIsJump(true);
		player->ChangeState(new JumpAttackP);
	}
}

void NoAttackP::Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack)
{
	player->draw[0].DrawModel(player->GetWorldTransForm(), &view, &projection, model);
}


//--------------------------------------------------------------------------------------
void JumpAttackP::Update()
{
	//重力を加算していく
	player->SetJumpPower(player->GetJumpPower() - player->GetGravityTmp());
	//ジャンプパワーで座標を移動(yのみ)
	player->SetWorldPos({ player->GetWorldPos().x, player->GetWorldPos().y + player->GetJumpPower(), player->GetWorldPos().z });

	//地面と当たったら
	if (player->GetIsGround() || player->input_->KeyTrigger(DIK_Q))
	{
		player->SetIsJump(false);
		player->SetJumpPower(0);
		player->ChangeState(new NoAttackP);
	}
}

void JumpAttackP::Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack)
{
	player->draw[1].DrawModel(player->GetWorldTransForm(), &view, &projection, model);
}
