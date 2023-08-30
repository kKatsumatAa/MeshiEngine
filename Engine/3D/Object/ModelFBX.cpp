#include "ModelFBX.h"
#include "DirectXWrapper.h"
#include "FbxLoader.h"
#include "Object.h"
#include "ImGuiManager.h"


/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string ModelFBX::S_TYPE_DIRECTORY_ = "fbx/";


ModelFBX::~ModelFBX()
{
	//fbxシーンの解放（FBXLoaderのFinalize()の前じゃないとエラー出る）
	fbxScene_->Destroy();
}

ModelFBX::ModelFBX()
{
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
			constMapSkin->bones[i] = DirectX::XMMatrixIdentity();
		}
		constBuffSkin_->Unmap(0, nullptr);
	}

	//1フレーム分の時間を60fpsで設定
	frameTime_.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
}

void ModelFBX::Initialize()
{
	isFbx_ = true;

	IModel::Initialize();

	scaleExtend_ = 0.01f;
}

void ModelFBX::LoadTextures()
{
	std::string directoryPathL = IModel::S_BASE_DIRECTORY_ + S_TYPE_DIRECTORY_ + name_ + "/";

	IModel::LoadTexturesInternal(directoryPathL);
}

void ModelFBX::PlayAnimation(bool isLoop)
{
	PlayAnimationInternal(startTime_, endTime_, isLoop);
}

void ModelFBX::PlayReverseAnimation(bool isLoop)
{
	PlayAnimationInternal(endTime_, startTime_, isLoop, true);
}

void ModelFBX::SendingBoneData()
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

	//モデルのボーン配列
	std::vector<ModelFBX::Bone>& bones = GetBones();

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


		//初期姿勢の逆行列と今の姿勢行列を合成してスキニング行列に
		XMMATRIX globalInv = XMMatrixInverse(nullptr, bones[i].globalTransform);
		constMapSkin->bones[i] = bones[i].globalTransform * bones[i].invInitialPose * matCurrentPose * globalInv;
	}
	constBuffSkin_->Unmap(0, nullptr);
}

void ModelFBX::PlayAnimationInternal(FbxTime& sTime, FbxTime& eTime,
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

void ModelFBX::AnimationReset(FbxTime& sTime, FbxTime& eTime)
{
	//子クラスに変換
	ModelFBX* model = this;

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

//----------------------------------------------------------------------------------------------------------
//
	//スキニング用
void ModelFBX::Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex, const ConstBuffTransform& cbt)
{
	SendingBoneData();

	auto setBuffsF = [=]()
	{
		setMaterialLightTex();
		//ボーンの行列セット
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(Object::SKIN, constBuffSkin_->GetGPUVirtualAddress());
	};

	IModel::Draw(setRootParam, setBuffsF, cbt);
}

void ModelFBX::DrawImGui(const std::function<void()>& childImguiF)
{
	std::function<void()>f = [=]()
	{
		ImGui::Checkbox("isPlay", &isPlay_);
		ImGui::Checkbox("isLoop", &isLoop_);
		ImGui::Checkbox("isReverse", &isReverse_);
		if (currentTime_ - startTime_ > 0 && endTime_ - startTime_ > 0)
		{
			float timer = (float)(currentTime_ - startTime_).GetSecondDouble();
			float timerMax = (float)(endTime_ - startTime_).GetSecondDouble();

			ImGui::Text("animationTimeRatio: %.2f", timer / timerMax);
		}
	};

	IModel::DrawImGui(f);
}
