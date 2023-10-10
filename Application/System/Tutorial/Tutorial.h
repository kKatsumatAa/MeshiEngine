#pragma once
#include<memory>
#include<list>
#include<string>

class TutorialState;

class Tutorial final
{
private:
	std::unique_ptr<TutorialState> state_ = nullptr;

	//次のステートの名前
	std::list<std::string> nextStateNames_;

	//フォルダ名
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
	//次のステート
	void ToNextState();

	//ステージのチュートリアル読み込み
	void LoadTutorialData(int32_t fileIndex);

public:
	void Initialize();
	void Update();
	void Draw();

};