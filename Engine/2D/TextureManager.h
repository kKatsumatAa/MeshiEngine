#pragma once
#include "DirectXWrapper.h"
#include <map>


class TextureManager
{
private:
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//�i�[��
	static const std::string sDirectoryPath_;

	static int32_t sCount_;

	//�e�N�X�`���f�[�^�̘A�z�z��
	static std::map < std::string, uint64_t> sTextureDatas_;

	TextureManager();

	~TextureManager();


private:
	//�e�N�X�`�����Ȃ��ꍇ�̕W���e�N�X�`���t�@�C����
	static const std::string S_DEFAULT_TEX_FILE_NAME_;

	static uint64_t sWhiteTexHandle_;

	//���\�[�X�ݒ�
	static D3D12_RESOURCE_DESC sResDesc_;
	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	static  ComPtr < ID3D12DescriptorHeap> sSrvHeap_;
	static D3D12_CPU_DESCRIPTOR_HANDLE sSrvHandle_;

	static const int32_t S_SRV_COUNT_ = 512;
	static ComPtr<ID3D12Resource> sTexBuff_[S_SRV_COUNT_];

	//SRV�̍ő��
	static const int32_t S_K_MAX_SRV_COUNT_;
	//�f�X�N���v�^�q�[�v�̐ݒ�
	static D3D12_DESCRIPTOR_HEAP_DESC sSrvHeapDesc_;

	//�f�X�N���v�^�����W�̐ݒ�
	static D3D12_DESCRIPTOR_RANGE sDescriptorRange_;

public:
	//�R�s�[�R���X�g���N�^�𖳌�
	TextureManager(const TextureManager& obj) = delete;
	//������Z�q��
	TextureManager& operator=(const TextureManager& obj) = delete;

	static TextureManager& GetInstance();

	//�f�X�N���v�^�q�[�v������
	static void InitializeDescriptorHeap();
	static void Initialize();
	static uint64_t LoadGraph(const char* name, ID3D12Resource** texBuff = nullptr,
		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr, D3D12_CPU_DESCRIPTOR_HANDLE* srvHandle = nullptr);

	static void AddSRVHandleCount() { sCount_++; }

	//�e�N�X�`���n���h�������������ĂȂ������甒���摜�̃n���h�������
	static void CheckTexHandle(uint64_t& texHandle);

public:
	static ID3D12DescriptorHeap** GetDescHeapPP() { return sSrvHeap_.GetAddressOf(); }
	static ID3D12DescriptorHeap* GetDescHeapP() { return sSrvHeap_.Get(); }
	static const D3D12_DESCRIPTOR_RANGE& GetDescRange() { return sDescriptorRange_; }
	static uint64_t GetWhiteTexHandle() { return sWhiteTexHandle_; }
	static int32_t GetSRVCount() { return sCount_; }
	static ComPtr<ID3D12Resource>* GetTexBuff() { return sTexBuff_; }
	static const D3D12_RESOURCE_DESC& GetResDesc() { return sResDesc_; }
	static const D3D12_DESCRIPTOR_HEAP_DESC & GetHeapDesc() { return sSrvHeapDesc_; }

};

