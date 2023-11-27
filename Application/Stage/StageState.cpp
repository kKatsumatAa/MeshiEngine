#include "StageState.h"
#include "StageManager.h"
#include "MouseInput.h"
#include "PlayerUI.h"
#include "PostEffectManager.h"
#include "EnemyManager.h"


//共通の処理
void StageState::Initialize()
{
}

//共通の処理
void StageState::Update()
{
	//敵マネージャー
	EnemyManager::GetInstance().Update();

	//弾
	BulletManager::GetInstance().Update();

	//判定
	CollisionManager::GetInstance()->CheckAllCollisions();

	//プレイヤーui
	PlayerUI::GetInstance().Update();

	//海
	if (!EnemyManager::GetInstance().GetIsAllEnemyDead())
	{
		StageManager::GetInstance().ApproachLava();
	}
}

void StageState::DrawShadow()
{
	//弾のシャドウマップ用描画
	BulletManager::GetInstance().DrawShadow();
}

//共通の処理
void StageState::Draw()
{
	//弾
	BulletManager::GetInstance().Draw();
}

void StageState::DrawSprite()
{
}

//共通の処理
void StageState::DrawImgui()
{
	GameVelocityManager::GetInstance().UpdateImGui();
	EnemyManager::GetInstance().DrawImGui();
}


std::unique_ptr<StageState> StageState::GetState(const std::string& name)
{
	std::unique_ptr<StageState> state;

	if (name == "BEGINING")
	{
		state = std::make_unique<StageStateBegining>();
	}
	if (name == "BATTLE")
	{
		state = std::make_unique<StageStateBattle>();
	}
	if (name == "GAMEOVER")
	{
		state = std::make_unique<StageStateDead>();
	}
	if (name == "CLEAR")
	{
		state = std::make_unique<StageStateClear>();
	}

	return std::move(state);
}


//--------------------------------------------------------------------------
//開始時
void StageStateBegining::Initialize()
{
	StageState::Initialize();
}

void StageStateBegining::Update()
{
	//共通
	StageState::Update();

	//ステージを徐々にディゾルブ(そのオブジェクトがディゾルブ画像読み込んでいる前提)
	float t = (float)timer_ / (float)EFFECT_TIMER_MAX_;

	auto objs = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_LANDSHAPE);
	for (auto obj : objs)
	{
		obj->SetDissolveT(1.0f - t);
	}

	//時間超えたら
	if (timer_ >= EFFECT_TIMER_MAX_)
	{
		StageManager::GetInstance().ChangeState("BATTLE");
	}

	timer_++;
}

void StageStateBegining::DrawShadow()
{
	//共通
	StageState::DrawShadow();
}

void StageStateBegining::Draw()
{
	StageState::Draw();
}

void StageStateBegining::DrawSprite()
{
	PlayerUI::GetInstance().DrawSprite();

	StageState::DrawSprite();
}


//--------------------------------------------------------------------------
//戦闘時
void StageStateBattle::Initialize()
{
	StageState::Initialize();
}

void StageStateBattle::Update()
{
	//共通
	StageState::Update();

	//チュートリアル
	Tutorial::GetInstance().Update();

	//ゲームオーバー
	if (ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, "player").size() <= 0)
	{
		StageManager::GetInstance().ChangeState("GAMEOVER");
	}
	//ゲームクリア
	else if (EnemyManager::GetInstance().GetIsAllEnemyDead())
	{
		//ゲームスピードの加算無効
		GameVelocityManager::GetInstance().SetIsInvalidAddGameVel(true);

		//少しずつゆっくりにしていく
		GameVelocityManager::GetInstance().AddGameVelocity(CLEAR_ADD_GAME_VEL_);

		//時間超えたら
		if (timer_ >= CLEAR_COOL_TIMER_)
		{
			//ゲームスピードの加算無効解除
			GameVelocityManager::GetInstance().SetIsInvalidAddGameVel(false);

			StageManager::GetInstance().ChangeState("CLEAR");
		}

		timer_++;
	}
}

void StageStateBattle::DrawShadow()
{
	//共通
	StageState::DrawShadow();
}

void StageStateBattle::Draw()
{
	//共通
	StageState::Draw();
}

void StageStateBattle::DrawSprite()
{
	StageState::DrawSprite();

	PlayerUI::GetInstance().DrawSprite();

	//チュートリアル
	Tutorial::GetInstance().Draw();
}


//--------------------------------------------------------------------------
//死亡後
void StageStateDead::Initialize()
{
	StageState::Initialize();

	//ステージを徐々にディゾルブ(そのオブジェクトがディゾルブ画像読み込んでいる前提)
	auto objs = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_LANDSHAPE);

	for (auto obj : objs)
	{
		obj->SetisDissolve(true);
	}
}

void StageStateDead::Update()
{
	//共通
	StageState::Update();

	//ステージを徐々にディゾルブ(そのオブジェクトがディゾルブ画像読み込んでいる前提)
	float t = (float)timer_ / (float)EFFECT_TIMER_MAX_;

	auto objs = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_LANDSHAPE);
	for (auto obj : objs)
	{
		obj->SetDissolveT(t);
	}

	//一定時間たったらフラグ立てる
	if (timer_ >= EFFECT_TIMER_MAX_ || objs.size() <= 0)
	{
		StageManager::GetInstance().SetIsGameOver(true);
	}

	timer_++;
}

void StageStateDead::DrawShadow()
{
	//共通
	StageState::DrawShadow();
}

void StageStateDead::Draw()
{
	//共通
	StageState::Draw();
}

void StageStateDead::DrawSprite()
{
	StageState::DrawSprite();
}


//--------------------------------------------------------------------------
//クリア後
void StageStateClear::Initialize()
{
	StageState::Initialize();

	//クリア演出
	ClearEffect::GetInstance().BeginClearEffect();
}

void StageStateClear::Update()
{
	//共通
	StageState::Update();

	//クリア演出用
	ClearEffect::GetInstance().Update();

	//連打してもクリア演出飛ばせないように
	if (MouseInput::GetInstance().GetClick(CLICK_LEFT) && timer_ >= CLEAR_COOL_TIME_MAX_)
	{
		StageManager::GetInstance().SetIsClear(true);
	}

	timer_ = min(timer_ + 1, CLEAR_COOL_TIME_MAX_);
}

void StageStateClear::DrawShadow()
{
	//共通
	StageState::DrawShadow();
}

void StageStateClear::Draw()
{
	//共通
	StageState::Draw();
}

void StageStateClear::DrawSprite()
{
	StageState::DrawSprite();

	//クリア演出
	ClearEffect::GetInstance().Draw();
}