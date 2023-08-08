#pragma once
#include "GausianBuffer.h"
#include <array>
#include <list>


class DirectXWrapper final
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//���ʕ��n
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	ComPtr < ID3D12CommandQueue> commandQueue_ = nullptr;
	ComPtr < ID3D12CommandQueue> texCommandQueue_ = nullptr;
	ComPtr<ID3D12Resource> depthBuff_;
	// �X���b�v�`�F�[���̐ݒ�
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	ComPtr<IDXGISwapChain4> swapChain_;
	// �f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc_{};
	ComPtr < ID3D12DescriptorHeap> rtvHeap_ = nullptr;
		// �����_�[�^�[�Q�b�g�r���[�̃n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
	// �o�b�N�o�b�t�@
	std::vector< ComPtr <ID3D12Resource>> backBuffers_;
	//�[�x�o�b�t�@�r���[�p
	ComPtr < ID3D12DescriptorHeap> dsvHeap_ = nullptr;
	//�[�x�l�e�N�X�`���p
	ComPtr < ID3D12DescriptorHeap> depthSRVHeap_ = nullptr;

	// �t�F���X�̐���
	ComPtr < ID3D12Fence> fence_ = nullptr;
	uint64_t fenceVal_ = 0;
	ComPtr < ID3D12Fence> texFence_ = nullptr;
	uint64_t texFenceVal_ = 0;

	// 1.���\�[�X�o���A�̐ݒ�
	D3D12_RESOURCE_BARRIER barrierDesc_{};

	//�f�o�C�X
	ComPtr<ID3D12Device> device_ = nullptr;

	ComPtr < ID3D12CommandAllocator> commandAllocator_ = nullptr;
	ComPtr < ID3D12GraphicsCommandList> commandList_ = nullptr;
	ComPtr < ID3D12CommandAllocator> texCommandAllocator_ = nullptr;
	ComPtr < ID3D12GraphicsCommandList> texCommandList_ = nullptr;

	//�L�^���ԁiFPS�Œ�
	std::chrono::steady_clock::time_point reference_;

	float clearColor_[4] = { 0,0,0,0 };

	//
	HRESULT result_;

	//�e�N�X�`���A�b�v���[�h�p�̃o�b�t�@
	std::list<ComPtr<ID3D12Resource>> texUploadBuff_;

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

	//�R�}���h�n�̍쐬
	void CreateCommandLA(ID3D12CommandAllocator** texCommandAllocator,
		ID3D12GraphicsCommandList** texCommandList);

public:
	void Initialize();


	//�R�s�[�R���X�g���N�^�𖳌�
	DirectXWrapper(const DirectXWrapper& obj) = delete;
	//������Z�q��
	DirectXWrapper& operator=(const DirectXWrapper& obj) = delete;

	//�֐�
	static DirectXWrapper& GetInstance();

	void CommandReset();

	void DrawInitialize();

	//�e�N�X�`����`��Ƃ͕ʂ̃R�}���h���X�g�ŃA�b�v����R�}���h
	void UpLoadTexture();

	//1���ڂ̃e�N�X�`���ɕ`��
	void PreDraw();
	void PostDraw();

	//���\�[�X�o���A
	void ResourceBarrier(D3D12_RESOURCE_STATES beforeState,
		D3D12_RESOURCE_STATES afterState,ID3D12Resource* buff );

	//getter
	ID3D12Device* GetDevice() const { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	ID3D12GraphicsCommandList* GetTexCommandList() const { return texCommandList_.Get(); }
	ID3D12CommandQueue* GetCommandQueue()const { return commandQueue_.Get(); }
	uint64_t& GetFenceVal() { return fenceVal_; }
	ID3D12Fence* GetFence() { return fence_.Get(); }
	ID3D12DescriptorHeap* GetRtvheap() { return rtvHeap_.Get(); }
	float* GetClearColor() { return clearColor_; }
	std::vector< ComPtr <ID3D12Resource>>& GetBackBuffer() { return backBuffers_; }
	ID3D12DescriptorHeap* GetDSVHeap() { return dsvHeap_.Get(); }
	ID3D12Resource* GetDepthBuff() { return depthBuff_.Get(); }
	ID3D12DescriptorHeap* GetDepthSRVHeap() { return depthSRVHeap_.Get(); }
	ID3D12DescriptorHeap** GetDepthSRVHeapPP() { return depthSRVHeap_.GetAddressOf(); }
	ID3D12Resource** GetTexUploadBuffPP() { return texUploadBuff_.back().GetAddressOf(); }
	ID3D12Resource* GetTexUploadBuffP() { return texUploadBuff_.back().Get(); }
	void UploatBuffEmplaceBack() { return texUploadBuff_.emplace_back(); }

	//�o�b�N�o�b�t�@�̐����擾
	size_t GetBackBufferCount() const { return backBuffers_.size(); }
};
//�摜�̃��[�h�i�����Ƀo�b�t�@�ݒ�j
void LoadPictureFromFile(const char* fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texBuff);

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, uint32_t size);

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
	ID3D12Resource** buff);


