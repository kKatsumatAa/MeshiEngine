#pragma once
#include "GausianBuffer.h"
#include <array>


class DirectXWrapper final
{
private:
	//���ʕ��n
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr < ID3D12CommandQueue> commandQueue = nullptr;
	ComPtr<ID3D12Resource> depthBuff;
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



	//�O���ŎQ��
	ComPtr<ID3D12Device> device = nullptr;
	ComPtr < ID3D12GraphicsCommandList> commandList = nullptr;

	//�L�^���ԁiFPS�Œ�
	std::chrono::steady_clock::time_point reference_;

	FLOAT clearColor[4] = { 0,0,0,0 };


private:
	DirectXWrapper();
	~DirectXWrapper();

	//�������n
	void InitializeDevice();
	void InitializeCommand();
	void InitializeSwapchain();
	void InitializeRendertargetView();
	void InitializeDepthBuffer();
	void InitializeFence();

	//FPS�Œ菉����
	void InitializeFixFPS();
	//FPS�Œ�X�V
	void UpdateFixFPS();

	//�K���X
	bool CreateEffectBufferAndView(const wchar_t* fileName);

public:
	//
	HRESULT result;

	void Initialize();


	//�R�s�[�R���X�g���N�^�𖳌�
	DirectXWrapper(const DirectXWrapper& obj) = delete;
	//������Z�q��
	DirectXWrapper& operator=(const DirectXWrapper& obj) = delete;

	//�֐�
	static DirectXWrapper& GetInstance();

	void CommandReset();

	void DrawInitialize();

	//1���ڂ̃e�N�X�`���ɕ`��
	void PreDraw(const XMFLOAT4& winRGBA = { 0,0,0,0 });
	void PostDraw();


	//getter
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	//ComPtr<ID3D12DescriptorHeap> GetPeraSRVHeap() const { return _peraSRVHeap; }
	ID3D12CommandQueue* GetCommandQueue()const { return commandQueue.Get(); }
	UINT64& GetFenceVal() { return fenceVal; }
	ID3D12Fence* GetFence() { return fence.Get(); }
	ID3D12DescriptorHeap* GetRtvheap() { return rtvHeap.Get(); }
	FLOAT* GetClearColor() { return clearColor; }
	std::vector< ComPtr <ID3D12Resource>>& GetBackBuffer() { return backBuffers; }
	ID3D12DescriptorHeap* GetDSVHeap() { return dsvHeap.Get(); }

	//�o�b�N�o�b�t�@�̐����擾
	size_t GetBackBufferCount() const { return backBuffers.size(); }
};
//�摜�̃��[�h�i�����Ƀo�b�t�@�ݒ�j
void LoadPictureFromFile(const wchar_t* fileName, ComPtr<ID3D12Resource>& texBuff);

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size);

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
	ID3D12Resource** buff);


