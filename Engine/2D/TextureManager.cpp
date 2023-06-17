#include "TextureManager.h"
#include <d3dx12.h>
#include "Util.h"
using namespace DirectX;

int32_t TextureManager::sCount_ = 0;
//���\�[�X�ݒ�
D3D12_RESOURCE_DESC TextureManager::sResDesc_;
//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
ComPtr < ID3D12DescriptorHeap> TextureManager::sSrvHeap_;
D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::sSrvHandle_;

ComPtr<ID3D12Resource> TextureManager::sTexBuff_[S_SRV_COUNT_];

//SRV�̍ő��
const int32_t TextureManager::S_K_MAX_SRV_COUNT_ = 2056;
//�f�X�N���v�^�q�[�v�̐ݒ�
D3D12_DESCRIPTOR_HEAP_DESC TextureManager::sSrvHeapDesc_;

D3D12_DESCRIPTOR_RANGE TextureManager::sDescriptorRange_;

uint64_t TextureManager::sWhiteTexHandle_ = 114514;

std::map < std::string, uint64_t> TextureManager::sTextureDatas_;

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
}

TextureManager& TextureManager::GetInstance()
{
	static TextureManager sInstance;

	return sInstance;
}

void TextureManager::InitializeDescriptorHeap()
{
	HRESULT result = {};

	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	sSrvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	sSrvHeapDesc_.NumDescriptors = S_K_MAX_SRV_COUNT_;
	sSrvHeapDesc_.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_�[���猩����悤��
	//desc�͐ݒ�
	result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(&sSrvHeapDesc_, IID_PPV_ARGS(sSrvHeap_.GetAddressOf()));
	assert(SUCCEEDED(result));


	//�f�X�N���v�^�����W�̐ݒ�
	sDescriptorRange_.NumDescriptors = 100;   //��x�̕`��Ɏg���e�N�X�`���̖���
	sDescriptorRange_.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	sDescriptorRange_.BaseShaderRegister = 0;  //�e�N�X�`�����W�X�^0��(t0)
	sDescriptorRange_.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	sSrvHandle_ = sSrvHeap_->GetCPUDescriptorHandleForHeapStart();
}


void TextureManager::LoadGraph(const wchar_t* name, uint64_t& textureHandle)
{
	assert(sCount_ <= S_SRV_COUNT_ - 1);

	HRESULT result = {};

	//�ǂݍ��܂�Ă��邩�ǂ���
	char namec[128] = {};
	ConstWCharTToChar(name, namec);

	std::string fileName = namec;
	{
		//�t�@�C��������T��
		std::map<std::string, uint64_t>::iterator it = sTextureDatas_.find(fileName);
		//���łɓǂݍ��܂�Ă����炻�̃n���h����Ԃ�
		if (it != sTextureDatas_.end())
		{
			textureHandle = it->second;
			return;
		}
	}

	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WIC�̃e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		name,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	//mipmap����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);


	//�R�s�[��p
	// �q�[�v�̐ݒ�
	CD3DX12_HEAP_PROPERTIES textureHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC textureResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			(uint64_t)metadata.width,
			(uint32_t)metadata.height,
			(uint16_t)metadata.arraySize,
			(uint16_t)metadata.mipLevels,
			1);

	// �e�N�X�`���o�b�t�@�̐���
	result = DirectXWrapper::GetInstance().GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&sTexBuff_[sCount_]));
	assert(SUCCEEDED(result));


	//�A�b�v���[�h�o�b�t�@
	// �q�[�v�̐ݒ�
	uint64_t uploadSize = GetRequiredIntermediateSize(sTexBuff_[sCount_].Get(), 0, (uint32_t)metadata.mipLevels);

	D3D12_HEAP_PROPERTIES uploadHeapProp{};
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC resDesc =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuff;

	// �e�N�X�`���o�b�t�@�̐���
	result = DirectXWrapper::GetInstance().GetDevice()->
		CreateCommittedResource(
			&uploadHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuff));
	assert(SUCCEEDED(result));


	//�T�u���\�[�X����
	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	for (int32_t i = 0; i < subResourcesDatas.size(); i++)
	{
		// �S�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* IMG = scratchImg.GetImage(i, 0, 0);

		subResourcesDatas[i].pData = IMG->pixels;
		subResourcesDatas[i].RowPitch = IMG->rowPitch;
		subResourcesDatas[i].SlicePitch = IMG->slicePitch;
	}

	//�T�u���\�[�X��]��
	UpdateSubresources(
		DirectXWrapper::GetInstance().GetCommandList(),
		sTexBuff_[sCount_].Get(),
		uploadBuff.Get(),
		0,
		0,
		(uint32_t)metadata.mipLevels,
		subResourcesDatas.data()
	);

	//SRV
	{
		//SRV�q�[�v�̐擪�n���h�����擾
		if (sCount_ == 0) { sSrvHandle_ = sSrvHeap_->GetCPUDescriptorHandleForHeapStart(); }
		else { sSrvHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(sSrvHeapDesc_.Type); }

		//04_03
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping =
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
		srvDesc.Texture2D.MipLevels = 1;
		//�n���h���̂����ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(TextureManager::GetInstance().sTexBuff_[sCount_].Get(), &srvDesc, sSrvHandle_);

		//04_02(�摜�\��p�̃A�h���X��������)
		//SRV�q�[�v�̃n���h�����擾
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = sSrvHeap_->GetGPUDescriptorHandleForHeapStart();
		textureHandle = srvGpuHandle.ptr + (DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(TextureManager::GetInstance().sSrvHeapDesc_.Type) * sCount_);

		{
			//���O�ƃf�[�^��R�Â��ĕۑ�
			//�T�E���h�f�[�^��A�z�z��Ɋi�[(�������ăZ�b�g�Ń}�b�v�Ɋi�[)
			sTextureDatas_.insert(std::make_pair(fileName, textureHandle));
		}

		//���f�[�^���
		scratchImg.Release();
	}

	//�o���A�ƃt�F���X
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = sTexBuff_[sCount_].Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;//�������d�v
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;//�������d�v

	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &BarrierDesc);
	DirectXWrapper::GetInstance().GetCommandList()->Close();

	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdlists[] = { DirectXWrapper::GetInstance().GetCommandList() };
	DirectXWrapper::GetInstance().GetCommandQueue()->ExecuteCommandLists(1, cmdlists);

	//�R�}���h����
	DirectXWrapper::GetInstance().GetCommandQueue()->Signal(DirectXWrapper::GetInstance().GetFence(),
		++DirectXWrapper::GetInstance().GetFenceVal());

	if (DirectXWrapper::GetInstance().GetFence()->GetCompletedValue() != DirectXWrapper::GetInstance().GetFenceVal())
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		DirectXWrapper::GetInstance().GetFence()->SetEventOnCompletion(DirectXWrapper::GetInstance().GetFenceVal(), event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//�o�b�t�@���
	/*uploadBuff->Release();
	uploadBuff.Reset();*/

	//�R�}���h���Z�b�g
	DirectXWrapper::GetInstance().CommandReset();


	//�o�b�t�@�p�̃J�E���g
	sCount_++;
}