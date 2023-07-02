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

	//�ǂ��炩�̎肪�U��������Ȃ�������
	bool GetIsUseWitchHand();
	//�U��������Ȃ���̃|�C���^���炤
	PlayerHand* GetWitchUseHand();

public:
	//����
	void Attack(RaycastHit info);

public:
	PlayerHandManager() { ; }
	~PlayerHandManager() { ; }

	void Initialize(Player* player);

	void Update();

	void Draw();
};