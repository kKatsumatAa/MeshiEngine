#include "StageState.h"
#include "StageManager.h"
#include "MouseInput.h"


//共通の処理
void StageState::Initialize()
{
}

//共通の処理
void StageState::Update()
{
	//レベルデータで読み込んだオブジェクト等
	LevelManager::GetInstance().Update();

	//弾
	BulletManager::GetInstance().Update();

	//判定
	CollisionManager::GetInstance()->CheckAllCollisions();

	//ゲームスピード
	GameVelocityManager::GetInstance().Update();
}

//共通の処理
void StageState::Draw()
{
	//弾
	BulletManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();

	ParticleManager::GetInstance()->Draw();
}

//共通の処理
void StageState::DrawImgui()
{
	GameVelocityManager::GetInstance().UpdateImGui();
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
	LevelManager::GetInstance().SetIsDissolveT(1.0f - t, COLLISION_ATTR_LANDSHAPE);

	//時間超えたら
	if (timer_ >= EFFECT_TIMER_MAX_)
	{
		StageManager::GetInstance().ChangeState("BATTLE");
	}

	timer_++;
}

void StageStateBegining::Draw()
{
	StageState::Draw();
}

void StageStateBegining::DrawSprite()
{
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
	if (LevelManager::GetInstance().GetGameOver())
	{
		StageManager::GetInstance().ChangeState("GAMEOVER");
	}
	//ゲームクリア
	else if (LevelManager::GetInstance().GetGameClear())
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

void StageStateBattle::Draw()
{
	//共通
	StageState::Draw();

	//チュートリアル
	Tutorial::GetInstance().Draw();
}

void StageStateBattle::DrawSprite()
{
}


//--------------------------------------------------------------------------
//死亡後
void StageStateDead::Initialize()
{
	StageState::Initialize();

	//ステージを徐々にディゾルブ(そのオブジェクトがディゾルブ画像読み込んでいる前提)
	LevelManager::GetInstance().SetObjectIsDissolve(true, COLLISION_ATTR_LANDSHAPE);
}

void StageStateDead::Update()
{
	//共通
	StageState::Update();

	//ステージを徐々にディゾルブ(そのオブジェクトがディゾルブ画像読み込んでいる前提)
	float t = (float)timer_ / (float)EFFECT_TIMER_MAX_;
	LevelManager::GetInstance().SetIsDissolveT(t, COLLISION_ATTR_LANDSHAPE);

	//一定時間たったらフラグ立てる
	if (timer_ >= EFFECT_TIMER_MAX_)
	{
		StageManager::GetInstance().SetIsGameOver(true);
	}

	timer_++;
}

void StageStateDead::Draw()
{
	//共通
	StageState::Draw();
}

void StageStateDead::DrawSprite()
{
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

void StageStateClear::Draw()
{
	//共通
	StageState::Draw();

	//クリア演出
	ClearEffect::GetInstance().Draw();
}

void StageStateClear::DrawSprite()
{
}