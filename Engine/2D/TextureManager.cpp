#include "TextureManager.h"
#include <d3dx12.h>
#include "Util.h"
using namespace DirectX;

int32_t TextureManager::count = 0;
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

uint64_t TextureManager::whiteTexHandle = 114514;

std::map < std::string, uint64_t> TextureManager::textureDatas_;

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
	srvHeapDesc.NumDescriptors = kMaxSRVCount;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_�[���猩����悤��
	//desc�͐ݒ�
	DirectXWrapper::GetInstance().result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(srvHeap.GetAddressOf()));
	assert(SUCCEEDED(DirectXWrapper::GetInstance().result));


	//�f�X�N���v�^�����W�̐ݒ�
	descriptorRange.NumDescriptors = 100;   //��x�̕`��Ɏg���e�N�X�`���̖���
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;  //�e�N�X�`�����W�X�^0��(t0)
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
}


void TextureManager::LoadGraph(const wchar_t* name, uint64_t& textureHandle)
{
	assert(count <= srvCount - 1);

	//�ǂݍ��܂�Ă��邩�ǂ���
	char namec[128] = {};
	ConstWCharTToChar(name, namec);

	std::string fileName = namec;
	{
		//�t�@�C��������T��
		std::map<std::string, uint64_t>::iterator it = textureDatas_.find(fileName);
		//���łɓǂݍ��܂�Ă����炻�̃n���h����Ԃ�
		if (it != textureDatas_.end())
		{
			textureHandle = it->second;
			return;
		}
	}

	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WIC�̃e�N�X�`���̃��[�h
	DirectXWrapper::GetInstance().result = LoadFromWICFile(
		name,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(DirectXWrapper::GetInstance().result));

	ScratchImage mipChain{};
	//mipmap����
	DirectXWrapper::GetInstance().result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(DirectXWrapper::GetInstance().result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);


	HRESULT result;

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
			IID_PPV_ARGS(&texBuff[count]));
	assert(SUCCEEDED(result));


	//�A�b�v���[�h�o�b�t�@
	// �q�[�v�̐ݒ�
	uint64_t uploadSize = GetRequiredIntermediateSize(texBuff[count].Get(), 0, (uint32_t)metadata.mipLevels);

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

	for (size_t i = 0; i < subResourcesDatas.size(); i++)
	{
		// �S�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);

		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = img->rowPitch;
		subResourcesDatas[i].SlicePitch = img->slicePitch;
	}

	//�T�u���\�[�X��]��
	UpdateSubresources(
		DirectXWrapper::GetInstance().GetCommandList(),
		texBuff[count].Get(),
		uploadBuff.Get(),
		0,
		0,
		(uint32_t)metadata.mipLevels,
		subResourcesDatas.data()
	);

	//SRV
	{
		//SRV�q�[�v�̐擪�n���h�����擾
		if (count == 0) { srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart(); }
		else { srvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type); }

		//04_03
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping =
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
		srvDesc.Texture2D.MipLevels = 1;
		//�n���h���̂����ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(TextureManager::GetInstance().texBuff[count].Get(), &srvDesc, srvHandle);

		//04_02(�摜�\��p�̃A�h���X��������)
		//SRV�q�[�v�̃n���h�����擾
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		textureHandle = srvGpuHandle.ptr + (DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(TextureManager::GetInstance().srvHeapDesc.Type) * count);

		{
			//���O�ƃf�[�^��R�Â��ĕۑ�
			//�T�E���h�f�[�^��A�z�z��Ɋi�[(�������ăZ�b�g�Ń}�b�v�Ɋi�[)
			textureDatas_.insert(std::make_pair(fileName, textureHandle));
		}

		//���f�[�^���
		scratchImg.Release();
	}

	//�o���A�ƃt�F���X
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = texBuff[count].Get();
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
	count++;
}