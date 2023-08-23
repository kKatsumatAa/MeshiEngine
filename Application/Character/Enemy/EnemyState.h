#pragma once
#include"CollisionManager.h"
#include"CharacterState.h"

class Enemy;


//�f�肩�A�e�������Ă邩���̃X�e�[�g
class EnemyState : public CharacterState
{
protected:
	Enemy* enemy_ = nullptr;

	const uint16_t BARE_HANDS_ATTR_TMP_ = COLLISION_ATTR_ITEMS | COLLISION_ATTR_LANDSHAPE;
	const uint16_t HAVE_WEAPON_ATTR_TMP_ = COLLISION_ATTR_ALLIES | COLLISION_ATTR_LANDSHAPE;

public:
	virtual ~EnemyState() { ; }

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	bool CheckEyeRayHit()override;

	//������̂��Ȃ��e���������炻�̍��W�A�Ȃ���΃v���C���[�̍��W
	Vec3 GetRayHitGunOrPlayerPos();
};

//�f����
class EnemyStateBareHands :
	public CharacterStateBareHands, 
	public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};

//�e�����Ă�
class EnemyStateHaveWeapon :
	public CharacterStateHaveWeapon,
	public EnemyState
{

public:
	void Initialize() override;
	void Update() override;
};
