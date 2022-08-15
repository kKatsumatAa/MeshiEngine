#pragma once
#include"Enemy.h"
#include<sstream>

class EnemyManager
{
private:
	SoundData* soundData;
	Player* player;
	BulletManager* bulletManager;

	float gTimer = 300;
	//�G�����R�}���h
	std::stringstream enemyPopCommands;
	//�ҋ@
	bool isWait = false;
	int  waitTimer = 0;


	/// <summary>
	/// �G�����R�}���h�̍X�V
	/// </summary>
	void UpdateEnemyPopCommands();
	void EnemyGenerate(const Vec3& pos);

public:
	std::list<std::unique_ptr<Enemy>> enemies;


	void Initialize(Player* player, BulletManager* bulletManager,SoundData* soundData);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

	//�e���X�g���擾(const�Q��)
	const std::list<std::unique_ptr<Enemy>>& GetEnemies()
	{
		return enemies;
	}

	/// <summary>
	/// �G�����f�[�^�̓ǂݍ���
	/// </summary>
	void LoadEnemyPopData();

	
};

