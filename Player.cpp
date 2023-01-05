#include "Player.h"
#include "Util.h"



void Player::ChangeState(PlayerAttackState* state)
{
	delete this->state;
	this->state = state;
	state->SetPlayer(this);
}

void Player::Initialize(Model* model, Model* modelAttack, PlayerBulletManager* playerBulletM, DebugText* debugText_,
	Camera* camera/*, EffectManager* effectM*//*, Tutorial* tutorial*/)
{
	assert(model);
	assert(modelAttack);

	model_ = model;
	this->modelAttack = modelAttack;
	this->playerBulletM = playerBulletM;
	this->debugText_ = debugText_;
	this->camera = camera;

	isPlayer = true;
	isDead = false;
	this->isGround = false;
	this->isJump = false;

	{
		//死亡フラグ
		isDead = false;

		velocity = { 0,0,0 };
	}

	this->audio = &Sound::GetInstance();

	//this->tutorial = tutorial;

	//シングルトンインスタンスを取得
	input_ = &KeyboardInput::GetInstance();

	worldTransform_.trans = { -50.0f,0.0f,0 };
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

		if (dmageCoolTime % 15 < 8)
		{
			worldTransform_.scale = { scaleTmp,scaleTmp,scaleTmp };
		}
		else
		{
			worldTransform_.scale = { scaleTmp + 0.5f,scaleTmp + 0.5f,scaleTmp + 0.5f };
		}
	}
	else
	{
		worldTransform_.scale = { scaleTmp,scaleTmp,scaleTmp };
	}

	//入力
	{
		velocity.x = ((input_->KeyPush(DIK_RIGHTARROW) || input_->KeyPush(DIK_D)) - (input_->KeyPush(DIK_LEFTARROW) || input_->KeyPush(DIK_A))) * 0.5f;
	}

	state->Update();

	//重力を加算していく
	SetJumpPower(GetJumpPower() - GetGravityTmp());

	worldTransform_.SetWorld();
}

void Player::Draw(ViewMat& view, ProjectionMat& projection)
{
	state->Draw(view, projection, model_, modelAttack);

	debugText_->Print("isGround", 10, 10, isGround);
	debugText_->Print("isJump", 10, 30, isJump);
	debugText_->Printf("velocityY", 10, 50, velocity.y);
}

void Player::DrawSprite()
{
	gauge[1].DrawClippingBoxSprite({ 100,40,0 }, 1.0f, { 0,0 }, { 1.0f,0.2f }, { 0.3f,0.3f,0.3f,1.0f });
	gauge[0].DrawClippingBoxSprite({ 100,40,0 }, 1.0f, { 0,0 }, { HPp / (float)hptmp ,0.2f }, { 1.0f,0.0f,0.0f,0.9f });
}

void Player::LandingEffect()
{
	//プレイヤー着地時演出
	ParticleManager::GetInstance()->GenerateRandomParticle(5, 7, 1.7f,
		{ GetWorldPos().x,GetWorldPos().y - GetRadius(),GetWorldPos().z }, 2.0f, 0, { 1.0f,1.0f,1.0f,1.0f }, { 0,0,0,0 });

	//リロード
	if (playerBulletM->GetBulletNum() < playerBulletM->GetBulletNumMax())
	{
		Vec3 pos = GetWorldPos();

		ReloadEffectManager::GetInstance().GenerateReloadEffect({ pos.x,pos.y - GetRadius(),pos.z },
			{ pos.x,pos.y + GetRadius() * 4.0f,pos.z }, 15, GetRadius() * 4.0f);
	}
}


//--------------------------------------------------------------------------------------
void Player::OnCollision(Collider& collider)
{
	if (dmageCoolTime <= 0)
	{
		HPp--;
		//無敵時間
		dmageCoolTime = dmageCoolTimeTmp;
		//
		camera->CameraShake(60, 2.0f);
		if (HPp <= 0)
		{
			isDead = true;
			HPp = 0;
		}
		//音

	}
}

//敵を踏んだ時
void Player::OnCollision2(Collider& collider)
{
	//弾全回復
	playerBulletM->SetBulletNum(playerBulletM->GetBulletNumMax());
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
	if (player->input_->KeyTrigger(DIK_SPACE))
	{
		//音

		//effect
		ParticleManager::GetInstance()->GenerateRandomParticle(8, 10, 2.0f,
			{ player->GetWorldPos().x,player->GetWorldPos().y - player->GetRadius(),player->GetWorldPos().z },
			3.0f, 0, { 1.0f,1.0f,1.0f,1.0f }, { 0,0,0,0 });
		player->SetIsGround(false);
		player->SetIsJump(true);
		player->SetJumpPower(0.5f);
		player->ChangeState(new JumpP);
	}
	//space押してなくて落ちたら
	else if (!player->GetIsGround())
	{
		player->SetIsJump(true);
		player->ChangeState(new JumpAttackP);
	}

}

void NoAttackP::Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack)
{
	player->draw[0].DrawModel(player->GetWorldTransForm(), &view, &projection, model);
}


//---------------------------------------------------------------------------------------------------
void JumpP::Update()
{
	//地面と当たったら                                     
	if (player->GetIsGround())
	{
		//プレイヤー着地時演出
		player->LandingEffect();
		//弾全回復
		player->playerBulletM->SetBulletNum(player->playerBulletM->GetBulletNumMax());
		player->SetIsJump(false);

		player->ChangeState(new NoAttackP);
	}
	else
	{
		if (player->input_->KeyPush(DIK_SPACE) && player->GetIsJump() && !player->GetIsGround())
		{
			if (count < countMax) {
				count++;
				player->SetJumpPower(LerpVec3({ 0,0,0 }, { 0,player->GetJumpPowerTmp(),0 }, EaseOut((float)count / (float)countMax)).y);
			}
		}
		else if (player->input_->KeyRelease(DIK_SPACE))
		{
			player->ChangeState(new JumpAttackP);
		}
	}
}

void JumpP::Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack)
{
	player->draw[0].DrawModel(player->GetWorldTransForm(), &view, &projection, model);
}



//--------------------------------------------------------------------------------------
void JumpAttackP::Update()
{
	//地面と当たったら                                     
	if (player->GetIsGround())
	{
		//プレイヤー着地時演出
		player->LandingEffect();
		//弾全回復
		player->playerBulletM->SetBulletNum(player->playerBulletM->GetBulletNumMax());
		player->SetIsJump(false);

		player->ChangeState(new NoAttackP);
	}
	else
	{
		//弾発射
		std::function<void()>p = [=]() {player->SetZeroJumpPower(); };
		player->playerBulletM->Shot(player->GetWorldPos(), p);

	}

}

void JumpAttackP::Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack)
{
	player->draw[1].DrawModel(player->GetWorldTransForm(), &view, &projection, model);
}

