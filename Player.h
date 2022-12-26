#pragma once
#include "Sound.h"
#include "PlayerBulletManager.h"

class Player;

class PlayerAttackState
{
protected:
	Player* player = nullptr;

public:
	void SetPlayer(Player* player);
	virtual void Update(/*Tutorial* tutorial = nullptr*/) = 0;
	virtual void Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack) = 0;
};


class Player : public Collider
{
private:
	//���f��
	Model* model_ = nullptr;
	Model* modelAttack = nullptr;

	//�e�N�X�`���n���h��
	uint32_t* textureHandle_;


	//�ړ�����
	const float movingMax = 25.0f;

	//�W�����v�n
	const float gravityTmp = 0.05f;
	float velocityY = 0;
	const float jumpPowerTmp = 1.1f;
	const float velocityYMin = -5.0f;
	bool isJump = false;



	float HPp;
	const float hptmp = 5;

	//�U���̏��
	PlayerAttackState* state = nullptr;

	//���G����
	int dmageCoolTime = 0;
	const int dmageCoolTimeTmp = 90;

	const float scaleTmp = 1.8f;

	//hp
	Object gaugeS;

	/*Tutorial* tutorial;*/

	//EffectManager* effectM_;
public:
	//sound
	Sound* audio = nullptr;
	KeyboardInput* input_ = nullptr;
	const Vec2 gaugeLength = { 1280,720 };
	Object draw[10];
	PlayerBulletManager* playerBulletM;
	DebugText* debugText_ = nullptr;

	/*XINPUT_STATE joyState;
	XINPUT_STATE oldJoyState;*/


	void ChangeState(PlayerAttackState* state);

	void Initialize(Model* model, Model* modelAttack, PlayerBulletManager* playerBulletM, DebugText* debugText_/* EffectManager* effectM_,*/
	/*,Tutorial* tutorial = nullptr*/);
	void Update();
	void Draw( ViewMat& view,  ProjectionMat& projection);
	void DrawSprite();


	void SetWorldPos(const Vec3& pos) { worldTransform_.trans = pos; };
	float GetGravityTmp() { return gravityTmp; }
	float GetJumpPowerTmp() { return jumpPowerTmp; }
	float& GetJumpPower() { return velocity.y; }
	bool GetIsJump() { return isJump; }

	void SetZeroJumpPower() { velocity.y = 0; }

	int GetHP() { return HPp; }

	void SetIsJump(bool isJump) { this->isJump = isJump; }

	void SetJumpPower(float jumpPower) { if (jumpPower > velocityYMin) this->velocity.y = jumpPower; }

	//�A�C�e���Ƃ̓����蔻�莞
	void SetBulletType(const int itemType) { playerBulletM->SetBulletType(itemType); }
	int GetBulletType() { playerBulletM->GetBulletType(); }

	//�Փ˂����o������Ăяo���i�R�[���o�b�N�֐��j
	//�_���[�W��������
	void OnCollision(Collider& collider)override;
	//�G���񂾎�
	void OnCollision2(Collider& collider)override;
};


//�������Ă��Ȃ�
class NoAttackP : public PlayerAttackState
{
private:
	int count = 0;
	const int countMax = 20;

public:
	void Update(/*Tutorial* tutorial = nullptr*/);
	void Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack);
};

//�W�����v�̂�
class JumpP : public PlayerAttackState
{
private:
	int count = 0;
	const int countMax = 20;

public:
	void Update(/*Tutorial* tutorial = nullptr*/);
	void Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack);
};

//�W�����v�U����
class JumpAttackP : public PlayerAttackState
{
private:
	const int countMax = 10;
	int count = countMax;

public:
	void Update(/*Tutorial* tutorial = nullptr*/);
	void Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack);
};