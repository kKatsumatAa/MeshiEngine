#include "LightManager.h"
#include "ImGuiManager.h"

ID3D12Device* LightManager::sDevice_ = nullptr;

using namespace DirectX;


void LightManager::StaticInitialize()
{
	//�ď��������`�F�b�N
	assert(!LightManager::sDevice_);
	//nullptr�`�F�b�N
	assert(DirectXWrapper::GetInstance().GetDevice());
	//�Z�b�g
	LightManager::sDevice_ = DirectXWrapper::GetInstance().GetDevice();
}

void LightManager::Initialize()
{
	//�q�[�v�ݒ�
	uint32_t sizeVB;
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
	sizeVB = static_cast<uint32_t>(sizeof(ConstBufferData));
	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//�萔�o�b�t�@�̐���
	BuffProperties(heapProp, resDesc, constBuff_.GetAddressOf());

	//�萔�o�b�t�@�փf�[�^�]��
	TransferConstBuffer();
}

void LightManager::InitializeActive()
{
	for (int i = 0; i < S_DIR_LIGHT_NUM_; i++)
	{
		dirLights_[i].SetActive(false);
	}

	for (int i = 0; i < S_POINT_LIGHT_NUM_; i++)
	{
		pointLights_[i].SetActive(false);
	}

	for (int i = 0; i < S_SPOT_LIGHT_NUM_; i++)
	{
		spotLights_[i].SetActive(false);
	}
}

void LightManager::TransferConstBuffer()
{
	HRESULT result;
	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		//����
		constMap->ambientColor = ambientColor_;
		constMap->diffuseColor = diffuseColor_;
		constMap->specularColor = specularColor_;

		for (int32_t i = 0; i < S_DIR_LIGHT_NUM_; i++) {

			//���s����
			// ���C�g���L���Ȃ�ݒ��]��
			if (dirLights_[i].GetActive()) {
				constMap->dirLights[i].active = 1;
				constMap->dirLights[i].lightv = -dirLights_[i].GetLightDir();
				constMap->dirLights[i].lightColor = dirLights_[i].GetLightColor();
			}
			// ���C�g�������Ȃ烉�C�g�F��0��
			else {
				constMap->dirLights[i].active = 0;
			}
		}
		for (int32_t i = 0; i < S_POINT_LIGHT_NUM_; i++) {
			//�_����
			//���C�g���L���Ȃ�ݒ��]��
			if (pointLights_[i].GetActive())
			{
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightpos = pointLights_[i].GetLightPos();
				constMap->pointLights[i].lightcolor =
					pointLights_[i].GetLightColor();
				constMap->pointLights[i].lightatten =
					pointLights_[i].GetLightAtten();
			}
			//���C�g�������Ȃ烉�C�g�F��0��
			else
			{
				constMap->pointLights[i].active = 0;
			}
		}
		//�X�|�b�g���C�g
		for (int32_t i = 0; i < S_SPOT_LIGHT_NUM_; i++)
		{
			//���C�g���L���Ȃ�ݒ��]��
			if (spotLights_[i].GetActive())
			{
				constMap->spotLights[i].active = 1;
				constMap->spotLights[i].lightv = -spotLights_[i].GetLightDir();
				constMap->spotLights[i].lightpos = spotLights_[i].GetLightPos();
				constMap->spotLights[i].lightcolor =
					spotLights_[i].GetLightColor();
				constMap->spotLights[i].lightatten =
					spotLights_[i].GetLightAtten();
				constMap->spotLights[i].lightfactoranglecos =
					spotLights_[i].GetLightFactorAngleCos();
			}
			//���C�g�������Ȃ烉�C�g�F��0��
			else
			{
				constMap->spotLights[i].active = 0;
			}
		}
		//�ۉe
		for (int32_t i = 0; i < S_CIRCLE_SHADOW_NUM_; i++)
		{
			//���C�g���L���Ȃ�ݒ��]��
			if (circleShadows_[i].GetLightActive())
			{
				constMap->circleShadows[i].active = 1;
				constMap->circleShadows[i].dir = -circleShadows_[i].GetDir();
				constMap->circleShadows[i].casterPos = circleShadows_[i].GetCasterPos();
				constMap->circleShadows[i].distanceCasterLight =
					circleShadows_[i].GetDistanceCasterLight();
				constMap->circleShadows[i].atten =
					circleShadows_[i].GetAtten();
				constMap->circleShadows[i].factorAngleCos =
					circleShadows_[i].GetFactorAngleCos();
			}
			//���C�g�������Ȃ烉�C�g�F��0��
			else
			{
				constMap->circleShadows[i].active = 0;
			}
		}

		constBuff_->Unmap(0, nullptr);
	}
}

void LightManager::DefaultLightSetting()
{
	dirLights_[0].SetActive(true);
	dirLights_[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[0].SetLightDir({ 0.0f, -1.0f, 0.0f, 0 });

	dirLights_[1].SetActive(true);
	dirLights_[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[1].SetLightDir({ +0.5f, +0.1f, +0.2f, 0 });

	dirLights_[2].SetActive(true);
	dirLights_[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[2].SetLightDir({ -0.5f, +0.1f, -0.2f, 0 });
}

void LightManager::SetAmbientColor(const XMFLOAT3& color)
{
	ambientColor_ = color;
	dirty_ = true;
}

void LightManager::SetDiffuseColor(const XMFLOAT3& color)
{
	diffuseColor_ = color;
	dirty_ = true;
}

void LightManager::SetSpecularColor(const XMFLOAT3& color)
{
	specularColor_ = color;
	dirty_ = true;
}

void LightManager::Update()
{
	//�l�̍X�V���������������萔�o�b�t�@�ɓ]��
	if (dirty_)
	{
		TransferConstBuffer();
		dirty_ = false;
	}
}

void LightManager::Draw(uint32_t rootParamaterIndex)
{
	//�萔�o�b�t�@�r���[���Z�b�g
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(
		rootParamaterIndex, constBuff_->GetGPUVirtualAddress()
	);
}

void LightManager::DrawImGui()
{
	ImGui::Begin("Light");

	for (int i = 0; i < S_DIR_LIGHT_NUM_; i++)
	{
		if (dirLights_->GetActive())
		{
			dirLights_[i].DrawImGui(i);
		}
	}

	for (int i = 0; i < S_POINT_LIGHT_NUM_; i++)
	{
		if (pointLights_->GetActive())
		{
			pointLights_[i].DrawImGui(i);
		}
	}

	for (int i = 0; i < S_SPOT_LIGHT_NUM_; i++)
	{
		if (spotLights_->GetActive())
		{
			spotLights_[i].DrawImGui(i);
		}
	}

	ImGui::End();
}

std::unique_ptr<LightManager> LightManager::Create()
{
	//3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	std::unique_ptr<LightManager> instance = std::make_unique< LightManager>();
	//������
	instance->Initialize();
	//���������C���X�^���X��Ԃ�
	return std::move(instance);
}

//-----------------------------------------------------------
void LightManager::SetDirLightActive(int32_t index, bool active)
{
	assert(0 <= index && index < S_DIR_LIGHT_NUM_);

	dirLights_[index].SetActive(active);
}

void LightManager::SetDirLightDir(int32_t index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < S_DIR_LIGHT_NUM_);

	dirLights_[index].SetLightDir(lightdir);
	dirty_ = true;
}

void LightManager::SetDirLightColor(int32_t index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < S_DIR_LIGHT_NUM_);

	dirLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}


//-------------------------------------------------------------
void LightManager::SetPointLightActive(int32_t index, bool active)
{
	assert(0 <= index && index < S_POINT_LIGHT_NUM_);
	pointLights_[index].SetActive(active);
}

void LightManager::SetPointLightPos(int32_t index, const XMFLOAT3& pos)
{
	assert(0 <= index && index < S_POINT_LIGHT_NUM_);
	pointLights_[index].SetLightPos(pos);

	dirty_ = true;
}

void LightManager::SetPointLightColor(int32_t index, const XMFLOAT3& color)
{
	assert(0 <= index && index < S_POINT_LIGHT_NUM_);
	pointLights_[index].SetLightColor(color);

	dirty_ = true;
}

void LightManager::SetPointLightAtten(int32_t index, const XMFLOAT3& atten)
{
	assert(0 <= index && index < S_POINT_LIGHT_NUM_);
	pointLights_[index].SetLightAtten(atten);

	dirty_ = true;
}

//------------------------------------------------------------------------
void LightManager::SetSpotLightActive(int32_t index, bool active)
{
	assert(0 <= index && index < S_SPOT_LIGHT_NUM_);

	spotLights_[index].SetActive(active);
}

void LightManager::SetSpotLightDir(int32_t index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < S_SPOT_LIGHT_NUM_);

	spotLights_[index].SetLightDir(lightdir);
	dirty_ = true;
}

void LightManager::SetSpotLightPos(int32_t index, const XMFLOAT3& lightpos)
{
	assert(0 <= index && index < S_SPOT_LIGHT_NUM_);

	spotLights_[index].SetLightPos(lightpos);
	dirty_ = true;
}

void LightManager::SetSpotLightColor(int32_t index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < S_SPOT_LIGHT_NUM_);

	spotLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}

void LightManager::SetSpotLightAtten(int32_t index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < S_SPOT_LIGHT_NUM_);

	spotLights_[index].SetLightAtten(lightAtten);
	dirty_ = true;
}

void LightManager::SetSpotLightFactorAngle(int32_t index, const XMFLOAT2& lightFactorAngle)
{
	assert(0 <= index && index < S_SPOT_LIGHT_NUM_);

	spotLights_[index].SetLightFactorAngleCos(lightFactorAngle);
	dirty_ = true;
}


//--------------------------------------------------------------------------------------
void LightManager::SetCircleShadowActive(int32_t index, bool active)
{
	assert(0 <= index && index < S_CIRCLE_SHADOW_NUM_);

	circleShadows_[index].SetLightActive(active);
}

void LightManager::SetCircleShadowCasterPos(int32_t index, const XMFLOAT3& casterPos)
{
	assert(0 <= index && index < S_CIRCLE_SHADOW_NUM_);

	circleShadows_[index].SetCasterPos(casterPos);
	dirty_ = true;
}

void LightManager::SetCircleShadowDir(int32_t index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < S_CIRCLE_SHADOW_NUM_);

	circleShadows_[index].SetDir(lightdir);
	dirty_ = true;
}

void LightManager::SetCircleShadowDistanceCasterLight(int32_t index, float distanceCasterLight)
{
	assert(0 <= index && index < S_CIRCLE_SHADOW_NUM_);

	circleShadows_[index].SetDistanceCasterLight(distanceCasterLight);
	dirty_ = true;
}

void LightManager::SetCircleShadowAtten(int32_t index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < S_CIRCLE_SHADOW_NUM_);

	circleShadows_[index].SetAtten(lightAtten);
	dirty_ = true;
}

void LightManager::SetCircleShadowFactorAngle(int32_t index, const XMFLOAT2& lightFactorAngle)
{
	assert(0 <= index && index < S_CIRCLE_SHADOW_NUM_);

	circleShadows_[index].SetFactorAngleCos(lightFactorAngle);
	dirty_ = true;
}


//-------------------------------------------------------------
bool LightManager::GetDoNotUseDirLightIndex(int32_t& index)
{
	for (int i = 0; i < S_DIR_LIGHT_NUM_; i++)
	{
		//�������
		if (!dirLights_[i].GetActive())
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool LightManager::GetDoNotUsePointLightIndex(int32_t& index)
{
	for (int i = 0; i < S_POINT_LIGHT_NUM_; i++)
	{
		//�������
		if (!pointLights_[i].GetActive())
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool LightManager::GetDoNotUseSpotLightIndex(int32_t& index)
{
	for (int i = 0; i < S_SPOT_LIGHT_NUM_; i++)
	{
		//�������
		if (!spotLights_[i].GetActive())
		{
			index = i;
			return true;
		}
	}

	return false;
}
