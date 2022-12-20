//#pragma once
//#include "Collider.h"
//
//class Block : public Collider
//{
//
//private:
//	//���f��
//	Model* model_ = nullptr;
//
//	//�e�N�X�`���n���h��
//	DebugText* debugText_ = nullptr;
//
//
//	float HPp;
//	const float hptmp = 1;
//
//	const float scaleTmp = 3.0f;
//
//public:
//	//sound
//	Sound* audio = nullptr;
//	Object object;
//
//
//	void Initialize(Model* model,Vec3);
//	void Update();
//	void Draw(ViewMat& view, ProjectionMat& projection);
//	void DrawSprite();
//
//
//	void SetWorldPos(const Vec3& pos) { worldTransform_.trans = pos; };
//	float GetGravityTmp() { return gravityTmp; }
//	float GetJumpPowerTmp() { return jumpPowerTmp; }
//	float GetJumpPower() { return velocityY; }
//	bool GetIsJump() { return isJump; }
//	bool GetIsGround() { return isGround; }
//	int GetHP() { return HPp; }
//
//	void SetIsJump(bool isJump) { this->isJump = isJump; }
//	void SetIsGround(bool isGround) { this->isGround = isGround; }
//	void SetJumpPower(float jumpPower) { if (jumpPower > velocityYMin) this->velocityY = jumpPower; }
//	void SetVelocity(Vec3 vec) { velocity = vec; }
//	Vec3& GetVelocity() { return velocity; }
//
//	//�Փ˂����o������Ăяo���i�R�[���o�b�N�֐��j
//	//�_���[�W��������
//	void OnCollision(Collider& collider)override;
//	//�G���񂾎�
//	void OnCollision2(Collider& collider)override;
//};
//
