#include "Object.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "ImguiManager.h"
#include "CameraManager.h"

using namespace DirectX;


//図形用
Primitive Object::primitive_;

RootPipe Object::objPipeLineSet_[3];
RootPipe Object::pipelineSet_;
//al4_02_02
RootPipe Object::pipelineSetM_;

//FBX用
RootPipe Object::pipelineSetFBX_;

//ルートパラメータの設定
D3D12_ROOT_PARAMETER Object::rootParams_[9] = {};

// グラフィックスパイプライン設定
D3D12_GRAPHICS_PIPELINE_STATE_DESC Object::pipelineDesc_{};

//static
LightManager* Object::sLightManager_ = nullptr;

//演出用
//ComPtr <ID3D12Resource> Object::effectFlagsBuff_ = nullptr;
//EffectOConstBuffer* Object::sMapEffectFlagsBuff_ = nullptr;
//EffectOConstBuffer Object::sEffectFlags_;
float Object::sRimColorF3_[3] = { 1.0f,1.0f,1.0f };


void Object::DrawInitialize()
{
	HRESULT result = {};

	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	//テクスチャ用のデスクリプタヒープ初期化
	TextureManager::GetInstance().InitializeDescriptorHeap();

	//図形クラスの
	primitive_.Initialize();

	//ルートパラメータの設定
	//定数バッファ0番（色）
	rootParams_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[0].Descriptor.ShaderRegister = 0;//定数バッファ番号(b0)
	rootParams_[0].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ0番（テクスチャ）
	rootParams_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	rootParams_[1].DescriptorTable.pDescriptorRanges = &TextureManager::GetInstance().sDescriptorRange_;//デスクリプタレンジ
	rootParams_[1].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ1番(行列)
	rootParams_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[2].Descriptor.ShaderRegister = 1;//定数バッファ番号(b1)
	rootParams_[2].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ2番（マテリアル）
	rootParams_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[3].Descriptor.ShaderRegister = 2;//定数バッファ番号(b2)
	rootParams_[3].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ3番(ライト用)
	rootParams_[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[4].Descriptor.ShaderRegister = 3;//定数バッファ番号(b3)
	rootParams_[4].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ4番(画面効果フラグ)
	rootParams_[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[5].Descriptor.ShaderRegister = 4;//定数バッファ番号(b4)
	rootParams_[5].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ5番（スキニング用）
	rootParams_[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams_[6].Descriptor.ShaderRegister = 5;//定数バッファ番号(b5)
	rootParams_[6].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams_[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ1番（ディゾルブ用テクスチャ）
	rootParams_[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	D3D12_DESCRIPTOR_RANGE dissolveDHRange = TextureManager::GetInstance().sDescriptorRange_;
	dissolveDHRange.BaseShaderRegister++;
	rootParams_[7].DescriptorTable.pDescriptorRanges = &dissolveDHRange;//デスクリプタレンジ
	rootParams_[7].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ2番（スペキュラマップ用テクスチャ）
	rootParams_[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	D3D12_DESCRIPTOR_RANGE dissolveDHRange2 = TextureManager::GetInstance().sDescriptorRange_;
	dissolveDHRange2.BaseShaderRegister += 2;
	rootParams_[8].DescriptorTable.pDescriptorRanges = &dissolveDHRange2;//デスクリプタレンジ
	rootParams_[8].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams_[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	// パイプランステートの生成
	PipeLineState(D3D12_FILL_MODE_SOLID, objPipeLineSet_[0]);
	//ワイヤーフレーム用
	PipeLineState(D3D12_FILL_MODE_WIREFRAME, objPipeLineSet_[1]);

	//line
	PipeLineState(D3D12_FILL_MODE_WIREFRAME, objPipeLineSet_[2], LINE);

	//sprite用
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSet_, SPRITE);

	//model用
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSetM_, OBJ);

	//FBX用
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSetFBX_, FBX);
}

//----------------------------------------------------------------
bool Object::Initialize(std::unique_ptr<WorldMat> worldMat)
{
	if (worldMat == nullptr)
	{
		worldMat = std::move(worldMat_);
	}

	SetWorldMat(std::move(worldMat));

	objName_ = typeid(*this).name();
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

const Vec3& Object::GetFrontVec()
{
	return frontVec_;
}

void Object::CulcFrontVec()
{
	frontVec_ = GetTurnVec3UseQuaternionAndRot(frontVecTmp_, GetRot());
}

void Object::Update()
{
	EffectUpdate();

	WorldMatColliderUpdate();
}

void Object::EffectUpdate()
{
	effectFlags_.time++;

#ifdef _DEBUG
	//imgui
	/*ImGui::Begin("ObjectEffect");
	ImGui::SliderInt("Fog", (int32_t*)&sEffectFlags_.isFog, 0, 1);
	ImGui::SliderInt("Toon", (int32_t*)&sEffectFlags_.isToon, 0, 1);
	ImGui::SliderInt("RimLight", (int32_t*)&sEffectFlags_.isRimLight, 0, 1);
	ImGui::ColorEdit3("RimColor", sRimColorF3_);
	ImGui::SliderInt("Silhouette", (int32_t*)&sEffectFlags_.isSilhouette, 0, 1);
	ImGui::End();*/
#endif // DEBUG

	effectFlags_.rimColor = { sRimColorF3_[0],sRimColorF3_[1],sRimColorF3_[2],0 };

	//画面効果用
	{
		mapEffectFlagsBuff_->isFog = effectFlags_.isFog;
		mapEffectFlagsBuff_->isToon = effectFlags_.isToon;
		mapEffectFlagsBuff_->isRimLight = effectFlags_.isRimLight;
		mapEffectFlagsBuff_->rimColor = effectFlags_.rimColor;
		mapEffectFlagsBuff_->isSilhouette = effectFlags_.isSilhouette;
		mapEffectFlagsBuff_->isDissolve = effectFlags_.isDissolve;
		mapEffectFlagsBuff_->dissolveT = effectFlags_.dissolveT;
		mapEffectFlagsBuff_->isSpecularMap = effectFlags_.isSpecularMap;
		mapEffectFlagsBuff_->time = effectFlags_.time;
	}
}

void Object::WorldMatColliderUpdate()
{
	//行列更新（ワールド座標系にして当たり判定を行う）
	worldMat_->CulcAllTreeMat();
	//当たり判定更新
	if (collider_.get())
	{
		collider_->Update();
	}
}

void Object::StaticUpdate()
{
}

void Object::SetCollider(std::unique_ptr<BaseCollider> collider)
{
	collider->SetObject(this);
	collider_ = std::move(collider);
	//コリジョンマネージャーに登録
	CollisionManager::GetInstance()->AddCollider(collider_.get());
	//行列,コライダーの更新
	Object::WorldMatColliderUpdate();
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
		BuffProperties(cbHeapProp, cbResourceDesc, &effectFlagsBuff_);
		//定数バッファのマッピング
		result = effectFlagsBuff_->Map(0, nullptr, (void**)&mapEffectFlagsBuff_);//マッピング
		assert(SUCCEEDED(result));
	}
}

void Object::SendingMat(int32_t indexNum, Camera* camera)
{

	if (objName_ == "gun")
	{
		//変換行列をGPUに送信
		worldMat_->CulcAllTreeMat();
	}

	//変換行列をGPUに送信
	worldMat_->CulcAllTreeMat();
	//スプライトじゃない場合
	if (indexNum != SPRITE)
	{
		Camera* lCamera = camera;
		//カメラがセットされてなければ使用されてるカメラを使う
		if (camera == nullptr)
		{
			lCamera = CameraManager::GetInstance().usingCamera_;
		}

		XMMATRIX matW;
		worldMat_->matWorld_.MatIntoXMMATRIX(matW);

		cbt_.constMapTransform_->world = matW;
		cbt_.constMapTransform_->viewproj = lCamera->viewMat_.matView_ * lCamera->projectionMat_.matProjection_;
		XMFLOAT3 cPos = { lCamera->viewMat_.eye_.x_,lCamera->viewMat_.eye_.y_,lCamera->viewMat_.eye_.z_ };
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

void Object::SetMaterialLightMTexSkin(uint64_t textureHandle, uint64_t dissolveTex, uint64_t specularMapTex, ConstBuffTransform cbt)
{
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	sLightManager_->Draw(4);

	//テクスチャ
		//SRVヒープの設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().sSrvHeap_.GetAddressOf());
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	srvGpuHandle.ptr = textureHandle;
	//(インスタンスで読み込んだテクスチャ用のSRVを指定)
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//ディゾルブテクスチャ
	srvGpuHandle.ptr = dissolveTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(7, srvGpuHandle);

	//スペキュラマップテクスチャ
	srvGpuHandle.ptr = specularMapTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(8, srvGpuHandle);

	//定数バッファビュー(CBV)の設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, cbt.constBuffTransform_->GetGPUVirtualAddress());

	//演出フラグ
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(5, effectFlagsBuff_->GetGPUVirtualAddress());
}

void Object::SetMaterialLightMTexSkinModel(uint64_t textureHandle, uint64_t dissolveTexHandle, uint64_t specularMapTexhandle, ConstBuffTransform cbt, Material* material)
{
	SetMaterialLightMTexSkin(textureHandle, dissolveTexHandle, specularMapTexhandle, cbt);

	//アンビエントとか
	material->Update();
	ID3D12Resource* constBuff = material->GetConstantBuffer();
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(3, constBuff->GetGPUVirtualAddress());

	//スキニング用
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(6, constBuffSkin_->GetGPUVirtualAddress());

}

void Object::Update(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, const ConstBuffTransform& constBuffTransform,
	Camera* camera, Model* model, ModelFBX* fbx, bool primitiveMode)
{
	//行列送信
	SendingMat(indexNum, camera);

	//テクスチャを設定していなかったら
	uint64_t textureHandleL = textureHandle;
	TextureManager::CheckTexHandle(textureHandleL);
	uint64_t dissolveTextureHandleL = dissolveTextureHandle_;
	TextureManager::CheckTexHandle(dissolveTextureHandleL);
	uint64_t specularMapTextureHandleL = specularMapTextureHandle_;
	TextureManager::CheckTexHandle(specularMapTextureHandleL);

	//ラムダ式でコマンド関数
	std::function<void()>SetRootPipeR = [=]() {SetRootPipe(objPipeLineSet_->pipelineState.Get(), pipelineNum, objPipeLineSet_->rootSignature.Get()); };
	std::function<void()>SetMaterialTex = [=]() {SetMaterialLightMTexSkin(textureHandleL, dissolveTextureHandleL, specularMapTextureHandleL, constBuffTransform); };

	if (indexNum == TRIANGLE)
	{
		primitive_.TriangleDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == BOX)
	{
		primitive_.BoxDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == CUBE)
	{
		primitive_.CubeDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == LINE)
	{
		primitive_.LineDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == CIRCLE)
	{
		primitive_.CircleDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == SPHERE)
	{
		primitive_.SphereDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == SPRITE)
	{
		SpriteCommonBeginDraw(&pipelineSet_);

		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

		//テクスチャ
			//SRVヒープの設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().sSrvHeap_.GetAddressOf());
		//SRVヒープの先頭ハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
		srvGpuHandle.ptr = textureHandleL;
		//(インスタンスで読み込んだテクスチャ用のSRVを指定)
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		//定数バッファビュー(CBV)の設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform_->GetGPUVirtualAddress());

		sprite_->SpriteDraw();
	}
	else if (indexNum == OBJ)
	{
		// パイプラインステートとルートシグネチャの設定コマンド
		SetRootPipe(pipelineSetM_.pipelineState.Get(), pipelineNum, pipelineSetM_.rootSignature.Get());

		DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

		sLightManager_->Draw(4);

		//ディゾルブテクスチャ
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
		srvGpuHandle.ptr = dissolveTextureHandleL;
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(7, srvGpuHandle);

		//スペキュラマップテクスチャ
		srvGpuHandle.ptr = specularMapTextureHandleL;
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(8, srvGpuHandle);

		//定数バッファビュー(CBV)の設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform_->GetGPUVirtualAddress());
		//エフェクトフラグ用
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(5, effectFlagsBuff_->GetGPUVirtualAddress());

		//スキニング用
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(6, constBuffSkin_->GetGPUVirtualAddress());

		//モデル用描画
		model->Draw();
	}
	else if (indexNum == FBX)
	{
		SendingBoneData(fbx);

		//ラムダ式でコマンド関数
		std::function<void()>SetRootPipeR = [=]() {SetRootPipe(pipelineSetFBX_.pipelineState.Get(), 0, pipelineSetFBX_.rootSignature.Get()); };
		std::function<void()>SetMaterialTex = [=]() {SetMaterialLightMTexSkinModel(fbx->material_->textureHandle_, dissolveTextureHandleL, specularMapTextureHandleL,
			constBuffTransform, fbx->material_.get()); };

		fbx->Draw(SetRootPipeR, SetMaterialTex);
	}
}

void Object::DrawTriangle(/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,*/
	Camera* camera, const XMFLOAT4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(TRIANGLE, pipelineNum, textureHandle, cbt_, camera);
}

void Object::DrawBox(Camera* camera,/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, */
	const XMFLOAT4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(BOX, pipelineNum, textureHandle, cbt_, camera);
}

void Object::DrawBoxSprite(const Vec2& pos, float scale,
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

	Update(SPRITE, pipelineNum, textureHandle, cbt_, nullptr);
}

void Object::Draw()
{
}



void Object::DrawClippingBoxSprite(const Vec2& leftTop, float scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
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

	Update(SPRITE, pipelineNum, textureHandle, cbt_, nullptr);
}

void Object::DrawCube3D(Camera* camera, const XMFLOAT4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(CUBE, pipelineNum, textureHandle, cbt_, camera);
}

void Object::DrawLine(/*const Vec3& pos1, const Vec3& pos2,*/  Camera* camera, const XMFLOAT4& color,
	uint64_t textureHandle)
{
	constMapMaterial_->color = color;

	Update(LINE, 2, textureHandle, cbt_, camera, nullptr, nullptr, false);
}

void Object::DrawCircle(Camera* camera,
	const XMFLOAT4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(CIRCLE, pipelineNum, textureHandle, cbt_, camera);
}

void Object::DrawSphere(Camera* camera,
	const XMFLOAT4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(SPHERE, pipelineNum, textureHandle, cbt_, camera);
}

void Object::DrawModel(Model* model, Camera* camera,
	const XMFLOAT4& color, int32_t pipelineNum)
{
	if (model == nullptr)
	{
		if (model_)
		{
			model = model_;
		}
		else
		{
			assert(true);
		}
	}

	constMapMaterial_->color = color;

	Update(OBJ, pipelineNum, NULL, cbt_, camera, model);
}

void Object::DrawFBX(ModelFBX* modelFbx, Camera* camera, const XMFLOAT4& color, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(FBX, pipelineNum, NULL, cbt_, camera, nullptr, modelFbx);
}

void Object::PipeLineState(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe, int32_t indexNum)
{
	HRESULT result = {};

	if (indexNum == SPRITE)
	{
		rootPipe.CreateBlob(L"Resources/shaders/SpriteVS.hlsl", L"Resources/shaders/SpritePS.hlsl");
	}
	else if (indexNum == OBJ || indexNum == FBX)
	{
		rootPipe.CreateBlob(L"Resources/shaders/OBJVertexShader.hlsl", L"Resources/shaders/OBJPixelShader.hlsl");
	}
	else
	{
		rootPipe.CreateBlob(L"Resources/shaders/BasicVS.hlsl", L"Resources/shaders/BasicPS.hlsl");
	}

	// シェーダーの設定
	pipelineDesc_.VS.pShaderBytecode = rootPipe.vsBlob->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = rootPipe.vsBlob->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = rootPipe.psBlob->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = rootPipe.psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	if (indexNum == SPRITE)
	{
		pipelineDesc_.RasterizerState = D3D12_RASTERIZER_DESC();
		pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}
	else
		pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面カリング

	pipelineDesc_.RasterizerState.FillMode = fillMode; // ポリゴン内塗りつぶし
	pipelineDesc_.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	Blend(D3D12_BLEND_OP_ADD, false, true);

	// 頂点レイアウトの設定
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
	if (indexNum == SPRITE)
	{
		pipelineDesc_.InputLayout.pInputElementDescs = inputLayoutSprite;
		pipelineDesc_.InputLayout.NumElements = _countof(inputLayoutSprite);
	}
	else
	{
		pipelineDesc_.InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc_.InputLayout.NumElements = _countof(inputLayout);
	}

	// 図形の形状設定
	if (indexNum == LINE)
	{
		pipelineDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	}
	else
	{
		pipelineDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}

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
	pipelineDesc_.DepthStencilState.DepthEnable = true;//深度テストを行う
	pipelineDesc_.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	if (indexNum == SPRITE)
	{
		pipelineDesc_.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//小さければ合格
	}
	else
	{
		pipelineDesc_.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さければ合格
	}
	pipelineDesc_.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_, IID_PPV_ARGS(rootPipe.pipelineState.GetAddressOf()));
	//assert(SUCCEEDED(result));
}

void Object::Blend(const D3D12_BLEND_OP& blendMode, bool Inversion, bool Translucent)
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
