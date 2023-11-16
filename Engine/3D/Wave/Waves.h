#pragma once
#include"Wave.h"
#include "DirectXWrapper.h"


//複数の波を管理
class Waves
{
private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//最大数
	static const int32_t WAVE_NUM_ = 10;

	struct WaveBuff
	{
		DirectX::XMFLOAT3 epicenter; //震源地
		float distance; //波の距離
		DirectX::XMFLOAT2 thickness; //波の太さ
		float padding[2] = { 0 };
	};

	struct WavesBuff
	{
		float tessFactor = 5.0f;
		float padding[3] = { 0 };
		WaveBuff waves[WAVE_NUM_];
	};

private:
	//バッファ
	ComPtr <ID3D12Resource> wavesBuff_;
	WavesBuff* mapWavesBuff_;

private:
	//波のインスタンス
	Wave waves_[WAVE_NUM_];

public:
	Waves();

	void SetBuffCmdLst(int32_t rootParamIndex);

public:
	//更新
	void Update();
	//波を発生
	void BeginWave(const Vec3& epicenter, const Vec2& thickness, float distanceMax, float timer);

	//分割数
	void SetTessFactor(float tessFactor) { mapWavesBuff_->tessFactor = tessFactor; }

private:
	//使えるウェーブの添え字を返す
	int32_t SearchUsableWaveNum();
};