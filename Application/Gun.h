#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"


class Gun :
	public Object
{
private:
	//�U���̃N�[���^�C��
	const float SHOT_COOL_TIME_MAX_ = 35;
	float shotCoolTime_ = SHOT_COOL_TIME_MAX_;

	const float BULLET_VELOCITY_ = 1.8f;

	//�e�̎c��
	const int32_t BULLETS_TMP_ = 3;
	int32_t remainingBullets_ = BULLETS_TMP_;

	//����ł�
	bool isAlreadyShot_ = false;

	//��𗣂ꂽ�Ƃ��̃x�N�g��
	Vec3 fallVec_ = { 0,0,0 };

	Vec3 localPos_ = { 0,0,0 };

	//���ˈʒu
	Vec3 shotPos = { 0,0,0 };


public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <param name="model"></param>
	/// <returns></returns>
	static std::unique_ptr<Gun> Create(std::unique_ptr<WorldMat> worldMat);

private:
	//���L�҂����Ȃ��Ƃ��ɔ��ł���
	void NoParentMove();

	void ParticleGenerate();

public:
	//�ˌ�
	void Shot(const Vec3& directionVec, int32_t decreBullet = 1);

	//������ύX
	void ChangeOwner(WorldMat* parent);

public:
	//
	void SetFallVec(const Vec3& fallVec) { fallVec_ = fallVec; }
	//���[�J�����W�Z�b�g
	void SetLocalPos(const Vec3& pos) { localPos_ = pos; }

	//������A��e���ė�����Ƃ��̃X�s�[�h
	const Vec3& GetFallVelocity() { return fallVec_; }

public:

	bool Initialize(std::unique_ptr<WorldMat> worldMat) override;

	void Update() override;

	void OnCollision(const CollisionInfo& info) override;
};

