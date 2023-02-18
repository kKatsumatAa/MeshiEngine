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


};


//ガウシアンぼかし
static std::vector<float> GetGaussianWeights(size_t count, float s)
{
	std::vector<float> weights(count); // ウェイト 配列 返却 用 
	float x = 0.0f;
	float total = 0.0f;
	for (auto& wgt : weights)
	{
		wgt = expf(-(x * x) / (2.0f * s * s));
		total += wgt;
		x += 1.0f;
	}
	total = total * 2.0f - 1.0f;

	// 足し て 1 に なる よう に する 
	for (auto& wgt : weights)
	{
		wgt /= total;
	}

	return weights;
}
