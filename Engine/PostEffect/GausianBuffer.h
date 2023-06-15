#pragma once
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include "WindowsApp.h"
#include <DirectXMath.h>
#include <DirectXTex.h>
#include "KeyboardInput.h"
using namespace DirectX;
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include"Sound.h"
#include <wrl.h>
using namespace Microsoft::WRL;
#include<chrono>


class GausianBuffer
{
private:


public:
	GausianBuffer();

	void Initialize(D3D12_CPU_DESCRIPTOR_HANDLE& peraHandle,  ID3D12Device& device,
		const D3D12_DESCRIPTOR_HEAP_DESC& heapDesc);

	std::vector<float> weights_;

	ComPtr <ID3D12Resource> buff_;
};


//ガウシアンぼかし
std::vector<float> GetGaussianWeights(int32_t count, float s);

