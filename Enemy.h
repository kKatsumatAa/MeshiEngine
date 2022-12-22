#pragma once
#include"Player.h"

class Enemy;

class EnemyState
{
protected:
	Enemy* enemy = nullptr;

public:
	void SetEnemy(Enemy* Enemy);
	virtual void Update(/*Tutorial* tutorial = nullptr*/) = 0;
	virtual void Draw(ViewMat& view, ProjectionMat& projection, Model* model) = 0;
};


class Enemy : public Collider
{
private:
	//���f��
	Model* model_ = nullptr;

	DebugText* debugText_ = nullptr;

	//�ړ�����
	const float movingMax = 25.0f;

	float HPp;
	const float hptmp = 1;

	//�U���̏��
	EnemyState* state = nullptr;

	const float scaleTmp = 2.5f;

	

	/*Tutorial* tutorial;*/

	//EffectManager* effectM_;
public:
	//sound
	Sound* audio = nullptr;
	Object draw[10];
	//player
	Player* player;
	static const float pLength;

	void ChangeState(EnemyState* state);

	void Initialize(Model* model, Player* player/* EffectManager* effectM_,*/
	/*,Tutorial* tutorial = nullptr*/);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);
	void DrawSprite();


	void SetWorldPos(const Vec3& pos) { worldTransform_.trans = pos; };
	int GetHP() { return HPp; }

	

	//�Փ˂����o������Ăяo���i�R�[���o�b�N�֐��j
	void OnCollision(Collider& collider)override;
	void OnCollision2(Collider& collider)override;
};


//�������Ă��Ȃ�
class NoAttackE : public EnemyState
{
private:

public:
	void Update(/*Tutorial* tutorial = nullptr*/);
	void Draw(ViewMat& view, ProjectionMat& projection, Model* model);
};

//�U����
class AttackE : public EnemyState
{
private:

public:
	void Update(/*Tutorial* tutorial = nullptr*/);
	void Draw(ViewMat& view, ProjectionMat& projection, Model* model);
};

