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


	HRESULT result;

	//�A�b�v���[�h�p

	const Image* img = scratchImg.GetImage(0, 0, 0); // �� �f�[�^ ���o

	//���ԃo�b�t�@�[�Ƃ��ẴA�b�v���[�h�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES uploadHeapProp = {};
	//�}�b�v�\�ɂ��邽�߁AUPLOAD�ɂ���
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//�A�b�v���[�h�p�Ɏg�p���邱�ƑO��Ȃ̂�UNKNOWN�ł悢
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	uploadHeapProp.CreationNodeMask = 0; // �P�� �A�_�v�^�[ �� ���� 0
	uploadHeapProp.VisibleNodeMask = 0; // �P�� �A�_�v�^�[ �� ���� 0

	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Format = DXGI_FORMAT_UNKNOWN;//�P�Ȃ�f�[�^�̉�Ȃ̂�UNKNOWN
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;//�P�Ȃ�o�b�t�@�[�Ƃ��Ďw��
	resDesc.Width = AligmentSize(img->slicePitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT)
		* img->height;//�f�[�^�T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;//�A�������f�[�^
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;//���Ƀt���O�Ȃ�
	resDesc.SampleDesc.Count = 1;//�ʏ�e�N�X�`���Ȃ̂ŃA���`�G�C���A�V���O���Ȃ�
	resDesc.SampleDesc.Quality = 0;

	// ���� �o�b�t�@�[ �쐬 
	ComPtr <ID3D12Resource> uploadbuff = nullptr;
	result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,//���Ɏw��Ȃ�
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadbuff)
	);


	//�R�s�[��p
		// �e�N�X�`�� �� ���� �� �q�[�v �ݒ� 
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;//�e�N�X�`���p
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	texHeapProp.CreationNodeMask = 0;//�P��A�_�v�^�[�̂���0
	texHeapProp.VisibleNodeMask = 0;//�P��A�_�v�^�[�̂���0

	//���\�[�X�ݒ�i�ϐ��͎g���܂킵�j
	resDesc.Format = metadata.format;
	resDesc.Width = metadata.width;//��
	resDesc.Height = metadata.height;//����
	resDesc.DepthOrArraySize = metadata.arraySize;//2D�Ŕz��ł��Ȃ��̂�1
	resDesc.MipLevels = metadata.mipLevels;//�~�b�v�}�b�v���Ȃ��̂Ń~�b�v����1��
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	// �e�N�X�`���o�b�t�@�[ �� �쐬 
	//ID3D12Resource* texbuff = nullptr;
	result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&texHeapProp, D3D12_HEAP_FLAG_NONE,//���Ɏw��Ȃ�
		&resDesc, D3D12_RESOURCE_STATE_COPY_DEST,//�R�s�[��
		nullptr, IID_PPV_ARGS(&texBuff[count]));

	//�}�b�v
	uint8_t* mapforImg = nullptr;//image->pixels�Ɠ����^�ɂ���
	result = uploadbuff->Map(0, nullptr, (void**)&mapforImg);//�}�b�v

	uint8_t* srcAddress = img->pixels;
	auto rowPitch = AligmentSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	for (int y = 0; y < img->height; ++y) {
		std::copy_n(srcAddress, rowPitch, mapforImg);//�R�s�[
		//1�s���Ƃ̂��܂����킹��
		srcAddress += img->rowPitch;
		mapforImg += rowPitch;
	}
	uploadbuff->Unmap(0, nullptr);//�A���}�b�v

	//
	D3D12_TEXTURE_COPY_LOCATION src = {};
	//�R�s�[���i�A�b�v���[�h���j�ݒ�
	src.pResource = uploadbuff.Get();//���ԃo�b�t�@�[
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;//�t�b�g�v�����g�w��
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Width = metadata.width;
	src.PlacedFootprint.Footprint.Height = metadata.height;
	src.PlacedFootprint.Footprint.Depth = metadata.depth;
	//src.PlacedFootprint.Footprint.RowPitch = img->rowPitch;
	src.PlacedFootprint.Footprint.Format = img->format;
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	//�R�s�[��ݒ�
	dst.pResource = texBuff[count].Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	//256�o�C�g�A���C�����g
	src.PlacedFootprint.Footprint.RowPitch =
		AligmentSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	//
	Directx::GetInstance().GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	//�o���A�ƃt�F���X
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = texBuff[count].Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;//�������d�v
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;//�������d�v

	{
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

		////���f�[�^���
		scratchImg.Release();
	}

	Directx::GetInstance().GetCommandList()->ResourceBarrier(1, &BarrierDesc);
	Directx::GetInstance().GetCommandList()->Close();

	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdlists[] = { Directx::GetInstance().GetCommandList() };
	Directx::GetInstance().GetCommandQueue()->ExecuteCommandLists(1, cmdlists);

	//�R�}���h����
	Directx::GetInstance().GetCommandQueue()->Signal(Directx::GetInstance().GetFence(),
		++Directx::GetInstance().GetFenceVal());

	if (Directx::GetInstance().GetFence()->GetCompletedValue() != Directx::GetInstance().GetFenceVal())
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		Directx::GetInstance().GetFence()->SetEventOnCompletion(Directx::GetInstance().GetFenceVal(), event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//�o�b�t�@���
	uploadbuff->Release();

	//�R�}���h���Z�b�g
	Directx::GetInstance().CommandReset();
}