#pragma once
#include "PlayerHand.h"
#include "CollisionManager.h"
#include "CollisionInfo.h"



class PlayerHandManager
{
private:
	std::unique_ptr<PlayerHand> handR_ = nullptr;
	std::unique_ptr<PlayerHand> handL_ = nullptr;

	Player* player_ = nullptr;

	CollisionInfo c_;

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
};