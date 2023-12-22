/*
* @file Counter.h
* @brief カウントするクラス
*/

#pragma once
#include<memory>


class Counter
{
private:
	//フレームカウント
	uint64_t count_ = 0;
	//カウント上限
	uint64_t countMax_ = 0;

public:
	//加算
	void AddCount(uint64_t addCount = 1) { count_ += addCount; }
	//リセット
	void ResetCount() { count_ = 0; }

public:
	//代入
	void SetCount(uint64_t count) { count_ = count; }
	//上限代入
	void SetCountMax(uint64_t countMax) { countMax_ = countMax; }

public:
	//上限以上になったか
	bool GetExceed() { return count_ >= countMax_; }
	//カウント取得
	int64_t GetCount() { return count_; }
};