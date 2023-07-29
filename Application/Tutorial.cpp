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
	//���O�ŃX�e�[�g�ς���
	ChangeState(name);
}

void Tutorial::LoadTutorialData(int32_t fileIndex)
{
	nextStateNames_.clear();

	const std::string FILE_NAME = FILE_NAME_ + std::to_string(fileIndex) + ".csv";

	// �t�@�C���X�g���[��
	std::ifstream file;
	//�t�@�C�����J��
	std::string fullPath = FOLDER_NAME_ + FILE_NAME;
	file.open(fullPath);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	// 1�s���ǂݍ���
	std::string line;
	while (getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		std::getline(line_stream, key, ' ');

		//�擪��t��������`���[�g���A���̖��O
		if (key == "t" || key == "T")
		{
			nextStateNames_.emplace_back();
			std::string& name = nextStateNames_.back();
			line_stream >> name;
		}
	}

	//�`���[�g���A���̏�����
	Initialize();
}


//-------------------------------------------------------------------------------------
void Tutorial::Initialize()
{
	TextureManager::GetInstance().LoadGraph(L"tutorial1.png");
	TextureManager::GetInstance().LoadGraph(L"tutorial2.png");
	TextureManager::GetInstance().LoadGraph(L"tutorial3.png");

	//��Ԗڂ̃X�e�[�g��
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
