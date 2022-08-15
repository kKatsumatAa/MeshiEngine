#include "EnemyManager.h"
using namespace std;

void EnemyManager::Initialize(Player* player, BulletManager* bulletManager, SoundData* soundData)
{
	this->soundData = soundData;
	this->player = player;
	this->bulletManager = bulletManager;
}

void EnemyManager::EnemyGenerate(const Vec3& pos)
{
	//���𐶐��A������
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Initialize(player, bulletManager, { pos.x,pos.y,pos.z });
	/*enemy->SetPlayer(player_);*/
	//����o�^
	enemies.push_back(std::move(enemy));
}

void EnemyManager::Update()
{
	//�X�N���v�g��������
	UpdateEnemyPopCommands();

	//�e
	for (std::unique_ptr<Enemy>& enemy : enemies)
	{
		enemy->Update();
		if(enemy->IsDead()) SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[3], 10.0f);
	}
	
	//�e������
	enemies.remove_if([](std::unique_ptr<Enemy>& enemy)
		{
			return (enemy->IsDead() || enemy->IsAnnihilation());
		}
	);
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

		//POP�R�}���h
		if (word.find("POP") == 0)
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
	}
}
