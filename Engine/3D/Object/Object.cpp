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

//-------------------------------------------------------------------
Object::~Object()
{
	constBuffMaterial_.Reset();
	constBuffSkin_.Reset();
	effectFlagsBuff_.Reset();

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

//----------------------------------------------------------------
bool Object::Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	if (worldMat == nullptr)
	{
		worldMat = std::move(worldMat_);
	}

	SetWorldMat(std::move(worldMat));

	SetModel(model);

	return true;
}

void Object::SetModel(IModel* model)
{
	model_ = model;

	//アニメーションのデータ取得
	if (model && model->GetIsFbx())
	{
		ModelFBX* modelL = dynamic_cast<ModelFBX*>(model);

		animeDatas_.clear();
		animeDatas_.resize(modelL->GetAnimations().size());

		//仮でノードのポインタ渡す
		nodes_ = modelL->GetNodes();
	}
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

	if (model_ && model_->GetIsFbx())
	{
		CalcNodeMatBoneMatInternal(dynamic_cast<ModelFBX*>(model_));
	}
}

void Object::Draw()
{
	if (model_)
	{
		DrawModel(nullptr);
	}
	else if (sprite_)
	{
		DrawBoxSprite(nullptr, NULL, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f });
	}
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

//-----------------------------------------------------------------------------------------
void Object::BlendAnimationUpdate()
{
	if (!model_ || !model_->GetIsFbx())
	{
		return;
	}

	ModelFBX* model = dynamic_cast<ModelFBX*>(model_);

	const int32_t START_INDEX = 0;//とりあえず一番目のアニメーションのみ
	const int32_t END_INDEX = 0;//とりあえず一番目のアニメーションのみ

	//補間前のアニメーション
	const std::vector<ModelFBX::Animation>& ANIMATIONS =
		model->GetAnimations();
	const ModelFBX::Animation& START_ANIMATION =
		ANIMATIONS[START_INDEX];

	//補間後のアニメーション
	const ModelFBX::Animation& END_ANIMATION =
		ANIMATIONS[END_INDEX];

	//補間前のキーフレーム
	const std::vector<ModelFBX::Keyframe>& START_KEY_FRAMES = START_ANIMATION.keyframes;

	//補間後のキーフレーム
	const std::vector<ModelFBX::Keyframe>& END_KEY_FRAMES = END_ANIMATION.keyframes;

	//ノードのパラメータ
	std::vector<XMVECTOR> startScales;
	std::vector<XMVECTOR> startRotates;
	std::vector<XMVECTOR> startTranslates;
	//計算したか
	bool isStartCalc = false;

	//補間前
	int32_t startKeyNum = static_cast<int32_t>(START_KEY_FRAMES.size());
	for (int32_t i = 0; i < startKeyNum - 1; i++)
	{
		//補間前
		const ModelFBX::Keyframe& START_KEY_FRAME0 = START_KEY_FRAMES[i];
		const ModelFBX::Keyframe& START_KEY_FRAME1 = START_KEY_FRAMES[i + 1];

		//補間前計算(インデックスのキーフレームと次のキーフレームの間かどうか)
		if (animeDatas_[START_INDEX].currentTime_ >= START_KEY_FRAME0.seconds &&
			animeDatas_[START_INDEX].currentTime_ < START_KEY_FRAME1.seconds
			|| (i + 1 == startKeyNum - 1 && animeDatas_[START_INDEX].currentTime_ <= START_KEY_FRAME1.seconds))
		{
			//計算済みとする
			isStartCalc = true;
			//キーフレーム間の割合を計算
			float rate = (float)((animeDatas_[START_INDEX].currentTime_ - START_KEY_FRAME0.seconds) /
				(START_KEY_FRAME1.seconds - START_KEY_FRAME0.seconds));

			//ノード全てに適用
			int32_t nodeCount = static_cast<int32_t>(nodes_->size());
			for (int32_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
			{
				//キーフレーム0と1の全てのノード
				const ModelFBX::NodeKeyData& key0 = START_KEY_FRAME0.nodeKeys[nodeIndex];
				const ModelFBX::NodeKeyData& key1 = START_KEY_FRAME1.nodeKeys[nodeIndex];

				XMVECTOR scale1 = DirectX::XMLoadFloat3(&key1.scale);
				XMVECTOR rotate1 = DirectX::XMLoadFloat4(&key1.rotate);
				XMVECTOR translate1 = DirectX::XMLoadFloat3(&key1.trans);

				XMVECTOR scale0 = DirectX::XMLoadFloat3(&key0.scale);
				XMVECTOR rotate0 = DirectX::XMLoadFloat4(&key0.rotate);
				XMVECTOR translate0 = DirectX::XMLoadFloat3(&key0.trans);

				//線形補完でパラメータ取得する
				startScales.push_back(DirectX::XMVectorLerp(scale0, scale1, rate));
				startRotates.push_back(DirectX::XMQuaternionSlerp(rotate0, rotate1, rate));
				startTranslates.push_back(DirectX::XMVectorLerp(translate0, translate1, rate));
			}
			//パラメータ取得したので抜ける
			break;
		}
	}
	//なかった場合はパラメータをそのままにする
	if (!isStartCalc)
	{
		return;
	}

	//変数
	std::vector<DirectX::XMVECTOR> endScales;
	std::vector<DirectX::XMVECTOR> endRotates;
	std::vector<DirectX::XMVECTOR> endTranslates;
	bool isEndCalc = false;

	//補間後
	int32_t endKeyCount = static_cast<int32_t>(END_KEY_FRAMES.size());
	for (int32_t keyIndex = 0; keyIndex < endKeyCount; keyIndex++)
	{
		//補間後
		const ModelFBX::Keyframe& endKeyframe0 = END_KEY_FRAMES.at(keyIndex);
		const ModelFBX::Keyframe& endKeyframe1 = END_KEY_FRAMES.at(keyIndex + 1);

		//補間後計算
		if (animeDatas_[END_INDEX].currentTime_ >= endKeyframe0.seconds &&
			animeDatas_[END_INDEX].currentTime_ < endKeyframe1.seconds
			|| (keyIndex + 1 == endKeyCount - 1 && animeDatas_[END_INDEX].currentTime_ <= endKeyframe1.seconds))
		{
			isEndCalc = true;
			//キーフレーム間の割合
			float rate = (float)((animeDatas_[END_INDEX].currentTime_ - endKeyframe0.seconds) /
				(endKeyframe1.seconds - endKeyframe0.seconds));

			int32_t nodeCount = static_cast<int32_t>(nodes_->size());
			for (int32_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
			{
				const ModelFBX::NodeKeyData& key0 = endKeyframe0.nodeKeys[nodeIndex];
				const ModelFBX::NodeKeyData& key1 = endKeyframe1.nodeKeys[nodeIndex];

				DirectX::XMVECTOR Scale1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR Rotate1 = DirectX::XMLoadFloat4(&key1.rotate);
				DirectX::XMVECTOR Translate1 = DirectX::XMLoadFloat3(&key1.trans);

				DirectX::XMVECTOR Scale0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR Rotate0 = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR Translate0 = DirectX::XMLoadFloat3(&key0.trans);

				endScales.push_back(DirectX::XMVectorLerp(Scale0, Scale1, rate));
				endRotates.push_back(DirectX::XMQuaternionSlerp(Rotate0, Rotate1, rate));
				endTranslates.push_back(DirectX::XMVectorLerp(Translate0, Translate1, rate));
			}
			break;
		}
	}
	//キーフレーム間に当てはまらなければ抜ける
	if (!isEndCalc)
	{
		return;
	}

	//補間//ここでノードのパラメータ変えてる（rateで）→それをフレームごとに行列にしてる
	int32_t nodeCount = static_cast<int32_t>(startScales.size());
	//仮で
	float rateL = 0.5f;
	for (int32_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
	{
		DirectX::XMVECTOR Scale =
			DirectX::XMVectorLerp(
				startScales[nodeIndex],
				endScales[nodeIndex], rateL);
		DirectX::XMVECTOR Rotate =
			DirectX::XMQuaternionSlerp(
				startRotates[nodeIndex],
				endRotates[nodeIndex], rateL);
		DirectX::XMVECTOR Translate =
			DirectX::XMVectorLerp(
				startTranslates[nodeIndex],
				endTranslates[nodeIndex], rateL);

		//線形補完したパラメータを取得
		Node& node = (*nodes_)[nodeIndex];
		node.scaling = Scale;
		node.rotation = Rotate;
		node.translation = Translate;
	}
}

void Object::AnimationUpdate()
{
	if (!model_ || !model_->GetIsFbx())
	{
		return;
	}

	//キーフレーム（対応した全ノードを所持）の配列を取得
	const std::vector<ModelFBX::Keyframe>& KEY_FRAMES = dynamic_cast<ModelFBX*>(model_)->GetAnimations()[animeIndex_].keyframes;
	int32_t keyCount = static_cast<int32_t>(KEY_FRAMES.size());
	for (int32_t i = 0; i < keyCount - 1; i++)
	{
		//補間前
		const ModelFBX::Keyframe& KEY_FRAME0 = KEY_FRAMES[i];
		const ModelFBX::Keyframe& KEY_FRAME1 = KEY_FRAMES[i + 1];

		//補間前計算(インデックスのキーフレームと,その次のキーフレームの間かどうか)
		if (animeDatas_[animeIndex_].currentTime_ >= KEY_FRAME0.seconds &&
			animeDatas_[animeIndex_].currentTime_ < KEY_FRAME1.seconds)
		{
			//キーフレーム間の割合を計算
			float rate = (float)((animeDatas_[animeIndex_].currentTime_ - KEY_FRAME0.seconds) /
				(KEY_FRAME1.seconds - KEY_FRAME0.seconds));

			//オブジェクトクラスが持っているノード全てに適用
			int32_t nodeCount = static_cast<int32_t>(nodes_->size());
			for (int32_t j = 0; j < nodeCount; j++)
			{
				//キーフレーム0と1のインデックスのノードを取得
				const ModelFBX::NodeKeyData& KEY_0 = KEY_FRAME0.nodeKeys[j];
				const ModelFBX::NodeKeyData& KEY_1 = KEY_FRAME1.nodeKeys[j];

				//参照
				auto& node = (*nodes_)[j];

				XMVECTOR scale1 = DirectX::XMLoadFloat3(&KEY_1.scale);
				XMVECTOR rotate1 = DirectX::XMLoadFloat4(&KEY_1.rotate);
				XMVECTOR translate1 = DirectX::XMLoadFloat3(&KEY_1.trans);

				XMVECTOR scale0 = DirectX::XMLoadFloat3(&KEY_0.scale);
				XMVECTOR rotate0 = DirectX::XMLoadFloat4(&KEY_0.rotate);
				XMVECTOR translate0 = DirectX::XMLoadFloat3(&KEY_0.trans);

				//線形補完でパラメータ取得する
				XMVECTOR scale = (DirectX::XMVectorLerp(scale0, scale1, rate));
				XMVECTOR rotate = (DirectX::XMQuaternionSlerp(rotate0, rotate1, rate));
				XMVECTOR trans = (DirectX::XMVectorLerp(translate0, translate1, rate));
				//代入
				node.scaling = scale;
				node.rotation = rotate;
				node.translation = trans;
			}
		}
	}
}

void Object::UpdateFBXNodeMat()
{
	//オブジェクトクラスが持ってるfbxモデルのノード全て
	for (Node& node : *nodes_)
	{
		//行列の作成(アニメーションなどで変化したパラメータを使用)
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(
			node.scaling.m128_f32[0],
			node.scaling.m128_f32[1],
			node.scaling.m128_f32[2]);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(node.rotation);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
			node.translation.m128_f32[0],
			node.translation.m128_f32[1],
			node.translation.m128_f32[2]);
		DirectX::XMMATRIX local_transform = S * R * T;

		//親がいるか(恐らくノードの上の方から順に登録され、処理しているのでこれでもできる)
		DirectX::XMMATRIX parent_transform;
		if (node.parent != nullptr)
		{
			parent_transform = node.parent->globalTransform;
		}
		else
		{
			parent_transform = DirectX::XMMatrixIdentity();
		}

		//モデルのノードのワールド
		DirectX::XMMATRIX world_transform = local_transform * parent_transform;

		//行列保存
		node.transform = local_transform;
		node.globalTransform = world_transform;
	}
}

//-------------------------------------------------------------------------------------
void Object::PlayAnimationInternal(int32_t animeIndex,
	bool isLoop, bool isReverse)
{
	if (!(model_ && model_->GetIsFbx()))
	{
		return;
	}

	animeIndex_ = animeIndex;

	//アニメーションのリセット
	AnimationReset(animeIndex);

	//再生中状態
	animeDatas_[animeIndex].isPlay_ = true;
	//ループ
	animeDatas_[animeIndex].isLoop_ = isLoop;
	//逆再生
	animeDatas_[animeIndex].isReverse_ = isReverse;
}

void Object::AnimationReset(int32_t animeIndex)
{
	if (model_ == nullptr || !model_->GetIsFbx())
	{
		return;
	}

	//子クラスに変換
	ModelFBX* model = dynamic_cast<ModelFBX*>(model_);

	//アニメーションが1つしかない前提
	FbxScene* fbxScene = model->GetFbxScene();
	//アニメーション取得
	FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(animeIndex);
	//アニメーションなかったら
	if (animStack == nullptr) { return; }

	//アニメーションの名前取得
	const char* ANIM_STACK_NAME = animStack->GetName();
	//アニメーションの時間情報
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(ANIM_STACK_NAME);

	//開始時間取得
	animeDatas_[animeIndex].currentTime_ = dynamic_cast<ModelFBX*>(model_)->GetAnimations()[animeIndex].startTime;
}

void Object::PlayAnimation(bool isLoop, int32_t animeIndex)
{
	PlayAnimationInternal(animeIndex, isLoop);
}

void Object::PlayReverseAnimation(bool isLoop, int32_t animeIndex)
{
	PlayAnimationInternal(isLoop, true);
}

void Object::CalcNodeMatBoneMatInternal(ModelFBX* model)
{
	//とりあえず一番のアニメ　仮
	auto anime = model->GetAnimations()[animeIndex_];

	AnimationData& animeData = animeDatas_[animeIndex_];

	//アニメーション
	if (animeData.isPlay_)
	{
		ModelFBX* model = dynamic_cast<ModelFBX*>(model_);

		//最後まで再生したら
		if ((!animeData.isReverse_ && animeData.currentTime_ >= anime.endTime)
			|| (animeData.isReverse_ && animeData.currentTime_ <= anime.startTime))
		{
			if (!animeData.isReverse_)
			{
				//先頭に戻す
				animeData.currentTime_ = anime.startTime;
			}
			else
			{
				//最後に戻す
				animeData.currentTime_ = anime.endTime;
			}

			//終了
			if (!animeData.isLoop_)
			{
				animeData.isPlay_ = false;
			}
		}

		else
		{
			//逆再生
			if (animeData.isReverse_)
			{
				animeData.currentTime_ -= anime.addTime * animeData.animationSpeed_;
			}
			else
			{
				//1フレーム進める
				animeData.currentTime_ += anime.addTime * animeData.animationSpeed_;
			}

			//範囲内に収める
			animeData.currentTime_ = min(max(animeData.currentTime_, anime.startTime), anime.endTime);
		}
	}

	if (animeData.isPlay_)
	{
		//アニメーションのアップデート
		BlendAnimationUpdate();
	}
	//ノードの行列更新
	UpdateFBXNodeMat();

	//マッピング
	MappingBoneData(model);
}

const std::vector<Node>& Object::GetNodes()
{
	return *nodes_;
}

void Object::MappingBoneData(ModelFBX* model)
{
	//定数バッファへデータ転送
	HRESULT result = constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin);
	for (int32_t i = 0; i < model->GetBoneNodeIndices().size(); i++)
	{
		//行列計算
		constMapSkin->bones[i] = GetCalcSkinMat(model_, i);
	}
	constBuffSkin_->Unmap(0, nullptr);
}

XMMATRIX Object::GetCalcSkinMat(IModel* model, int32_t index)
{
	//ボーン行列がおかしい！
	if (!(model && model->GetIsFbx()))
	{
		return XMMatrixIdentity();
	}

	ModelFBX* modelL = dynamic_cast<ModelFBX*>(model);

	uint64_t indexL = modelL->GetBoneNodeIndices()[index];

	//アニメーションで更新したワールド
	DirectX::XMMATRIX worldTransform =
		(*nodes_)[indexL].globalTransform;
	//初期姿勢などが入った行列
	DirectX::XMMATRIX offsetTransform =
		modelL->GetOffsetTransforms()[index];
	//ボーン行列(上の二つを掛け合わせる)
	DirectX::XMMATRIX boneTransform =
		offsetTransform * worldTransform;

	////スケールを省く処理
	// 
	//DirectX::XMFLOAT3 l_scale = {};
	//l_scale.x = 1 / scale.x;
	//l_scale.y = 1 / scale.y;
	//l_scale.z = 1 / scale.z;
	//worldTransform.r[0].m128_f32[0] *= l_scale.x;
	//worldTransform.r[0].m128_f32[1] *= l_scale.x;
	//worldTransform.r[0].m128_f32[2] *= l_scale.x;
	//worldTransform.r[1].m128_f32[0] *= l_scale.y;
	//worldTransform.r[1].m128_f32[1] *= l_scale.y;
	//worldTransform.r[1].m128_f32[2] *= l_scale.y;
	//worldTransform.r[2].m128_f32[0] *= l_scale.z;
	//worldTransform.r[2].m128_f32[1] *= l_scale.z;
	//worldTransform.r[2].m128_f32[2] *= l_scale.z;

	//fbxData.push_back(std::make_pair(
	//	nodes.at(model->GetNodeIndices().at(i)).name,
	//	worldTransform * matWorld));
	//DirectX::XMFLOAT4 fbxMatRot = {};
	//fbxMatRot = nodes.at(model->GetNodeIndices().at(i)).rotate;

	//DirectX::XMMATRIX matRot = DirectX::XMMatrixIdentity();
	//matRot *= DirectX::XMMatrixRotationZ(fbxMatRot.z);
	//matRot *= DirectX::XMMatrixRotationX(fbxMatRot.x);
	//matRot *= DirectX::XMMatrixRotationY(fbxMatRot.y);
	//localMatRots.push_back(matRot);

	//std::string l_name = nodes.at(model->GetNodeIndices().at(i)).name;
	//if (l_name.find("RightHand", 0) != std::string::npos)
	//{
	//	matrix = worldTransform * matWorld;
	//}

	return boneTransform;
}

//-----------------------------------------------------------------------------------------
void Object::SetCollider(std::unique_ptr<BaseCollider> collider)
{
	collider->SetObject(this);
	collider_ = std::move(collider);
	//コリジョンマネージャーに登録
	CollisionManager::GetInstance()->AddCollider(collider_.get());
	//行列,コライダーの更新
	Object::WorldMatColliderUpdate();

	if (collider_ && collider_->GetIs2D() && sprite_ == nullptr)
	{
		sprite_ = std::make_unique<Sprite>();
		sprite_->Initialize();
	}
}

//-----------------------------------------------------------------------------
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
			//オブジェクトのローカル
			worldMat_->CalcWorldMat();
			//親行列
			M4 pM = worldMat_->GetOnlyParentALLTreeMat();
			//ボーン(ボーンのグローバルトランスフォーム、)
			M4 bM;
			{
				//親ノードと同じ名前のボーンのインデックスを得る
				uint64_t bIndex = parentNodeModel_->GetBoneIndex(parentNode_->name);
				//親ノードがあるモデルのボーンをインデックスで指定して行列の計算
				XMMATRIX bXM;
				//親のワールド行列
				pM.MatIntoXMMATRIX(bXM);
				//親のノード行列にワールド行列を掛ける
				bXM = parentObj_->GetNodes()[bIndex].globalTransform;
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

void Object::DrawUpdate(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, ConstBuffTransform* constBuffTransform,
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

void Object::ResetParentFbxNode()
{
	parentNode_ = nullptr;
	parentNodeModel_ = nullptr;
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
			return;
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

	//正面ベクトル
	if (ImGui::TreeNode("FrontVec")) {

		ImGui::Text("FrontVec: %.2f %.2f %.2f", frontVec_.x, frontVec_.y, frontVec_.z);
		ImGui::Text("FrontVecTmp: %.2f %.2f %.2f", frontVecTmp_.x, frontVecTmp_.y, frontVecTmp_.z);

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

		if (model_ && model_->GetIsFbx())
		{
			AnimationData animeData = animeDatas_[animeIndex_];
			ModelFBX::Animation anime;

			anime = dynamic_cast<ModelFBX*>(model_)->GetAnimations()[animeIndex_];

			ImGui::Checkbox("isPlay", &animeData.isPlay_);
			ImGui::Checkbox("isLoop", &animeData.isLoop_);
			ImGui::Checkbox("isReverse", &animeData.isReverse_);
			if (animeData.currentTime_ - anime.startTime > 0 && anime.endTime - anime.startTime > 0)
			{
				ImGui::Text("animationTimeRatio: %.2f", (animeData.currentTime_ - anime.startTime) / (anime.endTime - anime.startTime));
			}
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
