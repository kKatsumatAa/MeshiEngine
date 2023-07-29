#pragma once
#include<memory>
#include<list>
#include<string>

class TutorialState;

class Tutorial final
{
private:
	std::unique_ptr<TutorialState> state_ = nullptr;

	//���̃X�e�[�g�̖��O
	std::list<std::string> nextStateNames_;

	//�t�H���_��
	const std::string FOLDER_NAME_ = "Resources/tutorialData/";
	const std::string FILE_NAME_ = "tutorial";

public:


private:
	Tutorial() { ; }
	~Tutorial() { ; }

public:
	Tutorial& operator=(const Tutorial& inst) = delete;
	Tutorial(const Tutorial& inst) = delete;

	static Tutorial& GetInstance();

private:
	void ChangeState(const std::string& stateName);

public:
	//���̃X�e�[�g
	void ToNextState();

	//�X�e�[�W�̃`���[�g���A���ǂݍ���
	void LoadTutorialData(int32_t fileIndex);

public:
	void Initialize();
	void Update();
	void Draw();

};