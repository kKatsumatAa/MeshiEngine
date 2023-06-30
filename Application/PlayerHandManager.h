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

	//�ǂ��炩�̎肪�U��������Ȃ�������
	bool GetIsUseWitchHand();
	//�U��������Ȃ���̃|�C���^���炤
	PlayerHand* GetWitchUseHand();

public:
	void Initialize(Player* player);

	void Update();

	void Draw();
};