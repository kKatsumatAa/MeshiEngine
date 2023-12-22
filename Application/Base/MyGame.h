/*
* @file MyGame.h
* @brief 作品ごとに異なるゲーム内の処理
*/

#pragma once
#include"Framework.h"


//ゲーム全体
class MyGame : public Framework
{
public:
	//初期化
	void Initialize() override;

	//終了処理
	void Finalize() override;

	//更新処理
	void Update() override;

	//描画
	void Draw() override;
};

