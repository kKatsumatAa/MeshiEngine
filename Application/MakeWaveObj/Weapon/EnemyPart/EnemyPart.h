#pragma once
/*
* @file EnemyPart.h
* @brief 敵の部位（攻撃されて分離した）
*/

#pragma once
#include"Weapon.h"

class EnemyPart : public Weapon
{
public:
	static const std::string S_OBJ_NAME_;
private:
	float timer_ = 0;
	const float LIFE_TIME_MAX_ = 120.0f;


protected:
	//地形と当たった時に呼び出す
	void OnLandShape(const Vec3& interPos) override;

	//パーティクル生成
	void ParticleGenerate(const XMFLOAT4& sColor, const XMFLOAT4& eColor, float particleSize = 1.0f
		, ParticleManager::BLEND_NUM blendNum = ParticleManager::BLEND_NUM::TRIANGLE) override;

public:
	//攻撃
	void Attack(const Vec3& directionVec, int32_t decreBullet = 1, IObject3D* owner = nullptr, float particleSize = 1.0f) override;

public:
	//クールタイムの終わりまでの割合(0~1.0f)
	float GetAttackCoolTimeRatio() override;

public:
	//インスタンス生成
	static std::unique_ptr<EnemyPart>Create(const WorldMat& worldMat, IModel* model);

	//初期化
	bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model) override;
	//更新
	void Update()override;
	//衝突時の処理
	void OnCollision(const CollisionInfo& info) override;
};