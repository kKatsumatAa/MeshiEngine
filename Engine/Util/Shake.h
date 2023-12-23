/*
* @file Shake.h
* @brief シェイクの処理
*/

#pragma once
#include"Util.h"
#include<random>

//乱数シード生成器
static std::random_device sSeedGen;
//メルセンヌツイスター
static std::mt19937_64 sEngine(sSeedGen());

class Shake
{
private:
	float shakeLength_ = 0.0f;
	int32_t   lifeTimer_ = 0;
	int32_t   lifeTimerTmp_ = 0;
	float t_ = 0.0f;


public:
	//初期化
	void Initialize();
	//更新
	void Update();

private:
	//ランダムな数値取得
	float GetRandom(float min, float max);
	//シェイクの幅セット
	void SetShakeLength(float length);
	//シェイク時間セット
	void SetLifeTime(int32_t time);

public:
	//シェイク始める
	void BeginShake(int32_t time, float length);
	//シェイクの大きさ取得
	float GetShake();
	//シェイクしているか取得
	bool GetIsShaking();
};

