#include "EnemyManager.h"
using namespace std;

void EnemyManager::Initialize(Player* player, BulletManager* bulletManager, SoundData* soundData, ParticleManager* pManager, ItemManager* iManager)
{
	////敵発生コマンド
	// バッファをクリアします。
	enemyPopCommands.str("");
	// 状態をクリアします。
	enemyPopCommands.clear(std::stringstream::goodbit);

	//待機
	isWait = false;
	waitTimer = 0;

	enemies.clear();
	phase = 0;
	//phaseが変わるまで待つフラグ
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
	//球を生成、初期化
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Initialize(player, bulletManager, { pos.x,pos.y,pos.z },soundData);
	/*enemy->SetPlayer(player_);*/
	//球を登録
	enemies.push_back(std::move(enemy));
}

void EnemyManager::BossGenerate(const Vec3& pos, float& scale, int& HP)
{
	//球を生成、初期化
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->isBoss = true;
	enemy->scaletmp = scale;
	enemy->Initialize(player, bulletManager, { pos.x,pos.y,pos.z },soundData);
	enemy->radius_ *= scale;
	enemy->worldMat.scale = { 10.0f,10.0f,10.0f };
	enemy->worldMat.SetWorld();
	//enemy->radius_ /= 2;
	enemy->HP = HP;

	//球を登録
	enemies.push_back(std::move(enemy));
}

void EnemyManager::Enemy2Generate(const Vec3& pos)
{
	//球を生成、初期化
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->isEnemy2 = true;

	enemy->Initialize(player, bulletManager, { pos.x,pos.y,pos.z }, soundData);
	enemy->HP = 2;

	//球を登録
	enemies.push_back(std::move(enemy));
}

void EnemyManager::Update()
{
	//スクリプト発生処理
	UpdateEnemyPopCommands();

	//敵
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

	//敵消す
	enemies.remove_if([](std::unique_ptr<Enemy>& enemy)
		{
			return (enemy->IsDead() || enemy->IsAnnihilation());
		}
	);

	//フェーズ変更（こいつを倒したらフェーズ変更になる && 敵が0になったら）
	if ((isPhase) && enemies.size() == 0)
	{
		phase++;
		isPhase = false;
	}
	//敵が0になるまで待つのを解除
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
	//ファイル開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void EnemyManager::UpdateEnemyPopCommands()
{
	//待機処理
	if (isPhase || isEnd[0])
	{
		return;
	}

	if (isWait)
	{
		waitTimer--;
		
		if (waitTimer <= 0)
		{
			//待機終了
			isWait = false;
		}

		return;//コマンドループを実行させない
	}

	//1行分の文字列を入れる変数
	std::string line;

	//コマンド実行ループ
	while (getline(enemyPopCommands, line))
	{
		//1桁分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//[,]区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		//[//]から始まる行はコメント
		if (word.find("//") == 0)
		{
			continue;
		}

		//Enemy2コマンド
		if (word.find("POP2") == 0)
		{
			//x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			Enemy2Generate({ x,y,z });
		}
		//POPコマンド
		else if (word.find("POP") == 0)
		{
			//x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			
			//y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			
			//z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			EnemyGenerate({ x,y,z });
		}
		//Bossコマンド
		else if (word.find("BOSS") == 0)
		{
			//x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//z座標
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
		//WAITコマンド
		else if(word.find("WAIT") == 0)
		{
			getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機開始
			isWait = true;
			waitTimer = waitTime;

			//コマンドループ抜ける
			break;//(次の行(POP)を読み込まないように)
		}
		//ZEROコマンド(敵がゼロになるまで待つ)
		else if (word.find("ZERO") == 0)
		{
			//phaseが変わるまで待つフラグ
			isEnd[0] = true;
			//phase++;

			//コマンドループ抜ける
			break;//(次の行(POP)を読み込まないように)
		}
		//PHASEコマンド
		else if (word.find("PHASE") == 0)
		{
			//phaseが変わるまで待つフラグ
			isPhase = true;
			//phase++;

			//コマンドループ抜ける
			break;//(次の行(POP)を読み込まないように)
		}
		//ITEMコマンド
		else if (word.find("ITEM") == 0)
		{
			//x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//scale
			getline(line_stream, word, ',');
			bool right = (float)std::atof(word.c_str());

			itemManager->ItemGenerate({ x,y,z }, right);

			//コマンドループ抜ける
			break;//(次の行(POP)を読み込まないように)
		}
		//ENDコマンド
		else if(word.find("END") == 0)
		{
			//終了フラグ
			isEnd[1] = true;

			//コマンドループ抜ける
			break;//(次の行(POP)を読み込まないように)
		}
	}
}
