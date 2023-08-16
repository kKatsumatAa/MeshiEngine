#include "TextureManager.h"
#include <d3dx12.h>
#include "Util.h"
using namespace DirectX;
using namespace Microsoft::WRL;


//�i�[��
const std::string TextureManager::sDirectoryPath_ = "Resources/image/";

const std::string TextureManager::S_DEFAULT_TEX_FILE_NAME_ = "white.png";

int32_t TextureManager::sCount_ = 0;
//���\�[�X�ݒ�
D3D12_RESOURCE_DESC TextureManager::sResDesc_;
//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
ComPtr < ID3D12DescriptorHeap> TextureManager::sSrvHeap_;
D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::sSrvHandle_;

ComPtr<ID3D12Resource> TextureManager::sTexBuff_[S_SRV_COUNT_];

//SRV�̍ő��
const int32_t TextureManager::S_K_MAX_SRV_COUNT_ = 2056 * 2 * 2 * 2 * 2;
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
	sDescriptorRange_.NumDescriptors = 1;   //��x�̕`��Ɏg���e�N�X�`���̖���
	sDescriptorRange_.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	sDescriptorRange_.BaseShaderRegister = 0;  //�e�N�X�`�����W�X�^0��(t0)
	sDescriptorRange_.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	sSrvHandle_ = sSrvHeap_->GetCPUDescriptorHandleForHeapStart();
}

void TextureManager::Initialize()
{
	//�����摜
	TextureManager::GetInstance().sWhiteTexHandle_ = TextureManager::LoadGraph("white.png");
}


uint64_t TextureManager::LoadGraph(const char* name, ID3D12Resource** texBuff,
	D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, D3D12_CPU_DESCRIPTOR_HANDLE* srvHandle)
{
	assert(sCount_ <= S_SRV_COUNT_ - 1);

	HRESULT result = {};

	//�o�b�t�@���Z�b�g����Ă����炻���ɍ쐬����
	ID3D12Resource** texBuffL = texBuff;
	if (texBuffL == nullptr)
	{
		texBuffL = &sTexBuff_[sCount_];
	}

	//�ǂݍ��܂�Ă��邩�ǂ���
	std::string fileName = name;

	//�i�[�悪�w�肳��ĂȂ����image���ɂ���̂�
	if (fileName.find("Resources") == std::string::npos)
	{
		fileName = sDirectoryPath_ + fileName;
	}

	//wchar_t�ɖ߂�
	wchar_t nameWc[128] = {};
	ConstCharToWcharT(fileName.c_str(), nameWc);

	//srvHandle���w�肳��Ă�����V����srv���O��Ȃ̂�handle�͕ʂ̂��̂ɂ��邽��
	if (texBuff == nullptr && srvDesc == nullptr && srvHandle == nullptr)
	{
		//�t�@�C��������T��
		std::map<std::string, uint64_t>::iterator it = sTextureDatas_.find(fileName);
		//���łɓǂݍ��܂�Ă����炻�̃n���h����Ԃ�
		if (it != sTextureDatas_.end())
		{
			return it->second;
		}
	}

	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WIC�̃e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		nameWc,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	//���s�����甒���摜�ǂݍ��܂��Ĕ�����
	if (!SUCCEEDED(result))
	{
		return LoadGraph(S_DEFAULT_TEX_FILE_NAME_.c_str());
	}

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
			IID_PPV_ARGS(texBuffL));
	assert(SUCCEEDED(result));

	//	FootPrint�擾
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	UINT64 total_bytes = 0;
	DirectXWrapper::GetInstance().GetDevice()->GetCopyableFootprints(
		&textureResourceDesc, 0, 1, 0, &footprint, nullptr, nullptr, &total_bytes);

#pragma region Upload
	D3D12_RESOURCE_DESC uploadResDesc{};

	//�e�N�X�`�����A�b�v���[�h����o�b�t�@�̃��X�g�ɐ�ɐς�ł���
	DirectXWrapper::GetInstance().UploatBuffEmplaceBack();

	uploadResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadResDesc.Width = total_bytes;
	uploadResDesc.Height = 1;
	uploadResDesc.DepthOrArraySize = 1;
	uploadResDesc.MipLevels = 1;
	uploadResDesc.SampleDesc.Count = 1;
	uploadResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_HEAP_PROPERTIES uploadHeap{};
	uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;

	DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&uploadResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,		//	CPU���珑�����݉\�AGPU�͓ǂݎ���p
		nullptr,
		IID_PPV_ARGS(DirectXWrapper::GetInstance().GetTexUploadBuffPP()));

#pragma endregion


	//	�]��
	uint8_t* mapforImg = nullptr;
	result = DirectXWrapper::GetInstance().GetTexUploadBuffP()->Map(0, nullptr, (void**)&mapforImg);//�}�b�s���O

	const Image* img = scratchImg.GetImage(0, 0, 0);
	uint8_t* uploadStart = mapforImg + footprint.Offset;
	uint8_t* sourceStart = img->pixels;
	uint32_t sourcePitch = ((uint32_t)img->width * sizeof(uint32_t));

	//	�摜�̍���(�s�N�Z��)���R�s�[����
	for (uint32_t i = 0; i < footprint.Footprint.Height; i++)
	{
		memcpy(
			uploadStart + i * footprint.Footprint.RowPitch,
			sourceStart + i * sourcePitch,
			sourcePitch
		);
	}
	DirectXWrapper::GetInstance().GetTexUploadBuffP()->Unmap(0, nullptr);	//	unmap

#pragma region CopyCommand
	//	�O���t�B�b�N�{�[�h��̃R�s�[��A�h���X
	D3D12_TEXTURE_COPY_LOCATION texCopyDest{};
	texCopyDest.pResource = *texBuffL;
	texCopyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	texCopyDest.SubresourceIndex = 0;
	//	�O���t�B�b�N�{�[�h��̃R�s�[���A�h���X
	D3D12_TEXTURE_COPY_LOCATION src{};
	src.pResource = *DirectXWrapper::GetInstance().GetTexUploadBuffPP();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = footprint;

	//	�쐬
	DirectXWrapper::GetInstance().GetTexCommandList()->CopyTextureRegion(&texCopyDest, 0, 0, 0, &src, nullptr);

	//	resourceBarrier�}��
	D3D12_RESOURCE_BARRIER copyBarrier{};
	copyBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	copyBarrier.Transition.pResource = *texBuffL;
	copyBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	copyBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	copyBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	DirectXWrapper::GetInstance().GetTexCommandList()->ResourceBarrier(1, &copyBarrier);

	uint64_t textureHandle;

	//SRV
	{
		//04_03
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDescL{};//�ݒ�\����
		//srv�n���h��
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleL = {};

		//srv�������ɐݒ肳��Ă�����
		if (texBuff && srvDesc && srvHandle)
		{
			srvDescL = *srvDesc;
			srvHandleL = *srvHandle;
			textureHandle = -114514;
		}
		else
		{
			//SRV�q�[�v�̐擪�n���h�����擾
			sSrvHandle_ = sSrvHeap_->GetCPUDescriptorHandleForHeapStart();
			sSrvHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(sSrvHeapDesc_.Type) * sCount_;

			srvDescL.Format = uploadResDesc.Format;
			srvDescL.Shader4ComponentMapping =
				D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDescL.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
			srvDescL.Texture2D.MipLevels = 1;

			srvHandleL = sSrvHandle_;

			//04_02(�摜�\��p�̃A�h���X��������)
			//SRV�q�[�v�̃n���h�����擾
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = sSrvHeap_->GetGPUDescriptorHandleForHeapStart();
			textureHandle = srvGpuHandle.ptr +
				(DirectXWrapper::GetInstance().GetDevice()->
					GetDescriptorHandleIncrementSize(TextureManager::GetInstance().sSrvHeapDesc_.Type) * sCount_);
		}

		//�n���h���̂����ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(*texBuffL, &srvDescL, srvHandleL);

		if (texBuff == nullptr)
		{
			//�o�b�t�@�p�̃J�E���g
			sCount_++;
		}

		{
			//���O�ƃf�[�^��R�Â��ĕۑ�
			//�e�N�X�`����A�z�z��Ɋi�[(�Z�b�g�Ń}�b�v�Ɋi�[)
			sTextureDatas_.insert(std::make_pair(fileName, textureHandle));
		}

		//���f�[�^���
		scratchImg.Release();
	}

	return textureHandle;
}

void TextureManager::CheckTexHandle(uint64_t& texHandle)
{
	if (texHandle == NULL)
	{
		texHandle = sWhiteTexHandle_;
	}
}
