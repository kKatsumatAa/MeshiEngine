#include "TextureManager.h"

int TextureManager::count = 0;
//���\�[�X�ݒ�
D3D12_RESOURCE_DESC TextureManager::resDesc;
//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
ComPtr < ID3D12DescriptorHeap> TextureManager::srvHeap;
D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::srvHandle;

ComPtr<ID3D12Resource> TextureManager::texBuff[srvCount];

//SRV�̍ő��
const size_t TextureManager::kMaxSRVCount = 2056;
//�f�X�N���v�^�q�[�v�̐ݒ�
D3D12_DESCRIPTOR_HEAP_DESC TextureManager::srvHeapDesc;

D3D12_DESCRIPTOR_RANGE TextureManager::descriptorRange;

UINT64 TextureManager::whiteTexHandle = NULL;

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
}

TextureManager& TextureManager::GetInstance()
{
	static TextureManager instance;

	return instance;
}

void TextureManager::InitializeDescriptorHeap()
{
	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.NumDescriptors = kMaxSRVCount + 2;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_�[���猩����悤��
	//desc�͐ݒ�
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(srvHeap.GetAddressOf()));
	assert(SUCCEEDED(Directx::GetInstance().result));


	//�f�X�N���v�^�����W�̐ݒ�
	descriptorRange.NumDescriptors = 100;   //��x�̕`��Ɏg���e�N�X�`���̖���
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;  //�e�N�X�`�����W�X�^0��(t0)
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//�����摜
	if (whiteTexHandle == NULL)
	{
		LoadGraph(L"Resources/image/white.png", whiteTexHandle);
	}
}


void TextureManager::LoadGraph(const wchar_t* name, UINT64& textureHandle)
{
	assert(count <= srvCount - 1);

	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WIC�̃e�N�X�`���̃��[�h
	Directx::GetInstance().result = LoadFromWICFile(
		name,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(Directx::GetInstance().result));

	ScratchImage mipChain{};
	//mipmap����
	Directx::GetInstance().result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(Directx::GetInstance().result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	// 04_03

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;						//���_�f�[�^�S�̂̃T�C�Y
	textureResourceDesc.Height = (UINT)metadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//�e�N�X�`���o�b�t�@�̐���
	//ID3D12Resource* texBuff = nullptr;
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&textureHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&TextureManager::GetInstance().texBuff[count]));
	assert(SUCCEEDED(Directx::GetInstance().result));

	{
		HRESULT result;

		//Footprint �擾�B

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT  footprint;
		UINT64  total_bytes = 0;
		Directx::GetInstance().GetDevice()->GetCopyableFootprints
		(&textureResourceDesc, 0, 1, 0, &footprint, nullptr, nullptr, &total_bytes);

		//Upload �p Buffer �쐬
		D3D12_RESOURCE_DESC  desc;
		memset(&desc, 0, sizeof(desc));
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = total_bytes;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		D3D12_HEAP_PROPERTIES  heap;
		memset(&heap, 0, sizeof(heap));
		heap.Type = D3D12_HEAP_TYPE_UPLOAD;

		ID3D12Resource* iUploadBuffer = nullptr;
		result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
			&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&iUploadBuffer));
		assert(SUCCEEDED(result));

		//Upload Buffer �ւ̏������݁B
		void* ptr = nullptr;
		result = iUploadBuffer->Map(0, nullptr, &ptr);
		//memcpy(reinterpret_cast<unsigned char*>(ptr) + footprint.Offset, 
		//	&(scratchImg.GetPixels()[y * bitmapWidth]), sizeof(DWORD) * bitmapWidth);
		//assert(SUCCEEDED(result));

		for (UINT y = 0; y < metadata.height; y++)
		{
			memcpy(reinterpret_cast<unsigned char*>(ptr) + footprint.Offset,
				&(scratchImg.GetPixels()[y * metadata.width]), sizeof(DWORD) * metadata.width);
		}

		//Copy �R�}���h�쐬
		D3D12_TEXTURE_COPY_LOCATION  dest;
		memset(&dest, 0, sizeof(dest));
		dest.pResource = texBuff[count].Get();
		dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dest.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION  src;
		memset(&src, 0, sizeof(src));
		src.pResource = iUploadBuffer;
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint = footprint;

		ID3D12GraphicsCommandList* iCommandList = Directx::GetInstance().GetCommandList();
		iCommandList->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);

		//ResourceBarrier �}��
		D3D12_RESOURCE_BARRIER  barrier;
		memset(&barrier, 0, sizeof(barrier));
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = texBuff[count].Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

		iCommandList->ResourceBarrier(1, &barrier);

		//Command ���s
		iCommandList->Close();
		ID3D12CommandList* list[] = { iCommandList };
		Directx::GetInstance().GetCommandQueue()->ExecuteCommandLists(1, list);

		Directx::GetInstance().GetFenceVal()++;
		Directx::GetInstance().GetCommandQueue()->Signal(Directx::GetInstance().GetFence(), Directx::GetInstance().GetFenceVal());

		//�R�}���h���Z�b�g
		Directx::GetInstance().CommandReset();
		//
		iUploadBuffer->Release();
	}


	////04_03
	//// �S�~�b�v�}�b�v�ɂ���
	//for (size_t i = 0; i < metadata.mipLevels; i++)
	//{
	//	//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
	//	const Image* img = scratchImg.GetImage(i, 0, 0);
	//	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	//	Directx::GetInstance().result = texBuff[count]->WriteToSubresource(
	//		(UINT)i,
	//		nullptr,//�S�̈�փR�s�[
	//		img->pixels,//���f�[�^�A�h���X
	//		(UINT)img->rowPitch,//1���C���T�C�Y
	//		(UINT)img->slicePitch//�S�T�C�Y
	//	);
	//	assert(SUCCEEDED(Directx::GetInstance().result));
	//}
	////���f�[�^���
	//delete[] imageData;


	//SRV�q�[�v�̐擪�n���h�����擾
	if (count == 0) { srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart(); }
	else { srvHandle.ptr += Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type); }

	//04_03
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;
	//�n���h���̂����ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	Directx::GetInstance().GetDevice()->CreateShaderResourceView(TextureManager::GetInstance().texBuff[count].Get(), &srvDesc, srvHandle);

	//04_02(�摜�\��p�̃A�h���X��������)
	//SRV�q�[�v�̃n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	textureHandle = srvGpuHandle.ptr + (Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(TextureManager::GetInstance().srvHeapDesc.Type) * count);


	count++;
}