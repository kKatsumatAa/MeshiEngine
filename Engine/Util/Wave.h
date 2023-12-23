/*
* @file Wave.h
* @brief 波紋
*/

#pragma once
#include"Vec2.h"
#include"Vec3.h"

//波
class Wave
{
private:
	Vec3 epicenter_ = { 0,0,0 };//震源地
	Vec2 thickness_ = { 0,0 };//太さ
	Vec2 thicknessMax_ = { 0,0 };//太さ
	float distance_ = 0;//波の震源地からの距離
	float distanceMax_ = 0;//波の震源地からの距離

	float timer_ = 0;
	float timerMax_ = 0;

public:
	//震源地セット
	void SetEpicenter(const Vec3& pos) { epicenter_ = pos; }
	//震源地取得
	const Vec3& GetEpicenter() { return epicenter_; }
	//波の太さセット
	void SetThickness(const Vec2& thickness) { thickness_ = thickness; }
	//波の太さ取得
	const Vec2& GetThickness() { return thickness_; }
	//波の震源地からの距離セット
	void SetDistance(float distance) { distance_ = distance; }
	//波の震源地からの距離取得
	float GetDistance() { return distance_; }

public:
	//波を発生
	void BeginWave(const Vec3& epicenter, const Vec2& thickness, float distanceMax, float timer);
	//更新
	void Update(float speed);
	//波終わり
	void End();

public:
	//ウェーブが終わったどうか
	bool GetIsEndWave();
};