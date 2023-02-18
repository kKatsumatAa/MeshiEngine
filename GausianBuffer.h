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


//�K�E�V�A���ڂ���
static std::vector<float> GetGaussianWeights(size_t count, float s)
{
	std::vector<float> weights(count); // �E�F�C�g �z�� �ԋp �p 
	float x = 0.0f;
	float total = 0.0f;
	for (auto& wgt : weights)
	{
		wgt = expf(-(x * x) / (2.0f * s * s));
		total += wgt;
		x += 1.0f;
	}
	total = total * 2.0f - 1.0f;

	// ���� �� 1 �� �Ȃ� �悤 �� ���� 
	for (auto& wgt : weights)
	{
		wgt /= total;
	}

	return weights;
}
