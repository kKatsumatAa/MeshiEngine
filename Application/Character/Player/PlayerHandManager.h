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

public:


private:
	void HandAttack(PlayerHand* hand, const RaycastHit& info);

	//‚Ç‚¿‚ç‚©‚Ìè‚ªUŒ‚’†‚¶‚á‚È‚©‚Á‚½‚ç
	bool GetIsUseWitchHand();
	//UŒ‚’†‚¶‚á‚È‚¢è‚Ìƒ|ƒCƒ“ƒ^‚à‚ç‚¤
	PlayerHand* GetWitchUseHand();

public:
	//‰£‚é
	void Attack(RaycastHit info);

public:
	PlayerHandManager() { ; }
	~PlayerHandManager() { ; }

	void Initialize(Player* player);

	void Update();

	void Draw();

	void DeleteHands();
};