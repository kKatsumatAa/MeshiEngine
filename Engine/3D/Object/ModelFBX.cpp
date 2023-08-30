#include "ModelFBX.h"
#include "DirectXWrapper.h"
#include "FbxLoader.h"
#include "Object.h"
#include "ImGuiManager.h"


/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
const std::string ModelFBX::S_TYPE_DIRECTORY_ = "fbx/";


ModelFBX::~ModelFBX()
{
	//fbx�V�[���̉���iFBXLoader��Finalize()�̑O����Ȃ��ƃG���[�o��j
	fbxScene_->Destroy();
}

ModelFBX::ModelFBX()
{
	if (constBuffSkin_.Get() == nullptr)
	{
		//�X�L���̃o�b�t�@
		//�q�[�v�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{}; D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
		//���\�[�X�ݒ�
		ResourceProperties(cbResourceDesc,
			((uint32_t)sizeof(ConstBufferDataSkin) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/);
		//�萔�o�b�t�@�̐���
		BuffProperties(cbHeapProp, cbResourceDesc, &constBuffSkin_);
		//�}�b�s���O
		ConstBufferDataSkin* constMapSkin = nullptr;
		constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin);
		for (uint32_t i = 0; i < S_MAX_BONES_; i++)
		{
			constMapSkin->bones[i] = DirectX::XMMatrixIdentity();
		}
		constBuffSkin_->Unmap(0, nullptr);
	}

	//1�t���[�����̎��Ԃ�60fps�Őݒ�
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

	//�A�j���[�V����
	if (isPlay_)
	{
		//�Ō�܂ōĐ�������
		if ((!isReverse_ && currentTime_ >= endTime_)
			|| (isReverse_ && currentTime_ <= startTime_))
		{
			if (!isReverse_)
			{
				//�擪�ɖ߂�
				currentTime_ = startTime_;
			}
			else
			{
				//�Ō�ɖ߂�
				currentTime_ = endTime_;
			}

			//�I��
			if (!isLoop_)
			{
				isPlay_ = false;
			}
		}

		//�A�j���[�V�����X�s�[�h��������
		frameTime_.SetTime(0, 0, 0, max((int)((1000.0f / 60.0f) * animationSpeed_), 1), 0, FbxTime::EMode::eFrames1000);

		//�t�Đ�
		if (isReverse_)
		{
			currentTime_ -= frameTime_;
		}
		else
		{
			//1�t���[���i�߂�
			currentTime_ += frameTime_;
		}
	}

	//���f���̃{�[���z��
	std::vector<ModelFBX::Bone>& bones = GetBones();

	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin);
	for (int32_t i = 0; i < bones.size(); i++)
	{
		//���̎p���s��
		XMMATRIX matCurrentPose;
		//���̎p���s����擾
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime_);
		//xmmatrix�ɕϊ�
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);


		//�����p���̋t�s��ƍ��̎p���s����������ăX�L�j���O�s���
		XMMATRIX globalInv = XMMatrixInverse(nullptr, bones[i].globalTransform);
		constMapSkin->bones[i] = bones[i].globalTransform * bones[i].invInitialPose * matCurrentPose * globalInv;
	}
	constBuffSkin_->Unmap(0, nullptr);
}

void ModelFBX::PlayAnimationInternal(FbxTime& sTime, FbxTime& eTime,
	bool isLoop, bool isReverse)
{
	//�A�j���[�V�����̃��Z�b�g
	AnimationReset(sTime, eTime);

	//�Đ������
	isPlay_ = true;
	//���[�v
	isLoop_ = isLoop;
	//�t�Đ�
	isReverse_ = isReverse;
}

void ModelFBX::AnimationReset(FbxTime& sTime, FbxTime& eTime)
{
	//�q�N���X�ɕϊ�
	ModelFBX* model = this;

	//�A�j���[�V������1�����Ȃ��O��
	FbxScene* fbxScene = model->GetFbxScene();
	//0�Ԃ̃A�j���[�V�����擾
	FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//�A�j���[�V�����Ȃ�������
	if (animStack == nullptr) { return; }

	//�A�j���[�V�����̖��O�擾
	const char* ANIM_STACK_NAME = animStack->GetName();
	//�A�j���[�V�����̎��ԏ��
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(ANIM_STACK_NAME);

	//�J�n���Ԏ擾
	sTime = takeInfo->mLocalTimeSpan.GetStart();
	//�I�����Ԏ擾
	eTime = takeInfo->mLocalTimeSpan.GetStop();
	//�J�n���Ԏ擾
	currentTime_ = startTime_;
}

//----------------------------------------------------------------------------------------------------------
//
	//�X�L�j���O�p
void ModelFBX::Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex, const ConstBuffTransform& cbt)
{
	SendingBoneData();

	auto setBuffsF = [=]()
	{
		setMaterialLightTex();
		//�{�[���̍s��Z�b�g
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
