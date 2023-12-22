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

public:


private:
	void HandAttack(PlayerHand* hand, const RaycastHit& info);

	//どちらかの手が攻撃中じゃなかったら
	bool GetIsUseWitchHand();
	//攻撃中じゃない手のポインタもらう
	PlayerHand* GetWitchUseHand();

public:
	//殴る
	void Attack(RaycastHit info);

public:
	PlayerHandManager() { ; }
	~PlayerHandManager() { ; }

	void Initialize(Player* player);

	void Update();

	void Draw();

	void DeleteHands();
};