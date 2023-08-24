#pragma once
#include"Enemy.h"


class EnemyManager final
{
private:
	int32_t waveNum_ = 1;

private:
	EnemyManager() { ; }
	~EnemyManager() { ; }

public:
	EnemyManager& operator=(const EnemyManager& inst) = delete;
	EnemyManager(const EnemyManager& inst) = delete;

	static EnemyManager& GetInstance();

public:
	void Initialize();
	void Update();
	void Draw();

private:
	std::vector<Object*>GetEnemys();
	Enemy* TransToEnemy(Object* obj);

	//���݂̃E�F�[�u�ȊO�̓G�̗L���t���O��false
	void SetIsValidOtherWaveEnemy();
	//���݂̃E�F�[�u�̓G�̗L���t���O��true
	void SetIsValidWaveEnemy();
	//�E�F�[�u�̓G���������Ȃ����ǂ���
	bool GetIsAliveWaveEnemy();
};