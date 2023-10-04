#include"IObject.h"
#include"CollisionManager.h"
#include "CameraManager.h"
#include "ObjectManager.h"

using namespace DirectX;

//ルートパラメータの設定
D3D12_ROOT_PARAMETER IObject::rootParams_[RootParamNum::count] = {};
D3D12_GRAPHICS_PIPELINE_STATE_DESC IObject::pipelineDesc_ = {};

//--------------------------------------------------------------------
IObject::~IObject()
{
	constBuffMaterial_.Reset();

	//object毎に消えるのでいらないかも
	if (collider_.get())
	{
		CollisionManager::GetInstance()->RemoveCollider(collider_.get());
	}
}


IObject::IObject()
{
	Construct();
}

void IObject::Construct()
{
	HRESULT result = S_OK;

	//マテリアルバッファ（色）
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	ResourceProperties(cbResourceDesc,
		((uint32_t)sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff/*256バイトアライメント*/);
	//定数バッファの生成
	BuffProperties(cbHeapProp, cbResourceDesc, &constBuffMaterial_);
	//定数バッファのマッピング
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial_);//マッピング
	assert(SUCCEEDED(result));
}

//------------------------------------------------------------------------
void IObject::CommonInitialize()
{
	//ルートパラメータの設定
	//定数バッファ0番（色）
	rootParams_[COLOR].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[COLOR].Descriptor.ShaderRegister = 0;//定数バッファ番号(b0)
	rootParams_[COLOR].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[COLOR].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ0番（テクスチャ）
	rootParams_[TEX].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	rootParams_[TEX].DescriptorTable.pDescriptorRanges = &TextureManager::GetDescRange();//デスクリプタレンジ
	rootParams_[TEX].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[TEX].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ1番(行列)
	rootParams_[MATRIX].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[MATRIX].Descriptor.ShaderRegister = 1;//定数バッファ番号(b1)
	rootParams_[MATRIX].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[MATRIX].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//メッシュごとの行列2番
	rootParams_[MESH_MAT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[MESH_MAT].Descriptor.ShaderRegister = 2;//定数バッファ番号(b2)
	rootParams_[MESH_MAT].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[MESH_MAT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ3番（マテリアル）
	rootParams_[MATERIAL].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[MATERIAL].Descriptor.ShaderRegister = 3;//定数バッファ番号(b3)
	rootParams_[MATERIAL].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[MATERIAL].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ4番(ライト用)
	rootParams_[LIGHT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[LIGHT].Descriptor.ShaderRegister = 4;//定数バッファ番号(b4)
	rootParams_[LIGHT].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[LIGHT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ5番(効果フラグ)
	rootParams_[EFFECT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[EFFECT].Descriptor.ShaderRegister = 5;//定数バッファ番号(b5)
	rootParams_[EFFECT].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[EFFECT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ6番（スキニング用）
	rootParams_[SKIN].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[SKIN].Descriptor.ShaderRegister = 6;//定数バッファ番号(b6)
	rootParams_[SKIN].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[SKIN].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ1番（ディゾルブ用テクスチャ）
	rootParams_[DISSOLVE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	D3D12_DESCRIPTOR_RANGE dissolveDHRange = TextureManager::GetDescRange();//デスクリプタレンジ
	dissolveDHRange.BaseShaderRegister++;
	rootParams_[DISSOLVE].DescriptorTable.pDescriptorRanges = &dissolveDHRange;//デスクリプタレンジ
	rootParams_[DISSOLVE].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[DISSOLVE].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ2番（スペキュラマップ用テクスチャ）
	rootParams_[SPECULAR_MAP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	D3D12_DESCRIPTOR_RANGE dissolveDHRange2 = TextureManager::GetDescRange();//デスクリプタレンジ
	dissolveDHRange2.BaseShaderRegister += 2;
	rootParams_[SPECULAR_MAP].DescriptorTable.pDescriptorRanges = &dissolveDHRange2;//デスクリプタレンジ
	rootParams_[SPECULAR_MAP].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[SPECULAR_MAP].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ3番（ノーマルマップ用テクスチャ）
	rootParams_[NORM_MAP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	D3D12_DESCRIPTOR_RANGE dissolveDHRange3 = TextureManager::GetDescRange();//デスクリプタレンジ;
	dissolveDHRange3.BaseShaderRegister += 3;
	rootParams_[NORM_MAP].DescriptorTable.pDescriptorRanges = &dissolveDHRange3;//デスクリプタレンジ
	rootParams_[NORM_MAP].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[NORM_MAP].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
}

//-------------------------------------------------------------------------
void IObject::Initialize(std::unique_ptr<WorldMat> worldMat)
{
	if (worldMat == nullptr)
	{
		worldMat = std::move(worldMat_);
	}
}

//---------------------------------------------------------------------
void IObject::SetColliderIsValid(bool isValid)
{
	if (collider_)
	{
		collider_->SetIsValid(isValid);
	}
}

void IObject::SetCollider(std::unique_ptr<BaseCollider> collider)
{
	collider->SetObject(this);
	collider_ = std::move(collider);
	//コリジョンマネージャーに登録
	CollisionManager::GetInstance()->AddCollider(collider_.get());
	//行列,コライダーの更新
	WorldMatColliderUpdate();
}

void IObject::SetIsAlive(bool isAlive)
{
	isAlive_ = isAlive;
	//当たり判定の方も無効
	if (collider_)
	{
		collider_->SetIsValid(false);
	}
}

void IObject::SetIsValid(bool isValid)
{
	isValid_ = isValid;
	SetColliderIsValid(isValid);

	if (isValid == true)
	{
		worldMat_->CalcAllTreeMat();
	}
}

//----------------------------------------------------------------------
void IObject::WorldMatColliderUpdate()
{
	//行列更新（ワールド座標系にして当たり判定を行う）
	worldMat_->CalcAllTreeMat();
	//当たり判定更新
	if (collider_.get())
	{
		collider_->Update();
	}
}

void IObject::Update()
{
	WorldMatColliderUpdate();
}

//----------------------------------------------------------------------
void IObject::DrawImGui(std::function<void()>imguiF)
{
	std::string str = "NO_NAME";
	if (objName_.size())
	{
		str = objName_;
	}

	ImGui::Begin(str.c_str());

	//生死フラグ
	ImGui::Checkbox("isAlive: ", &isAlive_);
	if (isAlive_ == false && collider_)
	{
		collider_->SetIsValid(false);
	}

	//トランスなど
	if (ImGui::TreeNode("TransScaleRot")) {

		ImGui::DragFloat3("Trans: ", &worldMat_->trans_.x);
		ImGui::DragFloat3("Scale: ", &worldMat_->scale_.x);
		ImGui::DragFloat3("Rot: ", &worldMat_->rot_.x);

		ImGui::TreePop();
	}

	//派生クラスごとの
	if (imguiF)
	{
		imguiF();
	}

	ImGui::End();
}

//---------------------------------------------------------------------------------------------------------------------
void IObject::SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature)
{
	// パイプラインステートとルートシグネチャの設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(&pipelineState[pipelineNum]);

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature);
}


//---------------------------------------------------------------------------------------------------------------------
void IObject::PipeLineSetting(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe,
	const std::string& vSName, const std::string& pSName,
	D3D12_INPUT_ELEMENT_DESC* inputLayout, uint32_t inputLCount,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE priTopoType, bool isSprite)
{
	HRESULT result = {};

	rootPipe.CreateBlob(vSName.c_str(), pSName.c_str());

	// シェーダーの設定
	pipelineDesc_.VS.pShaderBytecode = rootPipe.vsBlob->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = rootPipe.vsBlob->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = rootPipe.psBlob->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = rootPipe.psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	if (isSprite)
	{
		pipelineDesc_.RasterizerState = D3D12_RASTERIZER_DESC();
		pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}
	else
	{
		pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面カリング
	}

	pipelineDesc_.RasterizerState.FillMode = fillMode; // ポリゴン内塗りつぶし
	pipelineDesc_.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	Blend(D3D12_BLEND_OP_ADD, false, true);

	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc_.InputLayout.NumElements = inputLCount;

	// 図形の形状設定
	pipelineDesc_.PrimitiveTopologyType = priTopoType;

	// その他の設定
	pipelineDesc_.NumRenderTargets = 3; // 描画対象は3つ（ポストエフェクトの一枚目の3つ）
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc_.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc_.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc_.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//04_02
	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //奥行き繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;    //全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                  //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;                               //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダーからのみ使用可能

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams_;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams_);//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		rootSigBlob.ReleaseAndGetAddressOf(), rootPipe.errorBlob.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(result));
	result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootPipe.rootSignature));
	assert(SUCCEEDED(result));
	// パイプラインにルートシグネチャをセット
	pipelineDesc_.pRootSignature = rootPipe.rootSignature.Get();

	//06_01
	//デプスステンシルステート
	pipelineDesc_.DepthStencilState = D3D12_DEPTH_STENCIL_DESC();

	pipelineDesc_.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	if (isSprite)
	{
		pipelineDesc_.DepthStencilState.DepthEnable = false;
		pipelineDesc_.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	}
	else
	{
		pipelineDesc_.DepthStencilState.DepthEnable = true;//深度テストを行う
		pipelineDesc_.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さければ合格
	}
	pipelineDesc_.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_, IID_PPV_ARGS(rootPipe.pipelineState.GetAddressOf()));
	assert(SUCCEEDED(result));
}

void IObject::Blend(const D3D12_BLEND_OP& blendMode, bool Inversion, bool Translucent)
{
	//共通設定
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc_.BlendState.RenderTarget[0];
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA全てのチャンネルを描画
	blendDesc.BlendEnable = true;//ブレンドを有効
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//合成ごと
	blendDesc.BlendOp = blendMode;
	if (Inversion)//反転
	{
		blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
		blendDesc.DestBlend = D3D12_BLEND_ZERO;//使わない
	}
	else if (Translucent)//半透明
	{
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//1.0f-デストカラーの値
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
	}
	else
	{
		blendDesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	}

	//ポストエフェクトの一枚目の二つ目用に
	pipelineDesc_.BlendState.RenderTarget[1] = blendDesc;
}

void Error(bool filed, ID3DBlob* errorBlob)
{
	if (filed)
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}