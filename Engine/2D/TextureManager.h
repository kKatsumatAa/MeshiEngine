#pragma once
#include "DirectXWrapper.h"
#include <map>


class TextureManager
{
private:
	static int32_t sCount_;

	//�e�N�X�`���f�[�^�̘A�z�z��
	static std::map < std::string, uint64_t> sTextureDatas_;

	TextureManager();

	~TextureManager();


public:
	static uint64_t sWhiteTexHandle_;


	//���\�[�X�ݒ�
	static D3D12_RESOURCE_DESC sResDesc_;
	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	static ComPtr < ID3D12DescriptorHeap> sSrvHeap_;
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
	static void LoadGraph(const wchar_t* P_NAME, uint64_t& textureHandle);

	static void AddSrvHandleCount() { sCount_++; }
};

