#include "Light.h"

ID3D12Device* Light::device_ = nullptr;


void Light::StaticInitialize()
{
	//�ď��������`�F�b�N
	assert(!Light::device_);
	//nullptr�`�F�b�N
	assert(Directx::GetInstance().GetDevice());
	//�Z�b�g
	Light::device_ = Directx::GetInstance().GetDevice();
}

void Light::Initialize()
{
	//�q�[�v�ݒ�
	UINT sizeVB;
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
	sizeVB = static_cast<UINT>(sizeof(ConstBufferData));
	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//���_�o�b�t�@�̐���
	BuffProperties(heapProp, resDesc, constBuff.GetAddressOf());

	//�萔�o�b�t�@�փf�[�^�]��
	TransferConstBuffer();

	//�_����
	PointLight::ConstBufferData pointLights[PointLightNum];
}

void Light::TransferConstBuffer()
{
	HRESULT result;
	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->lightv = -lightdir;
		constMap->lightColor = lightColor;

		//�_����
		for (int i = 0; i < PointLightNum; i++)
		{
			//���C�g���L���Ȃ�ݒ��]��
			if (pointLights[i].GetActive())
			{
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightpos = pointLights[i].GetLightPos();
				constMap->pointLights[i].lightcolor = 
					pointLights[i].GetLightColor();
				constMap->pointLights[i].lightatten =
					pointLights[i].GetLightAtten();
			}

			//���C�g�������Ȃ烉�C�g�F��0��
			else
			{
				constMap->pointLights[i].active = 0;
			}
		}

		constBuff->Unmap(0, nullptr);
	}

	
}

void Light::SetLightDir(const XMVECTOR& lightdir)
{
	//���K�����ăZ�b�g
	this->lightdir = XMVector3Normalize(lightdir);
	dirty = true;
}

void Light::SetLightColor(const XMFLOAT3& lightcolor)
{
	this->lightColor = lightColor;
	dirty = true;
}

//-------------------------------------------------------------
void Light::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetActive(active);
}

void Light::SetPointLightPos(int index, const XMFLOAT3& pos)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightPos(pos);

	dirty = true;
}

void Light::SetPointLightColor(int index, const XMFLOAT3& color)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightColor(color);

	dirty = true;
}

void Light::SetPointLightAtten(int index, const XMFLOAT3& atten)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightAtten(atten);

	dirty = true;
}


//-----------------------------------------------------------------------
void Light::Update()
{
	//�l�̍X�V���������������萔�o�b�t�@�ɓ]��
	if (dirty)
	{
		TransferConstBuffer();
		dirty = false;
	}
}

void Light::Draw(UINT rootParamaterIndex)
{
	//�萔�o�b�t�@�r���[���Z�b�g
	Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(
		rootParamaterIndex, constBuff->GetGPUVirtualAddress()
	);
}

Light* Light::Create()
{
	//3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Light* instance = new Light();
	//������
	instance->Initialize();
	//���������C���X�^���X��Ԃ�
	return instance;
}
