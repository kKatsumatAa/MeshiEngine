#include "Object.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "BaseCollider.h"
#include "CollisionManager.h"

//図形のクラス
Primitive primitive;


//テクスチャ

// 頂点レイアウト
D3D12_INPUT_ELEMENT_DESC inputLayout[3] = {
{//xyz座標
 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
}, // (1行で書いたほうが見やすい)

	{//法線ベクトル
 "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
}, // (1行で書いたほうが見やすい)

{//uv座標
 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
} // (1行で書いたほうが見やすい)
};

//sprite
D3D12_INPUT_ELEMENT_DESC inputLayoutSprite[2] = {
	{//xyz座標
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1行で書いたほうが見やすい)

	{//uv座標
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	} // (1行で書いたほうが見やすい)
};
PipeLineSet pipelineSet;
//al4_02_02
PipeLineSet pipelineSetM;

//ルートパラメータの設定
D3D12_ROOT_PARAMETER rootParams[6] = {};

// パイプランステートの生成
ComPtr < ID3D12PipelineState> pipelineState[3] = { nullptr };
// ルートシグネチャ
ComPtr<ID3D12RootSignature> rootSignature;
// グラフィックスパイプライン設定
D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
ID3DBlob* errorBlob = nullptr; // エラーオブジェクト

// 2.描画先の変更
	// レンダーターゲットビューのハンドルを取得
D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

//static
LightManager* Object::lightManager = nullptr;

PostPera postPera;

ComPtr <ID3D12Resource> Object::effectFlagsBuff = nullptr;
EffectConstBuffer* Object::mapEffectFlagsBuff = nullptr;
EffectConstBuffer Object::effectFlags;


struct weightMap
{
	XMFLOAT4 mappedWeight[2];
};

void DrawInitialize()
{
	//テクスチャ用のデスクリプタヒープ初期化
	TextureManager::GetInstance().InitializeDescriptorHeap();

	//図形クラスの
	primitive.Initialize();

	//ルートパラメータの設定
	//定数バッファ0番（色）
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;//定数バッファ番号(b0)
	rootParams[0].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ0番（テクスチャ）
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	rootParams[1].DescriptorTable.pDescriptorRanges = &TextureManager::GetInstance().descriptorRange;//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ1番(行列)
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[2].Descriptor.ShaderRegister = 1;//定数バッファ番号(b1)
	rootParams[2].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ2番（マテリアル）
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[3].Descriptor.ShaderRegister = 2;//定数バッファ番号(b2)
	rootParams[3].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ3番(ライト用)
	rootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[4].Descriptor.ShaderRegister = 3;//定数バッファ番号(b3)
	rootParams[4].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ4番(画面効果フラグ)
	rootParams[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[5].Descriptor.ShaderRegister = 4;//定数バッファ番号(b4)
	rootParams[5].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	// パイプランステートの生成
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineState->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob);

	PipeLineState(D3D12_FILL_MODE_WIREFRAME, (pipelineState + 1)->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob);

	//line
	PipeLineState(D3D12_FILL_MODE_WIREFRAME, (pipelineState + 2)->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob, LINE);

	//sprite用
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSet.pipelineState.GetAddressOf(),
		pipelineSet.rootSignature.GetAddressOf(), pipelineSet.vsBlob,
		pipelineSet.psBlob, SPRITE);

	//model用
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSetM.pipelineState.GetAddressOf(),
		pipelineSetM.rootSignature.GetAddressOf(), pipelineSetM.vsBlob,
		pipelineSetM.psBlob, MODEL);

	//ポストエフェクト初期化
	postPera.Initialize(L"Resources/image/normalImage.jpg");

	//画面効果用
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		ResourceProperties(cbResourceDesc,
			((UINT)sizeof(EffectConstBuffer) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &Object::effectFlagsBuff);
		//定数バッファのマッピング
		DirectXWrapper::GetInstance().result = Object::effectFlagsBuff->Map(0, nullptr, (void**)&Object::mapEffectFlagsBuff);//マッピング
		assert(SUCCEEDED(DirectXWrapper::GetInstance().result));
	}
}

//----------------------------------------------------------------
bool Object::Initialize()
{
	name = typeid(*this).name();
	return true;
}

Object::~Object()
{
	if (collider)
	{
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
}

void Object::Update()
{
	//行列更新
	worldMat->SetWorld();
	//当たり判定更新
	if (collider)
	{
		collider->Update();
	}
}

void Object::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	//コリジョンマネージャーに登録
	CollisionManager::GetInstance()->AddCollider(collider);
	//コライダーを更新
	collider->Update();
}

void Object::SetIsValid(bool isValid)
{
	if (collider)
	{
		collider->SetIsValid(isValid);
	}
}

void Object::SetIs2D(bool is2D)
{
	if (collider)
	{
		collider->SetIs2D(is2D);
	}
}


//-------------------------------------------------------------------

Object::Object()
{
	//行列
	cbt.Initialize(DirectXWrapper::GetInstance());

	//03_02
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	ResourceProperties(cbResourceDesc,
		((UINT)sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff/*256バイトアライメント*/);
	//定数バッファの生成
	BuffProperties(cbHeapProp, cbResourceDesc, &constBuffMaterial);
	//定数バッファのマッピング
	DirectXWrapper::GetInstance().result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
	assert(SUCCEEDED(DirectXWrapper::GetInstance().result));
}

void Object::SendingMat(int indexNum)
{
	worldMat->SetWorld();

	//変換行列をGPUに送信
	if (worldMat->parent != nullptr && indexNum != SPRITE)//親がいる場合
	{
		worldMat->matWorld *= worldMat->parent->matWorld;
	}
	//親がいない場合
	if (indexNum != SPRITE)
	{
		XMMATRIX matW;
		matW = { (float)worldMat->matWorld.m[0][0],(float)worldMat->matWorld.m[0][1],(float)worldMat->matWorld.m[0][2],(float)worldMat->matWorld.m[0][3],
				 (float)worldMat->matWorld.m[1][0],(float)worldMat->matWorld.m[1][1],(float)worldMat->matWorld.m[1][2],(float)worldMat->matWorld.m[1][3],
				 (float)worldMat->matWorld.m[2][0],(float)worldMat->matWorld.m[2][1],(float)worldMat->matWorld.m[2][2],(float)worldMat->matWorld.m[2][3],
				 (float)worldMat->matWorld.m[3][0],(float)worldMat->matWorld.m[3][1],(float)worldMat->matWorld.m[3][2],(float)worldMat->matWorld.m[3][3] };

		cbt.constMapTransform->world = matW;
		cbt.constMapTransform->viewproj = view->matView * projection->matProjection;
		XMFLOAT3 cPos = { view->eye.x,view->eye.y,view->eye.z };
		cbt.constMapTransform->cameraPos = cPos;
	}
}

void Object::SetRootPipe(ID3D12PipelineState* pipelineState, int pipelineNum, ID3D12RootSignature* rootSignature)
{
	// パイプラインステートとルートシグネチャの設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(&pipelineState[pipelineNum]);

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature);
}

void Object::SetMaterialLightMTex(UINT64 textureHandle_, ConstBuffTransform cbt)
{
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	lightManager->Draw(4);

	//04_02
	{
		//SRVヒープの設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
		//SRVヒープの先頭ハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
		srvGpuHandle.ptr = textureHandle_;
		//(インスタンスで読み込んだテクスチャ用のSRVを指定)
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	}

	//定数バッファビュー(CBV)の設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, cbt.constBuffTransform->GetGPUVirtualAddress());

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(5, effectFlagsBuff->GetGPUVirtualAddress());
}

void Object::Update(const int& indexNum, const int& pipelineNum, const UINT64 textureHandle, const ConstBuffTransform& constBuffTransform,
	Model* model, const bool& primitiveMode)
{
	//行列送信
	SendingMat(indexNum);

	//画面効果用
	{
		mapEffectFlagsBuff->isEmboss = this->effectFlags.isEmboss;
		mapEffectFlagsBuff->isFog = this->effectFlags.isFog;
		mapEffectFlagsBuff->isGaussian = this->effectFlags.isGaussian;
		mapEffectFlagsBuff->isGaussian2 = this->effectFlags.isGaussian2;
		mapEffectFlagsBuff->isGradation = this->effectFlags.isGradation;
		mapEffectFlagsBuff->isOutLine = this->effectFlags.isOutLine;
		mapEffectFlagsBuff->isSharpness = this->effectFlags.isSharpness;
		mapEffectFlagsBuff->isVignette = this->effectFlags.isVignette;
		mapEffectFlagsBuff->isBarrelCurve = this->effectFlags.isBarrelCurve;
		mapEffectFlagsBuff->isScanningLine = this->effectFlags.isScanningLine;
		mapEffectFlagsBuff->isGrayScale = this->effectFlags.isGrayScale;
		mapEffectFlagsBuff->isGlassFilter = this->effectFlags.isGlassFilter;
	}

	//テクスチャを設定していなかったら
	UINT64 textureHandle_;

	if (textureHandle == NULL)
	{
		textureHandle_ = TextureManager::GetInstance().whiteTexHandle;
	}
	else
	{
		textureHandle_ = textureHandle;
	}

	//ラムダ式でコマンド関数
	std::function<void()>SetRootPipeR = [=]() {SetRootPipe(pipelineState->Get(), pipelineNum, rootSignature.Get()); };
	std::function<void()>SetMaterialTex = [=]() {SetMaterialLightMTex(textureHandle_, constBuffTransform); };

	if (indexNum == TRIANGLE)
	{
		primitive.TriangleDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == BOX)
	{
		primitive.BoxDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == CUBE)
	{
		primitive.CubeDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == LINE)
	{
		primitive.LineDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == CIRCLE)
	{
		primitive.CircleDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == SPHERE)
	{
		primitive.SphereDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == SPRITE)
	{
		SpriteCommonBeginDraw(&pipelineSet);

		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());


		//04_02
		{
			//SRVヒープの設定コマンド
			DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle_;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		sprite->SpriteDraw();
	}
	else if (indexNum == MODEL)
	{
		// パイプラインステートとルートシグネチャの設定コマンド
		SetRootPipe(pipelineSetM.pipelineState.Get(), pipelineNum, pipelineSetM.rootSignature.Get());

		DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		lightManager->Draw(4);

		//定数バッファビュー(CBV)の設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(5, effectFlagsBuff->GetGPUVirtualAddress());

		//モデル用描画
		model->Draw();
	}
}

void Object::DrawPera()
{
	//ポストエフェクト用
	postPera.Draw(effectFlags);
}

void Object::DrawPera2()
{
	//ポストエフェクト用
	postPera.Draw2();
}

void Object::DrawTriangle(/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,*/
	WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(TRIANGLE, pipelineNum, textureHandle, cbt);
}

void Object::DrawBox(WorldMat* world, ViewMat* view, ProjectionMat* projection,/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, */
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(BOX, pipelineNum, textureHandle, cbt);
}

void Object::DrawBoxSprite(const Vec3& pos, const float& scale,
	XMFLOAT4 color, const UINT64 textureHandle, const Vec2& ancorUV, const bool& isReverseX, const bool& isReverseY,
	float rotation, const int& pipelineNum)
{
	if (sprite == nullptr)
	{
		sprite = new Sprite();
		//スプライトクラスの初期化
		sprite->Initialize();
	}
	sprite->Update(pos, scale, color, textureHandle, ancorUV, isReverseX, isReverseY, rotation, &cbt, constMapMaterial);

	Update(SPRITE, pipelineNum, textureHandle, cbt);
}

void Object::Draw()
{
}



void Object::DrawClippingBoxSprite(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	XMFLOAT4 color, const UINT64 textureHandle, bool isPosLeftTop, const bool& isReverseX, const bool& isReverseY,
	float rotation, const int& pipelineNum)
{
	if (sprite == nullptr)
	{
		sprite = new Sprite();
		//スプライトクラスの初期化
		sprite->Initialize();
	}
	sprite->UpdateClipping(leftTop, scale, UVleftTop, UVlength, color, textureHandle,
		isPosLeftTop, isReverseX, isReverseY, rotation, &cbt, constMapMaterial);

	Update(SPRITE, pipelineNum, textureHandle, cbt);
}

void Object::DrawCube3D(WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(CUBE, pipelineNum, textureHandle, cbt);
}

void Object::DrawLine(/*const Vec3& pos1, const Vec3& pos2,*/ WorldMat* world, ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color,
	const UINT64 textureHandle)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(LINE, 2, textureHandle, cbt, nullptr, false);
}

void Object::DrawCircle(WorldMat* world, ViewMat* view, ProjectionMat* projection,
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(CIRCLE, pipelineNum, textureHandle, cbt);
}

void Object::DrawSphere(WorldMat* world, ViewMat* view, ProjectionMat* projection,
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(SPHERE, pipelineNum, textureHandle, cbt);
}

void Object::DrawModel(WorldMat* world, ViewMat* view, ProjectionMat* projection,
	Model* model, XMFLOAT4 color, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(MODEL, pipelineNum, NULL, cbt, model);
}

void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, const int& indexNum)
{
	if (indexNum == SPRITE)
	{
		// 頂点シェーダの読み込みとコンパイル
		DirectXWrapper::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/SpriteVS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(DirectXWrapper::GetInstance().result));

		// ピクセルシェーダの読み込みとコンパイル
		DirectXWrapper::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/SpritePS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(DirectXWrapper::GetInstance().result));
	}
	else if (indexNum == MODEL)
	{
		// 頂点シェーダの読み込みとコンパイル
		DirectXWrapper::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/OBJVertexShader.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(DirectXWrapper::GetInstance().result));

		// ピクセルシェーダの読み込みとコンパイル
		DirectXWrapper::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/OBJPixelShader.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(DirectXWrapper::GetInstance().result));
	}
	else
	{
		// 頂点シェーダの読み込みとコンパイル
		DirectXWrapper::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/BasicVS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(DirectXWrapper::GetInstance().result));

		// ピクセルシェーダの読み込みとコンパイル
		DirectXWrapper::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/BasicPS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(DirectXWrapper::GetInstance().result));
	}

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	if (indexNum == SPRITE)
	{
		pipelineDesc.RasterizerState = D3D12_RASTERIZER_DESC();
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}
	else
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面カリング

	pipelineDesc.RasterizerState.FillMode = fillMode; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	Blend(D3D12_BLEND_OP_ADD, false, true);

	// 頂点レイアウトの設定
	if (indexNum == SPRITE)
	{
		pipelineDesc.InputLayout.pInputElementDescs = inputLayoutSprite;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayoutSprite);
	}
	else
	{
		pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	}

	// 図形の形状設定
	if (indexNum == LINE)
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	else
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

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
	rootSignatureDesc.pParameters = rootParams;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	DirectXWrapper::GetInstance().result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(DirectXWrapper::GetInstance().result));
	DirectXWrapper::GetInstance().result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSig));
	assert(SUCCEEDED(DirectXWrapper::GetInstance().result));
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = *rootSig;

	//06_01
	//デプスステンシルステート
	pipelineDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC();
	pipelineDesc.DepthStencilState.DepthEnable = true;//深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	if (indexNum == SPRITE)
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//小さければ合格
	else
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	DirectXWrapper::GetInstance().result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelineState));
	//assert(SUCCEEDED(DirectXWrapper::GetInstance().result));
}

void Blend(const D3D12_BLEND_OP& blendMode, const bool& Inversion, const bool& Translucent)
{
	//共通設定
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc.BlendState.RenderTarget[0];
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
}

void Object::constBuffTransfer(const XMFLOAT4& plusRGBA)
{
	if (constMapMaterial->color.x <= 1.0f && constMapMaterial->color.x >= 0.0f)
		constMapMaterial->color.x += plusRGBA.x;
	if (constMapMaterial->color.y <= 1.0f && constMapMaterial->color.y >= 0.0f)
		constMapMaterial->color.y += plusRGBA.y;
	if (constMapMaterial->color.z <= 1.0f && constMapMaterial->color.z >= 0.0f)
		constMapMaterial->color.z += plusRGBA.z;
	if (constMapMaterial->color.w <= 1.0f && constMapMaterial->color.w >= 0.0f)
		constMapMaterial->color.w += plusRGBA.w;
}



void SetNormDigitalMat(XMMATRIX& mat)
{
	mat.r[0].m128_f32[0] = 2.0f / WindowsApp::GetInstance().window_width;
	mat.r[1].m128_f32[1] = -2.0f / WindowsApp::GetInstance().window_height;
	mat.r[3].m128_f32[0] = -1.0f;
	mat.r[3].m128_f32[1] = 1.0f;
}

void Error(const bool& filed)
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
