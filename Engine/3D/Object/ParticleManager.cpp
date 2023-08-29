#include "ParticleManager.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "CameraManager.h"

using namespace DirectX;


ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager sInstance;
	return &sInstance;
}

void ParticleManager::Initialize()
{
	// パイプライン初期化
	InitializeGraphicsPipeline();

	// モデル生成
	CreateModel();

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	ResourceProperties(cbResourceDesc,
		((uint32_t)sizeof(ConstBufferData) + 0xff) & ~0xff/*256バイトアライメント*/);
	//定数バッファの生成
	BuffProperties(cbHeapProp, cbResourceDesc, &constBuff_);
}

void ParticleManager::Update(float speed, Camera* camera)
{
	HRESULT result;

	if (particles_.empty())
	{
		return;
	}

	//カメラがセットされてなかったら
	Camera* cameraL = camera;
	if (camera == nullptr)
	{
		cameraL = CameraManager::GetInstance().usingCamera_;
	}

	// 全パーティクル更新
	for (std::forward_list<Particle>::iterator it = particles_.begin();
		it != particles_.end();
		it++) {


		// 経過フレーム数をカウント
		it->frame_ += 1.0f * speed;
		// 進行度を0～1の範囲に換算
		float f = (float)it->numFrame_ / it->frame_;

		// 速度に加速度を加算
		it->velocity_ = it->velocity_ + it->accel_;

		// 速度による移動
		it->position_ = it->position_ + it->velocity_ * speed;

		// カラーの線形補間
		it->color_ = it->sColor_ + (it->eColor_ - it->sColor_) / f;

		// スケールの線形補間
		it->scale_ = it->sScale_ + (it->eScale_ - it->sScale_) / f;

		// 回転の線形補間
		float t = (float)it->frame_ / it->numFrame_;
		it->rotation_ = LerpVec3(it->sRotation_, it->eRotation_, t);
	}

	// 寿命が尽きたパーティクルを全削除
	particles_.remove_if([](Particle& x) { return x.frame_ >= x.numFrame_; });

	// 頂点バッファへデータ転送
	int32_t vertCount = 0;
	VertexPos* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		// パーティクルの情報を1つずつ反映
		for (std::forward_list<Particle>::iterator it = particles_.begin();
			it != particles_.end();
			it++) {
			// 座標
			vertMap->pos = it->position_;
			// スケール
			vertMap->scale = it->scale_;
			// 色
			vertMap->color = it->color_;
			// 色
			vertMap->rot = it->rotation_;
			// 次の頂点へ
			vertMap++;
			if (++vertCount >= S_VERTEX_COUNT_) {
				break;
			}
		}
		vertBuff_->Unmap(0, nullptr);
	}

	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	constMap->mat = cameraL->GetViewMat() * cameraL->GetProjMat();
	constMap->matBillboard = cameraL->GetBillboardMat();
	constBuff_->Unmap(0, nullptr);
}

void ParticleManager::Draw(uint64_t texHandle)
{
	if (texHandle == NULL)
	{
		texHandle = TextureManager::GetWhiteTexHandle();
	}

	uint32_t drawNum = (uint32_t)std::distance(particles_.begin(), particles_.end());
	if (drawNum > S_VERTEX_COUNT_) {
		drawNum = S_VERTEX_COUNT_;
	}

	// パーティクルが1つもない場合
	if (drawNum == 0) {
		return;
	}

	// パイプラインステートの設定
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(rootPipe[blendNum_].pipelineState.Get());
	// ルートシグネチャの設定
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootPipe[blendNum_].rootSignature.Get());
	// プリミティブ形状を設定
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// 頂点バッファの設定
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::GetDescHeapP() };
	DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 定数バッファビューをセット
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	srvGpuHandle.ptr = texHandle;
	//(インスタンスで読み込んだテクスチャ用のSRVを指定)
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	// 描画コマンド
	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(drawNum, 1, 0, 0);
}

void ParticleManager::Add(int32_t life, const Vec3& position, const Vec3& velocity, const Vec3& accel, float start_scale, float end_scale
	, const XMFLOAT4& start_color, const XMFLOAT4& end_color, float start_rot, float end_rot)
{
	// リストに要素を追加
	particles_.emplace_front();
	// 追加した要素の参照
	Particle& p = particles_.front();
	p.position_ = position;
	p.velocity_ = velocity;
	p.accel_ = accel;
	p.sScale_ = start_scale;
	p.eScale_ = end_scale;
	p.numFrame_ = life;
	p.sColor_ = start_color;
	p.eColor_ = end_color;

	p.sRotation_.x_ = start_rot * GetRand(-2.0f, 2.0f);
	p.sRotation_.y_ = start_rot * GetRand(-2.0f, 2.0f);
	p.sRotation_.z_ = start_rot * GetRand(-2.0f, 2.0f);
	p.eRotation_.x_ = end_rot * GetRand(-2.0f, 2.0f);
	p.eRotation_.y_ = end_rot * GetRand(-2.0f, 2.0f);
	p.eRotation_.z_ = end_rot * GetRand(-2.0f, 2.0f);
}

void ParticleManager::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	//シェーダ読み込み
	rootPipe[ADD].CreateBlob("Resources/shaders/ParticleVS.hlsl", "Resources/shaders/ParticlePS.hlsl", "Resources/shaders/ParticleGS.hlsl");

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // スケール
			"TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 色
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 角度
			"ROT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(rootPipe[ADD].vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(rootPipe[ADD].psBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(rootPipe[ADD].gsBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	// デプスの書き込みを禁止
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	// 加算ブレンディング
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	gpipeline.BlendState.RenderTarget[2] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	gpipeline.NumRenderTargets = 3;	// 描画対象は3つ(ポストエフェクトのマルチレンダーターゲット)
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &rootPipe[ADD].errorBlob);
	// ルートシグネチャの生成
	result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootPipe[ADD].rootSignature));
	if (FAILED(result)) {
		assert(0);
	}

	gpipeline.pRootSignature = rootPipe[ADD].rootSignature.Get();

	// グラフィックスパイプラインの生成
	//加算
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(rootPipe[ADD].pipelineState.GetAddressOf()));

	//減算
	{
		//減算
		rootPipe[SUB] = rootPipe[ADD];
		// ルートシグネチャの生成
		result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
			IID_PPV_ARGS(rootPipe[SUB].rootSignature.GetAddressOf()));
		if (FAILED(result)) {
			assert(0);
		}
		gpipeline.pRootSignature = rootPipe[SUB].rootSignature.Get();
		// 減算ブレンディング
		blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		gpipeline.BlendState.RenderTarget[1] = blenddesc;
		gpipeline.BlendState.RenderTarget[2] = blenddesc;
		result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(rootPipe[SUB].pipelineState.GetAddressOf()));
	}
	//三角メッシュ
	{
		rootPipe[TRIANGLE] = rootPipe[SUB];
		rootPipe[TRIANGLE].CreateBlob("Resources/shaders/ParticleVS.hlsl", "Resources/shaders/ParticlePS.hlsl", "Resources/shaders/ParticleTriangleGS.hlsl");
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(rootPipe[TRIANGLE].gsBlob.Get());
		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
			IID_PPV_ARGS(rootPipe[TRIANGLE].rootSignature.GetAddressOf()));
		gpipeline.pRootSignature = rootPipe[TRIANGLE].rootSignature.Get();
		if (FAILED(result)) {
			assert(0);
		}
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		gpipeline.BlendState.RenderTarget[1] = blenddesc;
		gpipeline.BlendState.RenderTarget[2] = blenddesc;
		result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(rootPipe[TRIANGLE].pipelineState.GetAddressOf()));
	}

	if (FAILED(result)) {
		assert(0);
	}
}

void ParticleManager::CreateModel()
{
	HRESULT result = S_FALSE;

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	ResourceProperties(cbResourceDesc,
		((uint32_t)sizeof(VertexPos) * S_VERTEX_COUNT_));
	//定数バッファの生成
	BuffProperties(cbHeapProp, cbResourceDesc, &vertBuff_);

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(VertexPos) * S_VERTEX_COUNT_;
	vbView_.StrideInBytes = sizeof(VertexPos);
}

void ParticleManager::GenerateRandomParticle(int32_t num, int32_t lifeTime, float vecPower, Vec3 position, float start_scale, float end_scale, const XMFLOAT4& start_color, const XMFLOAT4& end_color)
{
	for (int32_t i = 0; i < num; i++)
	{
		Vec3 pos = position;

		const float MD_VEL = vecPower;
		Vec3 vel{};
		vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

		//重力に見立ててYのみ[-0.001f~0]でランダムに
		Vec3 acc{};
		const float MD_ACC = vecPower * 0.05f;
		acc.y_ = -(float)rand() / RAND_MAX * MD_ACC;

		ParticleManager::GetInstance()->Add(lifeTime, { pos.x_,pos.y_,pos.z_ }, vel, acc, start_scale, end_scale
			, start_color, end_color);
	}
}

ParticleManager::~ParticleManager()
{
	{ particles_.clear(); }
}
