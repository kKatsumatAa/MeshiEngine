#include "Waves.h"

Waves::Waves()
{
	//ヒープ設定
	uint32_t sizeVB;
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	// 頂点データ全体のサイズ
	sizeVB = static_cast<uint32_t>(sizeof(WavesBuff));
	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//定数バッファの生成
	BuffProperties(heapProp, resDesc, wavesBuff_.GetAddressOf());
	HRESULT result;
	result = wavesBuff_->Map(0, nullptr, (void**)&mapWavesBuff_);
	assert(SUCCEEDED(result));
}

void Waves::SetBuffCmdLst(int32_t rootParamIndex)
{
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(rootParamIndex, wavesBuff_->GetGPUVirtualAddress());
}

void Waves::Update(float speed)
{
	for (auto& wave : waves_)
	{
		wave.Update(speed);

		if (wave.GetIsEndWave())
		{
			wave.End();
		}
	}

	//マッピング
	for (int i = 0; i < WAVE_NUM_; i++)
	{
		mapWavesBuff_->waves[i].distance = waves_[i].GetDistance();
		mapWavesBuff_->waves[i].epicenter = { waves_[i].GetEpicenter().x,waves_[i].GetEpicenter().y,waves_[i].GetEpicenter().z };
		mapWavesBuff_->waves[i].thickness = { waves_[i].GetThickness().x,waves_[i].GetThickness().y };
	}
}

void Waves::BeginWave(const Vec3& epicenter, const Vec2& thickness, float distanceMax, float timer)
{
	waves_[SearchUsableWaveNum()].BeginWave(epicenter, thickness, distanceMax, timer);
}

int32_t Waves::SearchUsableWaveNum()
{
	for (int32_t i = 0; i < WAVE_NUM_; i++)
	{
		if (waves_[i].GetIsEndWave())
		{
			return i;
		}
	}

	return 0;
}
