#include "ConstBuffTransform.h"
#include "TextureManager.h"

std::map<uint64_t, ConstBuffTransform::ConstBuffTransformSaveData> ConstBuffTransform::sCbtDatas_;
D3D12_DESCRIPTOR_RANGE ConstBuffTransform::sDescRange_;


void ConstBuffTransform::StaticInitialize()
{
	//�f�X�N���v�^�����W�̐ݒ�
	sDescRange_.NumDescriptors = 1;   //��x�̕`��Ɏg����
	sDescRange_.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	sDescRange_.BaseShaderRegister = 1;  //b1
}


ConstBuffTransform::~ConstBuffTransform()
{
	auto itr = sCbtDatas_.find(cbvGPUHandle_.ptr);

	//�g�p���Ă�t���O���I�t
	if (itr != sCbtDatas_.end())
	{
		itr->second.isUsing = false;
	}
}

void ConstBuffTransform::Initialize()
{
	//�o�b�t�@�̃f�[�^�����炤
	ConstBuffTransformDrawData data = CreateBuffAndAdd();

	//�����o�ϐ��Ɋi�[
	cbvGPUHandle_ = data.cbvGPUHandle;
	constMapTransform_ = data.constMapTransform;
}

void ConstBuffTransform::DrawCommand(int32_t index)
{
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(index, cbvGPUHandle_);
}

//----------------------------------------------------------------------------------------
ConstBuffTransform::ConstBuffTransformDrawData ConstBuffTransform::CreateBuffAndAdd()
{
	ConstBuffTransformDrawData data;
	//�g�p����ĂȂ��쐬�ς݃o�b�t�@������Ύg�킹��
	if (GetSaveData(data))
	{
		return data;
	}
	//�Ȃ���ΐV���ɍ���ĕۑ��A�������炤
	else
	{
		return CreateBuff();
	}
}

bool ConstBuffTransform::GetSaveData(ConstBuffTransformDrawData& data)
{
	for (const auto& itr : sCbtDatas_)
	{
		//���܂͂��̃o�b�t�@���g�p����ĂȂ����
		if (itr.second.isUsing == false)
		{
			//�n���h���ƃ}�b�s���O�p�|�C���^
			data.cbvGPUHandle.ptr = itr.first;
			data.constMapTransform = itr.second.constMapTransform;

			return true;
		}
	}

	return false;
}

ConstBuffTransform::ConstBuffTransformAllData ConstBuffTransform::CreateBuffInternal()
{
	ConstBuffTransformAllData alldata;

	HRESULT result;

	//�o�b�t�@�쐬
	{
		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};

		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/;						//���_�f�[�^�S�̂̃T�C�Y
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
			&cbHeapProp,//�q�[�v�ݒ�
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc,//���\�[�X�ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&alldata.saveData.constBuffTransform));
		assert(SUCCEEDED(result));

	}
	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataTransform* map;
	result = alldata.saveData.constBuffTransform->Map(0, nullptr, (void**)&map);//�}�b�s���O
	assert(SUCCEEDED(result));
	alldata.drawData.constMapTransform = map;
	alldata.saveData.constMapTransform = map;


	{
		//�r���[�쐬
		D3D12_CPU_DESCRIPTOR_HANDLE cbvCPUHandle;
		cbvCPUHandle = TextureManager::GetDescHeapP()->GetCPUDescriptorHandleForHeapStart();
		cbvCPUHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * TextureManager::GetSRVCount();

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = alldata.saveData.constBuffTransform->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (uint32_t)alldata.saveData.constBuffTransform->GetDesc().Width;

		DirectXWrapper::GetInstance().GetDevice()->CreateConstantBufferView(&cbvDesc, cbvCPUHandle);
	}

	//Gpu�̃n���h�����Q�b�g
	{
		//gpu�ł̃X�^�[�g�ʒu+�C���N�������g�T�C�Y*�J�E���g
		alldata.drawData.cbvGPUHandle.ptr = TextureManager::GetDescHeapP()->GetGPUDescriptorHandleForHeapStart().ptr
			+ DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * TextureManager::GetSRVCount();
	}

	alldata.saveData.isUsing = true;

	//�J�E���g�i�߂�
	TextureManager::AddSRVHandleCount();

	return alldata;
}

ConstBuffTransform::ConstBuffTransformDrawData ConstBuffTransform::CreateBuff()
{
	//�o�b�t�@���쐬���������炤
	ConstBuffTransformAllData alldata = CreateBuffInternal();

	//����ۑ�
	sCbtDatas_.insert(std::make_pair(alldata.drawData.cbvGPUHandle.ptr, alldata.saveData));

	//�`��ɕK�v�Ȃ��̂����Ԃ�
	return alldata.drawData;
}
