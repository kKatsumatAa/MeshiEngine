/*
* @file Tutorial.h
* @brief チュートリアルの画像や処理
*/

/*
* @file Tutorial.h
* @brief チュートリアルの画像や処理
*/

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


private:
	//コンストラクタ
	Tutorial() { ; }
	//デストラクタ
	~Tutorial() { ; }

public:
	//コピーコンストラクタ禁止
	Tutorial& operator=(const Tutorial& inst) = delete;
	//コピーコンストラクタ禁止
	Tutorial(const Tutorial& inst) = delete;

	//インスタンス取得
	static Tutorial& GetInstance();

private:
	//ステート変更
	void ChangeState(const std::string& stateName);

public:
	//次のステートに
	void ToNextState();

	//ステージのチュートリアル読み込み
	void LoadTutorialData(int32_t fileIndex);

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();

};