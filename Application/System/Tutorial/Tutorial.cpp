#include "Tutorial.h"
#include "TutorialState.h"
#include <fstream>
#include <sstream>



Tutorial& Tutorial::GetInstance()
{
	static Tutorial sInst;
	return sInst;
}

void Tutorial::ChangeState(const std::string& stateName)
{
	state_.reset();
	state_ = std::move(TutorialState::GetState(stateName));
	state_->SetTutorial(this);
	state_->Initialize();
}

void Tutorial::ToNextState()
{
	std::string name = "";
	if (nextStateNames_.size())
	{
		name = nextStateNames_.front();
		nextStateNames_.pop_front();
	}
	//名前でステート変える
	ChangeState(name);
}

void Tutorial::LoadTutorialData(int32_t fileIndex)
{
	nextStateNames_.clear();

	const std::string FILE_NAME = FILE_NAME_ + std::to_string(fileIndex) + ".csv";

	// ファイルストリーム
	std::ifstream file;
	//ファイルを開く
	std::string fullPath = FOLDER_NAME_ + FILE_NAME;
	file.open(fullPath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(false);
	}

	// 1行ずつ読み込む
	std::string line;
	while (getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		std::string key;
		std::getline(line_stream, key, ' ');

		//先頭がtだったらチュートリアルの名前
		if (key == "t" || key == "T")
		{
			nextStateNames_.emplace_back();
			std::string& name = nextStateNames_.back();
			line_stream >> name;
		}
	}

	//チュートリアルの初期化
	Initialize();
}


//-------------------------------------------------------------------------------------
void Tutorial::Initialize()
{
	TextureManager::GetInstance().LoadGraph("tutorial1.dds");
	TextureManager::GetInstance().LoadGraph("tutorial2.dds");
	TextureManager::GetInstance().LoadGraph("tutorial3.dds");

	//一番目のステートに
	ToNextState();
}

void Tutorial::Update()
{
	state_->Update();
}

void Tutorial::Draw()
{
	state_->Draw();
}
