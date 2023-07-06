#pragma once
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include "WindowsApp.h"
#include <DirectXMath.h>
#include <DirectXTex.h>
#include "KeyboardInput.h"
#include <d3dcompiler.h>
#include"Sound.h"
#include <wrl.h>
#include<chrono>


class GausianBuffer
{
private:


public:
	GausianBuffer();

	void Initialize(D3D12_CPU_DESCRIPTOR_HANDLE& peraHandle,  ID3D12Device& device,
		const D3D12_DESCRIPTOR_HEAP_DESC& heapDesc);

	std::vector<float> weights_;

	Microsoft::WRL::ComPtr <ID3D12Resource> buff_;
};


//ƒKƒEƒVƒAƒ“‚Ú‚©‚µ
std::vector<float> GetGaussianWeights(int32_t count, float s);

