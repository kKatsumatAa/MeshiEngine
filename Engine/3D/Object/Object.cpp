#include "Object.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "ImguiManager.h"
#include "CameraManager.h"
#include "ObjectManager.h"

using namespace DirectX;


//図形用
Primitive Object::primitive_;

RootPipe Object::objPipeLineSet_[3];
RootPipe Object::spritePipelineSet_;
//al4_02_02
RootPipe Object::pipelineSetM_;

//FBX用
RootPipe Object::pipelineSetFBX_;

//ルートパラメータの設定
D3D12_ROOT_PARAMETER Object::rootParams_[11] = {};

// グラフィックスパイプライン設定
D3D12_GRAPHICS_PIPELINE_STATE_DESC Object::pipelineDesc_{};

//static
LightManager* Object::sLightManager_ = nullptr;


void Object::DrawInitialize()
{
	HRESULT result = {};

	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	//図形クラスの
	primitive_.Initialize();

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

	// パイプランステートの生成
	PipeLineState(D3D12_FILL_MODE_SOLID, objPipeLineSet_[0]);
	//ワイヤーフレーム用
	PipeLineState(D3D12_FILL_MODE_WIREFRAME, objPipeLineSet_[1]);

	//line
	PipeLineState(D3D12_FILL_MODE_WIREFRAME, objPipeLineSet_[2], LINE);

	//sprite用
	PipeLineState(D3D12_FILL_MODE_SOLID, spritePipelineSet_, SPRITE);

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

	return true;
}

void Object::SetIsAlive(bool isAlive)
{
	isAlive_ = isAlive;
	//当たり判定の方も無効
	if (collider_)
	{
		collider_->SetIsValid(false);
	}
}

void Object::SetIsValid(bool isValid)
{
	isValid_ = isValid;
	SetColliderIsValid(isValid);

	if (isValid == true)
	{
		worldMat_->CalcAllTreeMat();
	}
}

const Vec3& Object::GetFrontVec()
{
	return frontVec_;
}

void Object::CalcFrontVec()
{
	frontVec_ = GetTurnVec3UseQuaternionAndRot(frontVecTmp_, GetRot());
}

void Object::Update()
{
	EffectUpdate();

	WorldMatColliderUpdate();
}

void Object::Draw()
{
	DrawModel(nullptr);
}

void Object::EffectUpdate()
{
	effectFlags_.time++;

	//画面効果用
	{
		*mapEffectFlagsBuff_ = effectFlags_;
	}
}

void Object::WorldMatColliderUpdate()
{
	//行列更新（ワールド座標系にして当たり判定を行う）
	worldMat_->CalcAllTreeMat();
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

void Object::SetColliderIsValid(bool isValid)
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
Object::~Object()
{
	constBuffMaterial_.Reset();
	constBuffSkin_.Reset();

	//object毎に消えるのでいらないかも
	if (collider_.get())
	{
		CollisionManager::GetInstance()->RemoveCollider(collider_.get());
	}
}

Object::Object()
{
	HRESULT result = {};

	Object::Initialize();

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

	//演出用
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		ResourceProperties(cbResourceDesc,
			((uint32_t)sizeof(EffectOConstBuffer) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &effectFlagsBuff_);
		//定数バッファのマッピング
		result = effectFlagsBuff_->Map(0, nullptr, (void**)&mapEffectFlagsBuff_);//マッピング
		assert(SUCCEEDED(result));
	}

	//トランスフォーム行列
	cbt_.Initialize();
}

void Object::SendingMat(int32_t indexNum, Camera* camera, IModel* model)
{
	//スプライトじゃない場合
	if (indexNum != SPRITE)
	{
		//大きすぎたりするのを防ぐ用に
		Vec3 scale = worldMat_->scale_;

		if (model)
		{
			worldMat_->scale_ = worldMat_->scale_ * model->GetScaleExtend();
		}

		XMMATRIX matW;

		//変換行列をGPUに送信
		if (parentNode_ && worldMat_->parent_)
		{
			//ノードのローカル行列
			//M4 m;
			//m.PutInXMMATRIX(parentNode_->globalTransform);
			//m.m_[3][0] *= parentNodeModel_->GetScaleExtend();
			//m.m_[3][1] *= parentNodeModel_->GetScaleExtend();
			//m.m_[3][2] *= parentNodeModel_->GetScaleExtend();
			//オブジェクトのワールド
			worldMat_->CalcWorldMat();
			//親行列
			M4 pM = worldMat_->GetOnlyParentALLTreeMat();
			//ボーン(ボーンのグローバルトランスフォーム、)
			M4 bM;
			{
				//マッピング
				parentObj_->MappingBoneData(parentNodeModel_);
				//親ノードと同じ名前のボーンのインデックスを得る
				int32_t bIndex = parentNodeModel_->GetBoneIndex(parentNode_->name);
				//親ノードがあるモデルのボーンをインデックスで指定して行列の計算
				XMMATRIX bXM = parentObj_->GetCalcSkinMat(parentNodeModel_->GetBones(), bIndex);
				bM.PutInXMMATRIX(bXM);
				bM.m_[3][0] *= parentNodeModel_->GetScaleExtend();
				bM.m_[3][1] *= parentNodeModel_->GetScaleExtend();
				bM.m_[3][2] *= parentNodeModel_->GetScaleExtend();
			}
			(worldMat_->matWorld_ * (bM * pM)).MatIntoXMMATRIX(matW);
			worldMat_->matWorld_.PutInXMMATRIX(matW);
		}
		else
		{
			worldMat_->CalcAllTreeMat();
			worldMat_->matWorld_.MatIntoXMMATRIX(matW);
		}
		worldMat_->scale_ = scale;

		cbt_.SetWorldMat(matW);
		cbt_.SetViewProjMat(camera->GetViewMat() * camera->GetProjMat());
		cbt_.SetCameraPos(camera->GetEye());
	}
}

void Object::SendingMat(int32_t indexNum, Camera* camera, const XMMATRIX* mat)
{
}

void Object::PlayAnimationInternal(FbxTime& sTime, FbxTime& eTime,
	bool isLoop, bool isReverse)
{
	//アニメーションのリセット
	AnimationReset(sTime, eTime);

	//再生中状態
	isPlay_ = true;
	//ループ
	isLoop_ = isLoop;
	//逆再生
	isReverse_ = isReverse;
}

void Object::AnimationReset(FbxTime& sTime, FbxTime& eTime)
{
	if (model_ == nullptr || !model_->GetIsFbx())
	{
		return;
	}

	//子クラスに変換
	ModelFBX* model = dynamic_cast<ModelFBX*>(model_);

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
}

void Object::PlayAnimation(bool isLoop)
{
	PlayAnimationInternal(startTime_, endTime_, isLoop);
}

void Object::PlayReverseAnimation(bool isLoop)
{
	PlayAnimationInternal(endTime_, startTime_, isLoop, true);
}

void Object::CalcBoneDataInternal(ModelFBX* model)
{
	HRESULT result = {};

	//アニメーション
	if (isPlay_)
	{
		//最後まで再生したら
		if ((!isReverse_ && currentTime_ >= endTime_)
			|| (isReverse_ && currentTime_ <= startTime_))
		{
			if (!isReverse_)
			{
				//先頭に戻す
				currentTime_ = startTime_;
			}
			else
			{
				//最後に戻す
				currentTime_ = endTime_;
			}

			//終了
			if (!isLoop_)
			{
				isPlay_ = false;
			}
		}

		//アニメーションスピードをかけて
		frameTime_.SetTime(0, 0, 0, max((int)((1000.0f / 60.0f) * animationSpeed_), 1), 0, FbxTime::EMode::eFrames1000);

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
	}

	//マッピング
	MappingBoneData(model);
}

XMMATRIX Object::GetCalcSkinMat(const std::vector<ModelFBX::Bone>& bones, int32_t index)
{
	//今の姿勢行列
	XMMATRIX matCurrentPose;
	//今の姿勢行列を取得
	FbxAMatrix fbxCurrentPose =
		bones[index].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime_);
	//xmmatrixに変換
	FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);

	//初期姿勢の逆行列と今の姿勢行列を合成してスキニング行列に
	XMMATRIX globalInv = XMMatrixInverse(nullptr, bones[index].globalTransform);
	XMMATRIX ansM = bones[index].globalTransform * bones[index].invInitialPose * matCurrentPose * globalInv;

	return ansM;
}

void Object::MappingBoneData(ModelFBX* model)
{
	//モデルのボーン配列
	std::vector<ModelFBX::Bone>& bones = model->GetBones();

	//定数バッファへデータ転送
	HRESULT result = constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin);
	for (int32_t i = 0; i < bones.size(); i++)
	{
		//行列計算
		constMapSkin->bones[i] = GetCalcSkinMat(bones, i);
	}
	constBuffSkin_->Unmap(0, nullptr);
}

void Object::SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature)
{
	// パイプラインステートとルートシグネチャの設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(&pipelineState[pipelineNum]);

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature);
}

void Object::SetMaterialLightMTexSkin(uint64_t textureHandle, uint64_t dissolveTex, uint64_t specularMapTex,
	uint64_t normalMapTex, bool setTex)
{
	//テクスチャ
	//SRVヒープの設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetDescHeapPP());

	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(COLOR, constBuffMaterial_->GetGPUVirtualAddress());

	//モデルはメッシュごとに行列等セットするので
	if (setTex)
	{
		srvGpuHandle.ptr = textureHandle;
		//(インスタンスで読み込んだテクスチャ用のSRVを指定)
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(TEX, srvGpuHandle);
	}
	//行列
	cbt_.DrawCommand(MATRIX);

	//ライト
	sLightManager_->Draw(LIGHT);

	//ディゾルブテクスチャ
	srvGpuHandle.ptr = dissolveTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(DISSOLVE, srvGpuHandle);

	//スペキュラマップテクスチャ
	srvGpuHandle.ptr = specularMapTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(SPECULAR_MAP, srvGpuHandle);

	//ノーマルマップテクスチャ
	srvGpuHandle.ptr = normalMapTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(NORM_MAP, srvGpuHandle);

	//演出フラグ
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(EFFECT, effectFlagsBuff_->GetGPUVirtualAddress());
}

void Object::SetMaterialLightMTexSkinModel(uint64_t dissolveTexHandle, uint64_t specularMapTexhandle,
	uint64_t normalMapTexHandle)
{
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SetMaterialLightMTexSkin(NULL, dissolveTexHandle, specularMapTexhandle, normalMapTexHandle, false);

	//スキニング用
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(SKIN, constBuffSkin_->GetGPUVirtualAddress());

}

void Object::Update(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, ConstBuffTransform* constBuffTransform,
	Camera* camera, IModel* model, bool primitiveMode)
{
	Camera* lCamera = camera;
	//カメラがセットされてなければ使用されてるカメラを使う
	if (camera == nullptr)
	{
		lCamera = CameraManager::GetInstance().usingCamera_;
	}

	//行列送信
	SendingMat(indexNum, lCamera, model);

	//テクスチャを設定していなかったら
	uint64_t textureHandleL = textureHandle;
	TextureManager::CheckTexHandle(textureHandleL);
	uint64_t dissolveTextureHandleL = dissolveTextureHandle_;
	TextureManager::CheckTexHandle(dissolveTextureHandleL);
	uint64_t specularMapTextureHandleL = specularMapTextureHandle_;
	TextureManager::CheckTexHandle(specularMapTextureHandleL);
	uint64_t normalMapTextureHandleL = normalMapTextureHandle_;
	TextureManager::CheckTexHandle(normalMapTextureHandleL);

	//ラムダ式でコマンド関数
	std::function<void()>SetRootPipeR = [=]() {SetRootPipe(objPipeLineSet_->pipelineState.Get(), pipelineNum, objPipeLineSet_->rootSignature.Get()); };
	std::function<void()>SetMaterialTex = [=]() {SetMaterialLightMTexSkin(textureHandleL, dissolveTextureHandleL, specularMapTextureHandleL,
		normalMapTextureHandleL, constBuffTransform); };


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
		SpriteCommonBeginDraw(&spritePipelineSet_);

		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(COLOR, constBuffMaterial_->GetGPUVirtualAddress());

		//SRVヒープの先頭ハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
		srvGpuHandle.ptr = textureHandleL;
		//テクスチャ
	//SRVヒープの設定コマンド
		DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetDescHeapPP());
		//(インスタンスで読み込んだテクスチャ用のSRVを指定)
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(TEX, srvGpuHandle);
		//行列
		constBuffTransform->DrawCommand(MATRIX);


		sprite_->SpriteDraw();
	}
	else if (indexNum == OBJ || indexNum == FBX)
	{
		//モデル用
		//ラムダ式でコマンド関数
		std::function<void()>SetRootPipeRM = [=]() {SetRootPipe(pipelineSetFBX_.pipelineState.Get(), pipelineNum, pipelineSetFBX_.rootSignature.Get()); };
		std::function<void()>SetMaterialTexM = [=]() {SetMaterialLightMTexSkinModel(dissolveTextureHandleL, specularMapTextureHandleL,
			normalMapTextureHandleL); };

		//メッシュのオフセットデータセット
		GetModel()->SetPolygonOffsetData(meshOffsetData_);

		if (indexNum == FBX)
		{
			CalcBoneDataInternal(dynamic_cast<ModelFBX*>(model));
		}
		model->Draw(SetRootPipeRM, SetMaterialTexM, cbt_);
	}

}

void Object::ParentFbxNode(Object* obj, IModel* model, const std::string& nodeName)
{
	ModelFBX* modelL = dynamic_cast<ModelFBX*>(model);

	if (model->GetIsFbx() && modelL->GetNode(nodeName))
	{
		parentNode_ = modelL->GetNode(nodeName);
		parentNodeModel_ = modelL;
		parentObj_ = obj;
		return;
	}

	assert(false);
}

void Object::DrawTriangle(Camera* camera, const Vec4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(TRIANGLE, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawBox(Camera* camera, const Vec4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(BOX, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawBoxSprite(const Vec2& pos, const Vec2& scale,
	const Vec4& color, uint64_t textureHandle, const Vec2& ancorUV, bool isReverseX, bool isReverseY,
	float rotation, int32_t pipelineNum)
{
	if (sprite_.get() == nullptr)
	{
		sprite_ = std::make_unique<Sprite>();
		//スプライトクラスの初期化
		sprite_->Initialize();
	}
	sprite_->Update(pos, scale, color, textureHandle, ancorUV, isReverseX, isReverseY, rotation, &cbt_, constMapMaterial_);

	Update(SPRITE, pipelineNum, textureHandle, &cbt_, nullptr);
}

void Object::DrawClippingBoxSprite(const Vec2& leftTop, const Vec2& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	const Vec4& color, uint64_t textureHandle, bool isPosLeftTop, bool isReverseX, bool isReverseY,
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

	Update(SPRITE, pipelineNum, textureHandle, &cbt_, nullptr);
}

void Object::DrawCube3D(Camera* camera, const Vec4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(CUBE, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawLine(Camera* camera, const Vec4& color,
	uint64_t textureHandle)
{
	constMapMaterial_->color = color;

	Update(LINE, 2, textureHandle, &cbt_, camera, nullptr, false);
}

void Object::DrawCircle(Camera* camera,
	const Vec4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(CIRCLE, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawSphere(Camera* camera,
	const Vec4& color, uint64_t textureHandle, int32_t pipelineNum)
{
	constMapMaterial_->color = color;

	Update(SPHERE, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawModel(IModel* model, Camera* camera,
	const Vec4& color, int32_t pipelineNum)
{
	if (model == nullptr)
	{
		if (model_)
		{
			model = model_;
		}
		else
		{
			assert(false);
		}
	}

	constMapMaterial_->color = color;

	objType type = OBJ;
	if (model->GetIsFbx())
	{
		type = FBX;
	}

	Update(type, pipelineNum, NULL, &cbt_, camera, model);
}

void Object::DrawImGui(std::function<void()>imguiF)
{
	ImGui::Begin(objName_.c_str());

	//生死フラグ
	ImGui::Checkbox("isAlive: ", &isAlive_);
	if (isAlive_ == false && collider_)
	{
		collider_->SetIsValid(false);
	}

	//トランスなど
	if (ImGui::TreeNode("TransScaleRot")) {

		ImGui::SliderFloat3("Trans: ", &worldMat_->trans_.x_, -100.0f, 100.0f);
		ImGui::SliderFloat3("Scale: ", &worldMat_->scale_.x_, 0, 10.0f);
		ImGui::SliderFloat3("Rot: ", &worldMat_->rot_.x_, 0, PI * 2.0f);

		ImGui::TreePop();
	}

	//正面ベクトル
	if (ImGui::TreeNode("FrontVec")) {

		ImGui::Text("FrontVec: %.2f %.2f %.2f", frontVec_.x_, frontVec_.y_, frontVec_.z_);
		ImGui::Text("FrontVecTmp: %.2f %.2f %.2f", frontVecTmp_.x_, frontVecTmp_.y_, frontVecTmp_.z_);

		ImGui::TreePop();
	}

	//モデル
	if (ImGui::TreeNode("Model")) {

		std::string modelPState = "MODEL_NULL";
		if (model_ != nullptr)
		{
			modelPState = "MODEL_SET";
		}
		ImGui::Text(modelPState.c_str());
		if (model_ != nullptr)
		{
			ImGui::Text("modelIsFbx: %d", model_->GetIsFbx());
		}
		ImGui::Checkbox("isPlay", &isPlay_);
		ImGui::Checkbox("isLoop", &isLoop_);
		ImGui::Checkbox("isReverse", &isReverse_);
		if (currentTime_ - startTime_ > 0 && endTime_ - startTime_ > 0)
		{
			float timer = (float)(currentTime_ - startTime_).GetSecondDouble();
			float timerMax = (float)(endTime_ - startTime_).GetSecondDouble();

			ImGui::Text("animationTimeRatio: %.2f", timer / timerMax);
		}
		if (model_)
		{
			model_->DrawImGui();
		}
		ImGui::TreePop();
	}

	//演出
	if (ImGui::TreeNode("Effect")) {

		ImGui::Checkbox("isDissolve", (bool*)&effectFlags_.isDissolve);
		ImGui::SliderFloat("dissolveT", &effectFlags_.dissolveT, 0, 1.0f);
		ImGui::Checkbox("isFog", (bool*)&effectFlags_.isFog);
		ImGui::Checkbox("isNormalMap", (bool*)&effectFlags_.isNormalMap);
		ImGui::Checkbox("isRimLight", (bool*)&effectFlags_.isRimLight);
		ImGui::SliderFloat4("rimLightColor", &effectFlags_.rimColor.x, 0, 1.0f);
		ImGui::Checkbox("isSilhouette", (bool*)&effectFlags_.isSilhouette);
		ImGui::Checkbox("isSpecularMap", (bool*)&effectFlags_.isSpecularMap);

		ImGui::TreePop();
	}

	//派生クラスごとの
	if (imguiF)
	{
		imguiF();
	}

	ImGui::End();
}

void Object::PipeLineState(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe, int32_t indexNum)
{
	HRESULT result = {};

	if (indexNum == SPRITE)
	{
		rootPipe.CreateBlob("Resources/shaders/SpriteVS.hlsl", "Resources/shaders/SpritePS.hlsl");
	}
	else if (indexNum == OBJ || indexNum == FBX)
	{
		rootPipe.CreateBlob("Resources/shaders/OBJVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl");
	}
	else
	{
		rootPipe.CreateBlob("Resources/shaders/BasicVS.hlsl", "Resources/shaders/BasicPS.hlsl");
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
	D3D12_INPUT_ELEMENT_DESC inputLayout[7] = {
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
	},

	{//接線
	 "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1行で書いたほうが見やすい)

	{//従法線
	 "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1行で書いたほうが見やすい)

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

	pipelineDesc_.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	if (indexNum == SPRITE)
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
