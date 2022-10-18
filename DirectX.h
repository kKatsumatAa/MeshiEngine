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

class Directx
{
private:
	//���ʕ��n
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr < ID3D12CommandQueue> commandQueue = nullptr;
	// �X���b�v�`�F�[���̐ݒ�
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	// �f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ComPtr < ID3D12DescriptorHeap> rtvHeap = nullptr;
	// 2.�`���̕ύX
		// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	// �o�b�N�o�b�t�@
	std::vector< ComPtr <ID3D12Resource>> backBuffers;
	//
	ComPtr < ID3D12DescriptorHeap> dsvHeap = nullptr;
	
	// �t�F���X�̐���
	ComPtr < ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;

	// 1.���\�[�X�o���A�ŏ������݉\�ɕύX
	D3D12_RESOURCE_BARRIER barrierDesc{};
	
	//��
	IXAudio2MasteringVoice* masterVoice;


	Directx();

	//�������n
	void InitializeDevice();
	void InitializeCommand();
	void InitializeSwapchain();
	void InitializeRendertargetView();
	void InitializeDepthBuffer();
	void InitializeFence();

public:
	//��
	ComPtr<IXAudio2> xAudio2;
	//
	HRESULT result;
	ComPtr<ID3D12Device> device = nullptr;
	ComPtr < ID3D12GraphicsCommandList> commandList = nullptr;
	



	//�֐�
	static Directx& GetInstance();

	void DrawInitialize();

	void DrawUpdate(const XMFLOAT4& winRGBA = { 0.1f,0.25f,0.5f,0.0f });

	void DrawUpdate2();

	
};