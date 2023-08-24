#include "LightManager.h"
#include "ImGuiManager.h"

ID3D12Device* LightManager::sDevice_ = nullptr;

using namespace DirectX;


void LightManager::StaticInitialize()
{
	//再初期化をチェック
	assert(!LightManager::sDevice_);
	//nullptrチェック
	assert(DirectXWrapper::GetInstance().GetDevice());
	//セット
	LightManager::sDevice_ = DirectXWrapper::GetInstance().GetDevice();
}

void LightManager::Initialize()
{
	//ヒープ設定
	uint32_t sizeVB;
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<uint32_t>(sizeof(ConstBufferData));
	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//定数バッファの生成
	BuffProperties(heapProp, resDesc, constBuff_.GetAddressOf());

	//定数バッファへデータ転送
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
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		//環境光
		constMap->ambientColor = ambientColor_;
		constMap->diffuseColor = diffuseColor_;
		constMap->specularColor = specularColor_;

		for (int32_t i = 0; i < S_DIR_LIGHT_NUM_; i++) {

			//平行光源
			// ライトが有効なら設定を転送
			if (dirLights_[i].GetActive()) {
				constMap->dirLights[i].active = 1;
				constMap->dirLights[i].lightv = -dirLights_[i].GetLightDir();
				constMap->dirLights[i].lightColor = dirLights_[i].GetLightColor();
			}
			// ライトが無効ならライト色を0に
			else {
				constMap->dirLights[i].active = 0;
			}
		}
		for (int32_t i = 0; i < S_POINT_LIGHT_NUM_; i++) {
			//点光源
			//ライトが有効なら設定を転送
			if (pointLights_[i].GetActive())
			{
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightpos = pointLights_[i].GetLightPos();
				constMap->pointLights[i].lightcolor =
					pointLights_[i].GetLightColor();
				constMap->pointLights[i].lightatten =
					pointLights_[i].GetLightAtten();
			}
			//ライトが無効ならライト色を0に
			else
			{
				constMap->pointLights[i].active = 0;
			}
		}
		//スポットライト
		for (int32_t i = 0; i < S_SPOT_LIGHT_NUM_; i++)
		{
			//ライトが有効なら設定を転送
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
			//ライトが無効ならライト色を0に
			else
			{
				constMap->spotLights[i].active = 0;
			}
		}
		//丸影
		for (int32_t i = 0; i < S_CIRCLE_SHADOW_NUM_; i++)
		{
			//ライトが有効なら設定を転送
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
			//ライトが無効ならライト色を0に
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
	//値の更新があった時だけ定数バッファに転送
	if (dirty_)
	{
		TransferConstBuffer();
		dirty_ = false;
	}
}

void LightManager::Draw(uint32_t rootParamaterIndex)
{
	//定数バッファビューをセット
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
	//3Dオブジェクトのインスタンスを生成
	std::unique_ptr<LightManager> instance = std::make_unique< LightManager>();
	//初期化
	instance->Initialize();
	//生成したインスタンスを返す
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
		//見つかれば
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
		//見つかれば
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
		//見つかれば
		if (!spotLights_[i].GetActive())
		{
			index = i;
			return true;
		}
	}

	return false;
}
