#pragma once
#include "PlayerHand.h"
#include "CollisionManager.h"


class PlayerHandManager
{
private:
	std::unique_ptr<PlayerHand> handR = nullptr;
	std::unique_ptr<PlayerHand> handL = nullptr;

	Player* player_ = nullptr;

public:


private:
	void HandAttack(PlayerHand* hand, const RaycastHit& info);

	//どちらかの手が攻撃中じゃなかったら
	bool GetIsUseWitchHand();
	//攻撃中じゃない手のポインタもらう
	PlayerHand* GetWitchUseHand();

public:
	void Initialize(Player* player);

	void Update();

	void Draw();
};