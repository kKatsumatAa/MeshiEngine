#pragma once
#include"Object.h"
#include"Weapon.h"
#include"CollisionManager.h"
#include<functional>
#include"ImGuiManager.h"


class Character : public Object
{
protected:
	//����
	Weapon* weapon_ = nullptr;
	//hp
	int8_t hp_ = 0;

	//�n�ʂƓ������Ă��邩
	bool isOnGround_ = false;

	//�����������x
	const float FALL_ACC_ = -0.15f;
	const float FALL_V_Y_MIN_ = -3.5f;
	Vec3 fallVec_;

	const float JUMP_V_Y_FIST_ = 1.7f;//�W�����v�����������

	//�ǂƔF������p�x
	static const float IS_WALL_ROT_;

	//�_���[�W�󂯂邩�ǂ���(�f�o�b�O�p)
	bool isValidDamage_ = true;

protected:
	/// <summary>
	/// �_���[�W�󂯂鏈��
	/// </summary>
	/// <param name="damage">�_���[�W��</param>
	/// <param name="deadFunc">���񂾂Ƃ��ɌĂяo������</param>
	virtual void Damaged(int32_t damage, std::function<void()> deadFunc);

public:
	virtual ~Character() { ; }

public:
	virtual void Update();

	void DrawImGui(std::function<void()>imguiF = NULL)override;

public:
	//����
	Weapon* GetWeapon() { return weapon_; }
	void SetWeapon(Weapon* weapon) { weapon_ = weapon; }

	//����E��
	virtual void PickUpWeapon(Weapon* weapon, Vec3* localPos = nullptr);
	//���헎�Ƃ��A������
	virtual void FallWeapon(const Vec3& directionVec, Vec3* localPos = nullptr);

	//�n�ʂƕǂƂ̔���
	virtual void OnGroundAndWallUpdate(float LengthY, float velocityYPow, bool isJump = false, std::function<void()>f = nullptr);

	//�N�G���R�[���o�b�N
	virtual void QueryCallBackUpdate();

	void SetIsValid(bool isValid)override;

	//�݂Ă�������ɉ�����������
	bool CheckRayOfEyeHit(const Vec3& dir, float length, uint16_t attr, RaycastHit* info = nullptr);
};