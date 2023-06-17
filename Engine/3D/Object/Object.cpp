#include "Object.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "ImguiManager.h"

//図形のクラス
Primitive primitive;


//テクスチャ

// 頂点レイアウト
D3D12_INPUT_ELEMENT_DESC inputLayout[5] = {
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
}, // (1行で書いたほうが見やすい)

{//影響を受けるボーン番号
 "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
},

{//ボーンのスキンウェイト（4つ）
 "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
}
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

//FBX用
PipeLineSet pipelineSetFBX;

//ルートパラメータの設定
D3D12_ROOT_PARAMETER rootParams[7] = {};

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
LightManager* Object::sLightManager_ = nullptr;

//演出用
ComPtr <ID3D12Resource> Object::sEffectFlagsBuff_ = nullptr;
EffectOConstBuffer* Object::sMapEffectFlagsBuff_ = nullptr;
EffectOConstBuffer Object::sEffectFlags_;
float Object::sRimColorF3_[3] = { 1.0f,1.0f,1.0f };


struct weightMap
{
	XMFLOAT4 mappedWeight[2];
};

void DrawInitialize()
{
	HRESULT result = {};

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
	rootParams[1].DescriptorTable.pDescriptorRanges = &TextureManager::GetInstance().sDescriptorRange_;//デスクリプタレンジ
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
	//定数バッファ5番（スキニング用）
	rootParams[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[6].Descriptor.ShaderRegister = 5;//定数バッファ番号(b5)
	rootParams[6].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

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

	//FBX用
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSetFBX.pipelineState.GetAddressOf(),
		pipelineSetFBX.rootSignature.GetAddressOf(), pipelineSetFBX.vsBlob,
		pipelineSetFBX.psBlob, FBX);


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
			((uint32_t)sizeof(EffectConstBuffer) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &Object::sEffectFlagsBuff_);
		//定数バッファのマッピング
		result = Object::sEffectFlagsBuff_->Map(0, nullptr, (void**)&Object::sMapEffectFlagsBuff_);//マッピング
		assert(SUCCEEDED(result));
	}
}

//----------------------------------------------------------------
bool Object::Initialize()
{
	NAME = typeid(*this).name();
	return true;
}

Object::~Object()
{
	//delete& cbt;
	constBuffMaterial_.Reset();
	constBuffSkin_.Reset();

	//object毎に消えるのでいらないかも
	if (collider_.get())
	{
		CollisionManager::GetInstance()->RemoveCollider(collider_.get());
	}
}

void Object::Update()
{
	//行列更新
	worldMat_->SetWorld();
	//当たり判定更新
	if (collider_.get())
	{
		collider_->Update();
	}
}

void Object::StaticUpdate()
{
	sEffectFlags_.time++;
	//imgui
	ImGui::Begin("ObjectEffect");
	ImGui::SliderInt("Fog", (int32_t*)&sEffectFlags_.isFog, 0, 1);
	ImGui::SliderInt("Toon", (int32_t*)&sEffectFlags_.isToon, 0, 1);
	ImGui::SliderInt("RimLight", (int32_t*)&sEffectFlags_.isRimLight, 0, 1);
	ImGui::ColorEdit3("RimColor", sRimColorF3_);
	ImGui::End();

	sEffectFlags_.rimColor = { sRimColorF3_[0],sRimColorF3_[1],sRimColorF3_[2] };



	//画面効果用
	{
		sMapEffectFlagsBuff_->isFog = sEffectFlags_.isFog;
		sMapEffectFlagsBuff_->isToon = sEffectFlags_.isToon;
		sMapEffectFlagsBuff_->isRimLight = sEffectFlags_.isRimLight;
		sMapEffectFlagsBuff_->rimColor = sEffectFlags_.rimColor;
		sMapEffectFlagsBuff_->time = sEffectFlags_.time;
	}
}

void Object::SetCollider(std::unique_ptr<BaseCollider> collider)
{
	collider->SetObject(this);
	collider_ = std::move(collider);
	//コリジョンマネージャーに登録
	CollisionManager::GetInstance()->AddCollider(collider_.get());
	//コライダーを更新
	collider_->Update();
}

void Object::SetIsValid(bool isValid)
{
	if (collider_)
	{
		collider_->SetIsValid(isValid);
	}
}

void Object::SetIs2D(bool is2D)
{
	if (collider_)
	{
		collider_->SetIs2D(is2D);
	}
}


//-------------------------------------------------------------------

Object::Object()
{
	HRESULT result = {};

	Object::Initialize();

	//トランスフォーム行列
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

	if (constBuffSkin_.Get() == nullptr)
	{
		//スキンのバッファ
		//ヒープ設定
		D3D12_RESOURCE_DESC cbResourceDesc{}; D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		ResourceProperties(cbResourceDesc,
			((uint32_t)sizeof(ConstBufferDataSkin) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &constBuffSkin_);
		//マッピング
		ConstBufferDataSkin* constMapSkin = nullptr;
		constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin);
		for (uint32_t i = 0; i < S_MAX_BONES_; i++)
		{
			constMapSkin->bones[i] = XMMatrixIdentity();
		}
		constBuffSkin_->Unmap(0, nullptr);
	}

	//1フレーム分の時間を60fpsで設定
	frameTime_.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
}

void Object::SendingMat(int32_t indexNum)
{

	//変換行列をGPUに送信
	worldMat_->SetWorld();
	//スプライトじゃない場合
	if (indexNum != SPRITE)
	{
		XMMATRIX matW;
		matW = { (float)worldMat_->matWorld_.m_[0][0],(float)worldMat_->matWorld_.m_[0][1],(float)worldMat_->matWorld_.m_[0][2],(float)worldMat_->matWorld_.m_[0][3],
				 (float)worldMat_->matWorld_.m_[1][0],(float)worldMat_->matWorld_.m_[1][1],(float)worldMat_->matWorld_.m_[1][2],(float)worldMat_->matWorld_.m_[1][3],
				 (float)worldMat_->matWorld_.m_[2][0],(float)worldMat_->matWorld_.m_[2][1],(float)worldMat_->matWorld_.m_[2][2],(float)worldMat_->matWorld_.m_[2][3],
				 (float)worldMat_->matWorld_.m_[3][0],(float)worldMat_->matWorld_.m_[3][1],(float)worldMat_->matWorld_.m_[3][2],(float)worldMat_->matWorld_.m_[3][3] };

		cbt_.constMapTransform_->world = matW;
		cbt_.constMapTransform_->viewproj = view_->matView_ * projection_->matProjection_;
		XMFLOAT3 cPos = { view_->eye_.x_,view_->eye_.y_,view_->eye_.z_ };
		cbt_.constMapTransform_->cameraPos = cPos;
	}
}

void Object::PlayAnimationInternal(ModelFBX* model, FbxTime& sTime, FbxTime& eTime,
	bool isLoop, bool isReverse)
{
	//アニメーションが1つしかない前提

	FbxScene* fbxScene = model->GetFbxScene();
	//0番のアニメーション取得
	FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//アニメーションなかったら
	if (animStack == nullptr) { return; }

	//アニメーションの名前取得
	const char* ANIM_STACK_NAME = animStack->GetName();
	//アニメーションの時間情報
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(ANIM_STACK_NAME);

	//開始時間取得
	sTime = takeInfo->mLocalTimeSpan.GetStart();
	//終了時間取得
	eTime = takeInfo->mLocalTimeSpan.GetStop();
	//開始時間取得
	currentTime_ = startTime_;
	//再生中状態
	isPlay_ = true;
	//ループ
	isLoop_ = isLoop;
	//逆再生
	isReverse_ = isReverse;
}

void Object::PlayAnimation(ModelFBX* model, bool isLoop)
{
	PlayAnimationInternal(model, startTime_, endTime_, isLoop);
}

void Object::PlayReverseAnimation(ModelFBX* model, bool isLoop)
{
	PlayAnimationInternal(model, endTime_, startTime_, isLoop, true);
}

void Object::SendingBoneData(ModelFBX* model)
{
	HRESULT result = {};

	//アニメーション
	if (isPlay_)
	{
		//逆再生
		if (isReverse_)
		{
			currentTime_ -= frameTime_;
		}
		else
		{
			//1フレーム進める
			currentTime_ += frameTime_;
		}
		//最後まで再生したら
		if ((!isReverse_ && currentTime_ > endTime_)
			|| (isReverse_ && currentTime_ < endTime_))
		{
			//先頭に戻す
			if (isLoop_)
			{
				currentTime_ = startTime_;
			}
			//終了
			else
			{
				isPlay_ = false;
			}
		}
	}


	//モデルのボーン配列
	std::vector<ModelFBX::Bone>& bones = model->GetBones();

	//定数バッファへデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin);
	for (int32_t i = 0; i < bones.size(); i++)
	{
		//今の姿勢行列
		XMMATRIX matCurrentPose;
		//今の姿勢行列を取得
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime_);
		//xmmatrixに変換
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);

		////メッシュノードのグローバルトランスフォーム
		//model->GetFbxScene()->GetNode()->GetMesh()

		//初期姿勢の逆行列と今の姿勢行列を合成してスキニング行列に
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin_->Unmap(0, nullptr);
}

void Object::SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature)
{
	// パイプラインステートとルートシグネチャの設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(&pipelineState[pipelineNum]);

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature);
}

void Object::SetMaterialLightMTexSkin(uint64_t textureHandle, ConstBuffTransform cbt)
{
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	sLightManager_->Draw(4);

	//04_02
	{
		//SRVヒープの設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().sSrvHeap_.GetAddressOf());
		//SRVヒープの先頭ハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
		srvGpuHandle.ptr = textureHandle;
		//(インスタンスで読み込んだテクスチャ用のSRVを指定)
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	}

	//定数バッファビュー(CBV)の設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, cbt.constBuffTransform_->GetGPUVirtualAddress());

	//演出フラグ
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(5, sEffectFlagsBuff_->GetGPUVirtualAddress());
}

void Object::SetMaterialLightMTexSkinModel(uint64_t textureHandle, ConstBuffTransform cbt, Material* material)
{
	SetMaterialLightMTexSkin(textureHandle, cbt);

	//アンビエントとか
	material->Update();
	ID3D12Resource* constBuff = material->GetConstantBuffer();
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(3, constBuff->GetGPUVirtualAddress());

	//スキニング用
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(6, constBuffSkin_->GetGPUVirtualAddress());

}

void Object::Update(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, const ConstBuffTransform& constBuffTransform,
	Model* model, ModelFBX* fbx, bool primitiveMode)
{
	//行列送信
	SendingMat(indexNum);


	//テクスチャを設定していなかったら
	uint64_t textureHandle_;

	if (textureHandle == NULL)
	{
		textureHandle_ = TextureManager::GetInstance().sWhiteTexHandle_;
	}
	else
	{
		textureHandle_ = textureHandle;
	}

	//ラムダ式でコマンド関数
	std::function<void()>SetRootPipeR = [=]() {SetRootPipe(pipelineState->Get(), pipelineNum, rootSignature.Get()); };
	std::function<void()>SetMaterialTex = [=]() {SetMaterialLightMTexSkin(textureHandle_, constBuffTransform); };

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

		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());


		//04_02
		{
			//SRVヒープの設定コマンド
			DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().sSrvHeap_.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle_;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform_->GetGPUVirtualAddress());

		sprite_->SpriteDraw();
	}
	else if (indexNum == MODEL)
	{
		// パイプラインステートとルートシグネチャの設定コマンド
		SetRootPipe(pipelineSetM.pipelineState.Get(), pipelineNum, pipelineSetM.rootSignature.Get());

		DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

		sLightManager_->Draw(4);

		//定数バッファビュー(CBV)の設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform_->GetGPUVirtualAddress());
		//ポストエフェクトフラグ用
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(5, sEffectFlagsBuff_->GetGPUVirtualAddress());

		//スキニング用
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(6, constBuffSkin_->GetGPUVirtualAddress());

		//モデル用描画
		model->Draw();
	}
	else if (indexNum == FBX)
	{
		SendingBoneData(fbx);

		//ラムダ式でコマンド関数
		std::function<void()>SetRootPipeR = [=]() {SetRootPipe(pipelineSetFBX.pipelineState.Get(), 0, pipelineSetFBX.rootSignature.Get()); };
		std::function<void()>SetMaterialTex = [=]() {SetMaterialLightMTexSkinModel(fbx->material_->textureHandle_, constBuffTransform, fbx->material_.get()); };

		fbx->Draw(SetRootPipeR, SetMaterialTex);
	}
}

void Object::DrawTriangle(/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,*/
	ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color,  uint64_t textureHandle, int32_t pipelineNum)
{
	view_ = view;
	projection_ = projection;

	constMapMaterial_->color = color;

	Update(TRIANGLE, pipelineNum, textureHandle, cbt_);
}

void Object::DrawBox(ViewMat* view, ProjectionMat* projection,/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, */
	const XMFLOAT4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	view_ = view;
	projection_ = projection;

	constMapMaterial_->color = color;

	Update(BOX, pipelineNum, textureHandle, cbt_);
}

void Object::DrawBoxSprite(const Vec3& pos, float scale,
	const XMFLOAT4& color, uint64_t textureHandle, const Vec2& ancorUV, bool isReverseX, bool isReverseY,
	float rotation, int32_t pipelineNum)
{
	if (sprite_.get() == nullptr)
	{
		sprite_ = std::make_unique<Sprite>();
		//スプライトクラスの初期化
		sprite_->Initialize();
	}
	sprite_->Update(pos, scale, color, textureHandle, ancorUV, isReverseX, isReverseY, rotation, &cbt_, constMapMaterial_);

	Update(SPRITE, pipelineNum, textureHandle, cbt_);
}

void Object::Draw()
{
}



void Object::DrawClippingBoxSprite(const Vec3& leftTop, float scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	const XMFLOAT4& color, uint64_t textureHandle, bool isPosLeftTop, bool isReverseX, bool isReverseY,
	float rotation, int32_t pipelineNum)
{
	if (sprite_.get() == nullptr)
	{
		sprite_ = std::make_unique <Sprite>();
		//スプライトクラスの初期化
		sprite_->Initialize();
	}
	sprite_->UpdateClipping(leftTop, scale, UVleftTop, UVlength, color, textureHandle,
		isPosLeftTop, isReverseX, isReverseY, rotation, &cbt_, constMapMaterial_);

	Update(SPRITE, pipelineNum, textureHandle, cbt_);
}

void Object::DrawCube3D(ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	view_ = view;
	projection_ = projection;

	constMapMaterial_->color = color;

	Update(CUBE, pipelineNum, textureHandle, cbt_);
}

void Object::DrawLine(/*const Vec3& pos1, const Vec3& pos2,*/  ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color,
	uint64_t textureHandle)
{
	view_ = view;
	projection_ = projection;

	constMapMaterial_->color = color;

	Update(LINE, 2, textureHandle, cbt_, nullptr, nullptr, false);
}

void Object::DrawCircle(ViewMat* view, ProjectionMat* projection,
	const XMFLOAT4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	view_ = view;
	projection_ = projection;

	constMapMaterial_->color = color;

	Update(CIRCLE, pipelineNum, textureHandle, cbt_);
}

void Object::DrawSphere(ViewMat* view, ProjectionMat* projection,
	const XMFLOAT4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	view_ = view;
	projection_ = projection;

	constMapMaterial_->color = color;

	Update(SPHERE, pipelineNum, textureHandle, cbt_);
}

void Object::DrawModel(ViewMat* view, ProjectionMat* projection,
	Model* model, const XMFLOAT4& color, int32_t pipelineNum)
{
	view_ = view;
	projection_ = projection;

	constMapMaterial_->color = color;

	Update(MODEL, pipelineNum, NULL, cbt_, model);
}

void Object::DrawFBX(ViewMat* view, ProjectionMat* projection, ModelFBX* modelFbx, const XMFLOAT4& color, int32_t pipelineNum)
{
	view_ = view;
	projection_ = projection;

	constMapMaterial_->color = color;

	Update(FBX, pipelineNum, NULL, cbt_, nullptr, modelFbx);
}

void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, int32_t indexNum)
{
	HRESULT result = {};

	if (indexNum == SPRITE)
	{
		// 頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/shaders/SpriteVS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(result));

		// ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/shaders/SpritePS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(result));
	}
	else if (indexNum == MODEL || indexNum == FBX)
	{
		// 頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/shaders/OBJVertexShader.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(result));

		// ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/shaders/OBJPixelShader.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(result));
	}
	else
	{
		// 頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/shaders/BasicVS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(result));

		// ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/shaders/BasicPS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(result));
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
	pipelineDesc.NumRenderTargets = 3; // 描画対象は2つ（ポストエフェクトの一枚目の3つ）
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
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
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSig));
	assert(SUCCEEDED(result));
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

	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelineState));
	//assert(SUCCEEDED(result));
}

void Blend(const D3D12_BLEND_OP& blendMode, bool Inversion, bool Translucent)
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

	//ポストエフェクトの一枚目の二つ目用に
	pipelineDesc.BlendState.RenderTarget[1] = blendDesc;
}

void Object::constBuffTransfer(const XMFLOAT4& plusRGBA)
{
	if (constMapMaterial_->color.x <= 1.0f && constMapMaterial_->color.x >= 0.0f)
		constMapMaterial_->color.x += plusRGBA.x;
	if (constMapMaterial_->color.y <= 1.0f && constMapMaterial_->color.y >= 0.0f)
		constMapMaterial_->color.y += plusRGBA.y;
	if (constMapMaterial_->color.z <= 1.0f && constMapMaterial_->color.z >= 0.0f)
		constMapMaterial_->color.z += plusRGBA.z;
	if (constMapMaterial_->color.w <= 1.0f && constMapMaterial_->color.w >= 0.0f)
		constMapMaterial_->color.w += plusRGBA.w;
}



void SetNormDigitalMat(XMMATRIX& mat)
{
	mat.r[0].m128_f32[0] = 2.0f / WindowsApp::GetInstance().WINDOW_WIDTH_;
	mat.r[1].m128_f32[1] = -2.0f / WindowsApp::GetInstance().WINDOW_HEIGHT_;
	mat.r[3].m128_f32[0] = -1.0f;
	mat.r[3].m128_f32[1] = 1.0f;
}

void Error(bool filed)
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
