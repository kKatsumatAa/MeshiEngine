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

	//‚Ç‚¿‚ç‚©‚Ìè‚ªUŒ‚’†‚¶‚á‚È‚©‚Á‚½‚ç
	bool GetIsUseWitchHand();
	//UŒ‚’†‚¶‚á‚È‚¢è‚Ìƒ|ƒCƒ“ƒ^‚à‚ç‚¤
	PlayerHand* GetWitchUseHand();

public:
	void Initialize(Player* player);

	void Update();

	void Draw();
};