#include "EnemyManager.h"
using namespace std;

void EnemyManager::Initialize(Player* player, BulletManager* bulletManager, SoundData* soundData, ParticleManager* pManager, ItemManager* iManager)
{
	////�G�����R�}���h
	// �o�b�t�@���N���A���܂��B
	enemyPopCommands.str("");
	// ��Ԃ��N���A���܂��B
	enemyPopCommands.clear(std::stringstream::goodbit);

	//�ҋ@
	isWait = false;
	waitTimer = 0;

	enemies.clear();
	phase = 0;
	//phase���ς��܂ő҂t���O
	isPhase = false;
	isEnd[0] = {false};
	isEnd[1] = {false};
	isItem = false;


	this->soundData = soundData;
	this->player = player;
	this->bulletManager = bulletManager;
	particleManager = pManager;
	itemManager = iManager;

	LoadEnemyPopData();
}

void EnemyManager::EnemyGenerate(const Vec3& pos)
{
	//���𐶐��A������
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Initialize(player, bulletManager, { pos.x,pos.y,pos.z },soundData);
	/*enemy->SetPlayer(player_);*/
	//����o�^
	enemies.push_back(std::move(enemy));
}

void EnemyManager::BossGenerate(const Vec3& pos, float& scale, int& HP)
{
	//���𐶐��A������
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->isBoss = true;
	enemy->scaletmp = scale;
	enemy->Initialize(player, bulletManager, { pos.x,pos.y,pos.z },soundData);
	enemy->radius_ *= scale;
	enemy->worldMat.scale = { 10.0f,10.0f,10.0f };
	enemy->worldMat.SetWorld();
	//enemy->radius_ /= 2;
	enemy->HP = HP;

	//����o�^
	enemies.push_back(std::move(enemy));
}

void EnemyManager::Enemy2Generate(const Vec3& pos)
{
	//���𐶐��A������
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->isEnemy2 = true;

	enemy->Initialize(player, bulletManager, { pos.x,pos.y,pos.z }, soundData);
	enemy->HP = 2;

	//����o�^
	enemies.push_back(std::move(enemy));
}

void EnemyManager::Update()
{
	//�X�N���v�g��������
	UpdateEnemyPopCommands();

	//�G
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Update();
		if (enemy->IsDead())
		{
			SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[3], 2.0f);
			particleManager->GenerateRandomParticle(enemy->worldMat.trans, 120, enemy->worldMat.scale.x * 3.5f,
				{ enemy->GetColor().x,enemy->GetColor().y,enemy->GetColor().z,1.0f }, 10);
		}
	}

	//�G����
	enemies.remove_if([](std::unique_ptr<Enemy>& enemy)
		{
			return (enemy->IsDead() || enemy->IsAnnihilation());
		}
	);

	//�t�F�[�Y�ύX�i������|������t�F�[�Y�ύX�ɂȂ� && �G��0�ɂȂ�����j
	if ((isPhase) && enemies.size() == 0)
	{
		phase++;
		isPhase = false;
	}
	//�G��0�ɂȂ�܂ő҂̂�����
	if ((isEnd[0]) && enemies.size() == 0)
	{
		isEnd[0] = false;
	}
}

void EnemyManager::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Draw(view, projection, texHundle);
	}
}

void EnemyManager::LoadEnemyPopData()
{
	//�t�@�C���J��
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	//�t�@�C���̓��e�𕶎���X�g���[���ɃR�s�[
	enemyPopCommands << file.rdbuf();

	//�t�@�C�������
	file.close();
}

void EnemyManager::UpdateEnemyPopCommands()
{
	//�ҋ@����
	if (isPhase || isEnd[0])
	{
		return;
	}

	if (isWait)
	{
		waitTimer--;
		
		if (waitTimer <= 0)
		{
			//�ҋ@�I��
			isWait = false;
		}

		return;//�R�}���h���[�v�����s�����Ȃ�
	}

	//1�s���̕����������ϐ�
	std::string line;

	//�R�}���h���s���[�v
	while (getline(enemyPopCommands, line))
	{
		//1�����̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		std::string word;
		//[,]��؂�ōs�̐擪��������擾
		getline(line_stream, word, ',');

		//[//]����n�܂�s�̓R�����g
		if (word.find("//") == 0)
		{
			continue;
		}

		//Enemy2�R�}���h
		if (word.find("POP2") == 0)
		{
			//x���W
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//y���W
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//z���W
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			Enemy2Generate({ x,y,z });
		}
		//POP�R�}���h
		else if (word.find("POP") == 0)
		{
			//x���W
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			
			//y���W
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			
			//z���W
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			EnemyGenerate({ x,y,z });
		}
		//Boss�R�}���h
		else if (word.find("BOSS") == 0)
		{
			//x���W
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//y���W
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//z���W
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//scale
			getline(line_stream, word, ',');
			float scale = (float)std::atof(word.c_str());

			//hp
			getline(line_stream, word, ',');
			int HP = (int)std::atof(word.c_str());

			BossGenerate({ x,y,z }, scale, HP);
		}
		//WAIT�R�}���h
		else if(word.find("WAIT") == 0)
		{
			getline(line_stream, word, ',');

			//�҂�����
			int32_t waitTime = atoi(word.c_str());

			//�ҋ@�J�n
			isWait = true;
			waitTimer = waitTime;

			//�R�}���h���[�v������
			break;//(���̍s(POP)��ǂݍ��܂Ȃ��悤��)
		}
		//ZERO�R�}���h(�G���[���ɂȂ�܂ő҂�)
		else if (word.find("ZERO") == 0)
		{
			//phase���ς��܂ő҂t���O
			isEnd[0] = true;
			//phase++;

			//�R�}���h���[�v������
			break;//(���̍s(POP)��ǂݍ��܂Ȃ��悤��)
		}
		//PHASE�R�}���h
		else if (word.find("PHASE") == 0)
		{
			//phase���ς��܂ő҂t���O
			isPhase = true;
			//phase++;

			//�R�}���h���[�v������
			break;//(���̍s(POP)��ǂݍ��܂Ȃ��悤��)
		}
		//ITEM�R�}���h
		else if (word.find("ITEM") == 0)
		{
			//x���W
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//y���W
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//z���W
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//scale
			getline(line_stream, word, ',');
			bool right = (float)std::atof(word.c_str());

			itemManager->ItemGenerate({ x,y,z }, right);

			//�R�}���h���[�v������
			break;//(���̍s(POP)��ǂݍ��܂Ȃ��悤��)
		}
		//END�R�}���h
		else if(word.find("END") == 0)
		{
			//�I���t���O
			isEnd[1] = true;

			//�R�}���h���[�v������
			break;//(���̍s(POP)��ǂݍ��܂Ȃ��悤��)
		}
	}
}
