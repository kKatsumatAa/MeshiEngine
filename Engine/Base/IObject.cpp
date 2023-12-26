#include"IObject.h"
#include"CollisionManager.h"
#include "CameraManager.h"
#include "ObjectManager.h"

using namespace DirectX;

//ルートパラメータの設定
D3D12_ROOT_PARAMETER IObject::rootParams_[RootParamNum::count] = {};
D3D12_DESCRIPTOR_RANGE IObject::effectDescRange_[4] = {};

//--------------------------------------------------------------------
IObject::~IObject()
{
	//object毎に消えるのでいらないかも
	if (collider_)
	{
		CollisionManager::GetInstance()->RemoveCollider(collider_.get());
	}
}


IObject::IObject()
{
	HRESULT result = S_OK;

	//行列用インスタンス初期化
	cbt_.Initialize();

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

	//色初期化
	constMapMaterial_->color = { 1.0f,1.0f,1.0f,1.0f };

	//アフィン
	worldMat_ = std::make_unique<WorldMat>();

	//コライダー
	collider_ = nullptr;
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
	rootParams_[TEX].DescriptorTable.pDescriptorRanges = TextureManager::GetDescRange();//デスクリプタレンジ
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
	effectDescRange_[0] = *TextureManager::GetDescRange();//デスクリプタレンジ
	effectDescRange_[0].BaseShaderRegister++;
	rootParams_[DISSOLVE].DescriptorTable.pDescriptorRanges = &effectDescRange_[0];//デスクリプタレンジ
	rootParams_[DISSOLVE].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[DISSOLVE].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ2番（スペキュラマップ用テクスチャ）
	rootParams_[SPECULAR_MAP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	effectDescRange_[1] = *TextureManager::GetDescRange();//デスクリプタレンジ
	effectDescRange_[1].BaseShaderRegister += 2;
	rootParams_[SPECULAR_MAP].DescriptorTable.pDescriptorRanges = &effectDescRange_[1];//デスクリプタレンジ
	rootParams_[SPECULAR_MAP].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[SPECULAR_MAP].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ3番（ノーマルマップ用テクスチャ）
	rootParams_[NORM_MAP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	effectDescRange_[2] = *TextureManager::GetDescRange();//デスクリプタレンジ
	effectDescRange_[2].BaseShaderRegister += 3;
	rootParams_[NORM_MAP].DescriptorTable.pDescriptorRanges = &effectDescRange_[2];//デスクリプタレンジ
	rootParams_[NORM_MAP].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[NORM_MAP].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//シャドウマップテクスチャ
	rootParams_[SHADOW_TEX].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	effectDescRange_[3] = *TextureManager::GetDescRange();//デスクリプタレンジ
	effectDescRange_[3].BaseShaderRegister += 4;
	rootParams_[SHADOW_TEX].DescriptorTable.pDescriptorRanges = &effectDescRange_[3];//デスクリプタレンジ
	rootParams_[SHADOW_TEX].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[SHADOW_TEX].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ7番（メッシュ分割のウェーブ用）
	rootParams_[TESS_WAVE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[TESS_WAVE].Descriptor.ShaderRegister = 7;//定数バッファ番号(b7)
	rootParams_[TESS_WAVE].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[TESS_WAVE].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
}

//-------------------------------------------------------------------------
void IObject::Initialize(std::unique_ptr<WorldMat> worldMat)
{
	if (worldMat)
	{
		worldMat_.reset();

		//引数のインスタンスの所有権移動
		worldMat_ = std::move(worldMat);
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
	isValidDraw_ = isValid;
	isValidUpdate_ = isValid;
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
	ColliderUpdate();
}

void IObject::ColliderUpdate()
{
	//当たり判定更新
	if (collider_.get())
	{
		collider_->Update();
	}
}

void IObject::Update()
{
	//行列、コライダー情報更新
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

	//同じ名前の場合にimguiが使えなくなるので
	str = str + "-" + std::to_string(instanceNum_);

	ImGui::Begin(str.c_str());

	//生死フラグ
	ImGui::Checkbox("isAlive: ", &isAlive_);
	if (isAlive_ == false && collider_)
	{
		collider_->SetIsValid(false);
	}

	//有効フラグ
	ImGui::Checkbox("isValidUpdate: ", &isValidUpdate_);
	ImGui::Checkbox("isValidDraw: ", &isValidDraw_);

	//トランスなど
	if (ImGui::TreeNode("TransScaleRot")) {

		ImGui::DragFloat3("Trans: ", &worldMat_->trans_.x);
		ImGui::DragFloat3("Scale: ", &worldMat_->scale_.x);
		ImGui::DragFloat3("Rot: ", &worldMat_->rot_.x);

		ImGui::TreePop();
	}

	//色
	ImGui::ColorEdit4("color", &constMapMaterial_->color.x);

	//派生クラスごとの
	if (imguiF)
	{
		imguiF();
	}

	ImGui::End();
}

//---------------------------------------------------------------------------------------------------------------------
void IObject::SetRootPipe(RootPipe* pipelineSet, int32_t pipelineNum, ID3D12RootSignature* rootSignature)
{
	// パイプラインステートとルートシグネチャの設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(pipelineSet[pipelineNum].pipelineState.Get());

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature);
}


//---------------------------------------------------------------------------------------------------------------------
void IObject::PipeLineSetting(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe,
	const std::string& vSName, const std::string& pSName,
	const std::string& gSName, const std::string& hSName, const std::string& dSName,
	D3D12_INPUT_ELEMENT_DESC* inputLayout, uint32_t inputLCount,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE priTopoType, int32_t numRTarget, bool isSprite)
{
	HRESULT result = {};

	rootPipe.CreateBlob(vSName.c_str(), pSName.c_str(), gSName.c_str(), hSName.c_str(), dSName.c_str());

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = rootPipe.vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = rootPipe.vsBlob->GetBufferSize();
	if (pSName.size())
	{
		pipelineDesc.PS.pShaderBytecode = rootPipe.psBlob->GetBufferPointer();
		pipelineDesc.PS.BytecodeLength = rootPipe.psBlob->GetBufferSize();
	}
	else
	{
		pipelineDesc.PS.pShaderBytecode = nullptr;
		pipelineDesc.PS.BytecodeLength = 0;
	}
	//ハル
	if (hSName.size())
	{
		pipelineDesc.HS.pShaderBytecode = rootPipe.hsBlob->GetBufferPointer();
		pipelineDesc.HS.BytecodeLength = rootPipe.hsBlob->GetBufferSize();
	}
	//ドメイン
	if (dSName.size())
	{
		pipelineDesc.DS.pShaderBytecode = rootPipe.dsBlob->GetBufferPointer();
		pipelineDesc.DS.BytecodeLength = rootPipe.dsBlob->GetBufferSize();
	}


	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	if (isSprite)
	{
		pipelineDesc.RasterizerState = D3D12_RASTERIZER_DESC();
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}
	else
	{
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面カリング
	}

	pipelineDesc.RasterizerState.FillMode = fillMode; // ポリゴン内塗りつぶしかどうか
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	//ブレンド
	Blend(pipelineDesc, D3D12_BLEND_OP_ADD, false, true);

	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = inputLCount;

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = priTopoType;

	// その他の設定
	pipelineDesc.NumRenderTargets = numRTarget; // 描画対象は基本3つ（ポストエフェクトの一枚目の3つ）
	//レンダーターゲットの数によって設定変えるため
	for (int32_t i = 0; i < 3; i++)
	{
		if (numRTarget > 0)
		{
			pipelineDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
		}
		else
		{
			pipelineDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;//レンダーターゲット必要なし
		}
	}
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//04_02
	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	//通常テクスチャの設定
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //横繰り返し（タイリング）
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //縦繰り返し（タイリング）
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //奥行き繰り返し（タイリング）
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;//ボーダーの時は黒
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;    //全てリニア補間
	samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;                  //ミップマップ最大値
	samplerDesc[0].MinLOD = 0.0f;                               //ミップマップ最小値
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダーからのみ使用可能
	//シャドウマップ用テクスチャの設定-------
	samplerDesc[1] = samplerDesc[0];
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	//<=だったらtrue(1.0f),それ以外は0.0f
	samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	samplerDesc[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;    //比較結果をバイリニア補間
	samplerDesc[1].MaxAnisotropy = 1;    //深度傾斜を有効
	samplerDesc[1].ShaderRegister = 1;
	//samplerDesc[1].MipLODBias = 0.0f;
	samplerDesc[1].MinLOD = -D3D12_FLOAT32_MAX;

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams_;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams_);//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 2;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		rootSigBlob.GetAddressOf(), rootPipe.errorBlob.GetAddressOf());
	assert(SUCCEEDED(result));
	result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootPipe.rootSignature));
	assert(SUCCEEDED(result));
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootPipe.rootSignature.Get();

	//06_01
	//デプスステンシルステート
	pipelineDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC();
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	if (isSprite)
	{
		pipelineDesc.DepthStencilState.DepthEnable = false;
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	}
	else
	{
		pipelineDesc.DepthStencilState.DepthEnable = true;//深度テストを行う
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さければ合格
	}
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(&pipelineDesc,
		IID_PPV_ARGS(rootPipe.pipelineState.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void IObject::Blend(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipelineDesc, const D3D12_BLEND_OP& blendMode, bool Inversion, bool Translucent)
{
	//共通設定
	D3D12_RENDER_TARGET_BLEND_DESC& blendDescL = pipelineDesc.BlendState.RenderTarget[0];
	blendDescL.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA全てのチャンネルを描画
	blendDescL.BlendEnable = true;//ブレンドを有効
	blendDescL.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDescL.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDescL.DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//合成ごと
	blendDescL.BlendOp = blendMode;
	if (Inversion)//反転
	{
		blendDescL.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
		blendDescL.DestBlend = D3D12_BLEND_ZERO;//使わない
	}
	else if (Translucent)//半透明
	{
		blendDescL.SrcBlend = D3D12_BLEND_SRC_ALPHA;//1.0f-デストカラーの値
		blendDescL.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
	}
	else
	{
		blendDescL.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
		blendDescL.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	}

	//ポストエフェクトの一枚目の二つ目用に
	pipelineDesc.BlendState.RenderTarget[1] = blendDescL;
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
		assert(false);
	}
}