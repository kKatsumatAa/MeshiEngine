#pragma once
#include"Object.h"

class Player;
class PlayerAttackState;


class PlayerHand : public Object
{
private:
	bool isAttacking_ = false;
	std::unique_ptr<PlayerAttackState> attackState_ = nullptr;
	Player* player_ = nullptr;

	//�G�̏Փ˓_�܂ł̋���
	float interLength_ = 0;

	//�E�肩
	bool isRight_ = false;

	//�U�����ɑ����p�̃|�W�V����
	Vec3 addTrans_ = { 0,0,0 };
	//���ƂȂ���W
	Vec3 transTmp_ = { 0,0,0 };

public:
	void SetPlayer(Player* player) { player_ = player; }
	Player* GetPlayer() { return player_; }

	void SetIsAttacking(bool is) { isAttacking_ = is; }
	bool GetIsAttacking() { return isAttacking_; }

	//�G�̏Փ˓_�܂ł̋���
	void SetInterLength(float length) { interLength_ = length; }
	float GetInterLength() { return interLength_; }

	void SetAddTrans(const Vec3& addTrans) { addTrans_ = addTrans; }
	const Vec3& GetAddTrans() { return addTrans_; }

	//�X�e�[�g
	PlayerAttackState* GetAttackState() { return attackState_.get(); }


public:
	static std::unique_ptr<PlayerHand> Create(Player* player, const Vec3& transTmp,bool isRight, const std::string& objName);
	bool Initialize(Player* player, const Vec3& transTmp,bool isRight, const std::string& objName);

	void Update()override;

	void Draw()override;


	//�U���̃X�e�[�g�ύX
	void ChangeAttackState(std::unique_ptr<PlayerAttackState> state);
};
