/*
* @file PlayerHandManager.h
* @brief プレイヤーの腕のどちらかを使うかなどを管理
*/

#pragma once
#include "PlayerHand.h"
#include "CollisionManager.h"
#include "CollisionInfo.h"



class PlayerHandManager
{
public:
	const Vec3 LOCAL_POS_TMP_ = { 2.0f,-1.0f,1.0f };
	const float PUNCH_CAMERA_SHAKE_LENGTH_ = 1.05f;
	const int8_t PUNCH_CAMERA_SHAKE_TIME_ = 5;

private:
	PlayerHand* handR_ = nullptr;
	PlayerHand* handL_ = nullptr;

	const std::string OBJ_GROUP_NAME_ = "playerHand";
	const std::string HAND_R_NAME_ = "handR";
	const std::string HAND_L_NAME_ = "handL";

	Player* player_ = nullptr;

	CollisionInfo c_;

	Vec3 leftHandLocalPos = { 2.0f,-1.0f,-1.0f };
	Vec3 rightHandLocalPos = { -2.0f,-1.0f,-1.0f };


private:
	//腕の攻撃
	void HandAttack(PlayerHand* hand, const RaycastHit& info);

	//どちらかの手が攻撃中じゃなかったら
	bool GetIsUseWitchHand();
	//攻撃中じゃない手のポインタもらう
	PlayerHand* GetWitchUseHand();

public:
	//殴る
	void Attack(RaycastHit info);

public:
	//コンストラクタ
	PlayerHandManager() { ; }
	//デストラクタ
	~PlayerHandManager() { ; }

	//初期化
	void Initialize(Player* player);
	//更新
	void Update();
	//描画
	void Draw();

public:
	//両手を解放
	void DeleteHands();
};