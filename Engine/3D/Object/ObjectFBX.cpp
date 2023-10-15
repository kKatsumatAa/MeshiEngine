#include"ObjectFBX.h"
#include"BaseCollider.h"

using namespace DirectX;

//------------------------------------------
//パイプラインなどの設定
RootPipe ObjectFBX::pipelineSetM_[2];

//--------------------------------------------------------------------------------------
ObjectFBX::~ObjectFBX()
{
}

ObjectFBX::ObjectFBX()
{
	//インスタンスの種類
	objInsType_ = ObjectInstanceType::FBX;

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

//------------------------------------------------------------------------------------------
void ObjectFBX::CommonInitialize()
{
	//パイプラインなどの設定
	PipeLineSetting(D3D12_FILL_MODE_SOLID, pipelineSetM_[0],
		"Resources/shaders/FBXVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl",
		sInputLayoutM_, _countof(sInputLayoutM_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false);
	//ワイヤーフレーム
	PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, pipelineSetM_[1],
		"Resources/shaders/FBXVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl",
		sInputLayoutM_, _countof(sInputLayoutM_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false);
}

//-----------------------------------------------------------------------------------------
void ObjectFBX::SetMaterialLightMTexSkinModel(uint64_t dissolveTexHandle, uint64_t specularMapTexhandle,
	uint64_t normalMapTexHandle)
{
	SetMaterialLightMTex(NULL, dissolveTexHandle, specularMapTexhandle, normalMapTexHandle, false);

	//スキニング用
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(SKIN, constBuffSkin_->GetGPUVirtualAddress());
}

void ObjectFBX::DrawModelInternal(int32_t pipelineNum)
{
	//テクスチャを設定していなかったら
	uint64_t dissolveTextureHandleL = dissolveTextureHandle_;
	TextureManager::CheckTexHandle(dissolveTextureHandleL);
	uint64_t specularMapTextureHandleL = specularMapTextureHandle_;
	TextureManager::CheckTexHandle(specularMapTextureHandleL);
	uint64_t normalMapTextureHandleL = normalMapTextureHandle_;
	TextureManager::CheckTexHandle(normalMapTextureHandleL);

	//モデル用
	//ラムダ式でコマンド関数(ボーン行列もセット)
	std::function<void()>SetRootPipeRM = [=]() {SetRootPipe(pipelineSetM_, pipelineNum, pipelineSetM_[0].rootSignature.Get()); };
	std::function<void()>SetMaterialTexM = [=]() {SetMaterialLightMTexSkinModel(
		dissolveTextureHandleL, specularMapTextureHandleL, normalMapTextureHandleL); };

	//メッシュのオフセットデータセット
	GetModel()->SetPolygonOffsetData(meshOffsetData_);

	model_->Draw(SetRootPipeRM, SetMaterialTexM, cbt_);
}

//-----------------------------------------------------------------------------------------
void ObjectFBX::BlendAnimationUpdate()
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
				DirectX::XMVECTOR rotate1 = DirectX::XMLoadFloat4(&key1.rotate);
				DirectX::XMVECTOR Translate1 = DirectX::XMLoadFloat3(&key1.trans);

				DirectX::XMVECTOR Scale0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR rotate0 = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR Translate0 = DirectX::XMLoadFloat3(&key0.trans);

				endScales.push_back(DirectX::XMVectorLerp(Scale0, Scale1, rate));
				endRotates.push_back(DirectX::XMQuaternionSlerp(rotate0, rotate1, rate));
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
		DirectX::XMVECTOR rotate =
			DirectX::XMQuaternionSlerp(
				startRotates[nodeIndex],
				endRotates[nodeIndex], rateL);
		DirectX::XMVECTOR Translate =
			DirectX::XMVectorLerp(
				startTranslates[nodeIndex],
				endTranslates[nodeIndex], rateL);

		//線形補完したパラメータを取得
		Node& node = (*nodes_)[nodeIndex];
		node.scaling = { Scale.m128_f32[0],Scale.m128_f32[1],Scale.m128_f32[2],Scale.m128_f32[3] };
		node.rotation = { rotate.m128_f32[0],rotate.m128_f32[1],rotate.m128_f32[2],rotate.m128_f32[3] };
		node.translation = { Translate.m128_f32[0],Translate.m128_f32[1],Translate.m128_f32[2],Translate.m128_f32[3] };
	}
}

void ObjectFBX::AnimationUpdate()
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
				node.scaling = { scale.m128_f32[0],scale.m128_f32[1],scale.m128_f32[2],scale.m128_f32[3] };
				node.rotation = { rotate.m128_f32[0],rotate.m128_f32[1],rotate.m128_f32[2],rotate.m128_f32[3] };
				node.translation = { trans.m128_f32[0],trans.m128_f32[1],trans.m128_f32[2],trans.m128_f32[3] };
			}
		}
	}
}

void ObjectFBX::SetNodeAddRot(const std::string& nodeName, const Vec3& addRot)
{
	if (nodes_)
	{
		for (auto& node : *nodes_)
		{
			//一部でも文字列が一致していたら加算用の角度をセット
			if (node.name.find(nodeName) != std::string::npos)
			{
				node.addRot = { addRot.x,addRot.y,addRot.z,0 };
			}
		}
	}
}

void ObjectFBX::UpdateFBXNodeMat()
{
	//オブジェクトクラスが持ってるfbxモデルのノード全て
	for (Node& node : *nodes_)
	{
		//行列の作成(アニメーションなどで変化したパラメータを使用)
		//大きさ
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(
			node.scaling.x,
			node.scaling.y,
			node.scaling.z);
		//回転
		XMVECTOR rotVec = { node.rotation.x + node.addRot.x,
			node.rotation.y + node.addRot.y,
			node.rotation.z + node.addRot.z,
			node.rotation.w };
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(rotVec);
		//位置
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
			node.translation.x,
			node.translation.y,
			node.translation.z);
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
void ObjectFBX::PlayAnimationInternal(int32_t animeIndex,
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

void ObjectFBX::AnimationReset(int32_t animeIndex)
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

	//開始時間取得
	animeDatas_[animeIndex].currentTime_ = dynamic_cast<ModelFBX*>(model_)->GetAnimations()[animeIndex].startTime;
}

IObject3D* ObjectFBX::GetNodeColliderObj(const std::string& nodeName)
{
	return nodeColliders_.GetColliderObj(nodeName);
}

void ObjectFBX::PlayAnimation(bool isLoop, int32_t animeIndex)
{
	PlayAnimationInternal(animeIndex, isLoop);
}

void ObjectFBX::PlayReverseAnimation(bool isLoop, int32_t animeIndex)
{
	PlayAnimationInternal(animeIndex, isLoop, true);
}

//---------------------------------------------------------------------------
void ObjectFBX::CalcNodeMatBoneMatInternal(ModelFBX* model)
{
	//とりあえず一番のアニメ　仮
	auto anime = model->GetAnimations()[animeIndex_];

	AnimationData& animeData = animeDatas_[animeIndex_];

	//アニメーション
	if (animeData.isPlay_)
	{
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

void ObjectFBX::MappingBoneData(ModelFBX* model)
{
	//定数バッファへデータ転送
	HRESULT result = S_OK;
	result = constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin_);
	assert(SUCCEEDED(result));
	for (int32_t i = 0; i < model->GetBoneNodeIndices().size(); i++)
	{
		//行列計算
		constMapSkin_->bones[i] = GetCalcSkinMat(model_, i);
	}
	constBuffSkin_->Unmap(0, nullptr);
}

//---------------------------------------------------------------------------
const std::vector<Node>& ObjectFBX::GetNodes()
{
	return *nodes_;
}

XMMATRIX ObjectFBX::GetCalcSkinMat(IModel* model, int32_t index)
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

	return boneTransform;
}

//------------------------------------------------------------------------------------------
void ObjectFBX::Update()
{
	if (model_ && model_->GetIsFbx())
	{
		CalcNodeMatBoneMatInternal(dynamic_cast<ModelFBX*>(model_));
	}
}

void ObjectFBX::DrawImGui(std::function<void()> imguiF)
{
	auto f = [=]() {

		if (model_ && model_->GetIsFbx())
		{
			AnimationData animeData = animeDatas_[animeIndex_];
			ModelFBX::Animation anime;

			anime = dynamic_cast<ModelFBX*>(model_)->GetAnimations()[animeIndex_];

			//ノードごとの当たり判定描画
			ImGui::Checkbox("isValidNodeCollidersDraw", &isValidNodeCollidersDraw_);

			ImGui::Checkbox("isPlay", &animeData.isPlay_);
			ImGui::Checkbox("isLoop", &animeData.isLoop_);
			ImGui::Checkbox("isReverse", &animeData.isReverse_);
			if (animeData.currentTime_ - anime.startTime > 0 && anime.endTime - anime.startTime > 0)
			{
				ImGui::Text("animationTimeRatio: %.2f", (animeData.currentTime_ - anime.startTime) / (anime.endTime - anime.startTime));
			}
		}
		};

	//親クラスの
	IObject3D::DrawImGui(f);
}

//------------------------------------------------------------------------------------------
void ObjectFBX::SetModel(IModel* model)
{
	//共通の初期化
	IObject3D::SetModel(model);

	//アニメーションのデータ取得
	if (model && model->GetIsFbx())
	{
		ModelFBX* modelL = dynamic_cast<ModelFBX*>(model);

		animeDatas_.clear();
		animeDatas_.resize(modelL->GetAnimations().size());

		//仮でノードのポインタ渡す
		nodes_ = modelL->GetNodes();

		//
		AnimationUpdate();
	}
}

void ObjectFBX::InitializeNodeColliders(ModelFBX* model, float colliderScale, uint16_t attribute)
{
	nodeColliders_.SetNodesPointer(model->GetNodes());
	nodeColliders_.CreateNodeColliders(attribute);
	nodeColliders_.SetScaleCollider(colliderScale);
	nodeColliders_.SetParentObj(this);
}
