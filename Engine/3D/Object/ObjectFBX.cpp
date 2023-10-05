#include"ObjectFBX.h"
#include"BaseCollider.h"

using namespace DirectX;

//------------------------------------------
//�p�C�v���C���Ȃǂ̐ݒ�
RootPipe ObjectFBX::pipelineSetM_[2];

//�C���v�b�g���C�A�E�g
D3D12_INPUT_ELEMENT_DESC ObjectFBX::sInputLayout_[7] = {
	{//xyz���W
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�@���x�N�g��
	 "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//uv���W
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�e�����󂯂�{�[���ԍ�
	 "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�{�[���̃X�L���E�F�C�g�i4�j
	 "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�ڐ�
	 "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�]�@��
	 "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

};


//--------------------------------------------------------------------------------------
ObjectFBX::~ObjectFBX()
{
}

ObjectFBX::ObjectFBX()
{
	//�C���X�^���X�̎��
	objInsType_ = ObjectInstanceType::FBX;

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
		constMapSkin->bones[i] = XMMatrixIdentity();
	}
	constBuffSkin_->Unmap(0, nullptr);
}

//------------------------------------------------------------------------------------------
void ObjectFBX::CommonInitialize()
{
	//�p�C�v���C���Ȃǂ̐ݒ�
	PipeLineSetting(D3D12_FILL_MODE_SOLID, pipelineSetM_[0],
		"Resources/shaders/FBXVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl",
		sInputLayout_, _countof(sInputLayout_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false);
	//���C���[�t���[��
	PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, pipelineSetM_[1],
		"Resources/shaders/FBXVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl",
		sInputLayout_, _countof(sInputLayout_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false);
}

//-----------------------------------------------------------------------------------------
void ObjectFBX::SetMaterialLightMTexSkinModel(uint64_t dissolveTexHandle, uint64_t specularMapTexhandle,
	uint64_t normalMapTexHandle)
{
	SetMaterialLightMTex(NULL, dissolveTexHandle, specularMapTexhandle, normalMapTexHandle, false);

	//�X�L�j���O�p
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(SKIN, constBuffSkin_->GetGPUVirtualAddress());
}

void ObjectFBX::DrawModelInternal(int32_t pipelineNum)
{
	//�e�N�X�`����ݒ肵�Ă��Ȃ�������
	uint64_t dissolveTextureHandleL = dissolveTextureHandle_;
	TextureManager::CheckTexHandle(dissolveTextureHandleL);
	uint64_t specularMapTextureHandleL = specularMapTextureHandle_;
	TextureManager::CheckTexHandle(specularMapTextureHandleL);
	uint64_t normalMapTextureHandleL = normalMapTextureHandle_;
	TextureManager::CheckTexHandle(normalMapTextureHandleL);

	//���f���p
	//�����_���ŃR�}���h�֐�(�{�[���s����Z�b�g)
	std::function<void()>SetRootPipeRM = [=]() {SetRootPipe(pipelineSetM_, pipelineNum, pipelineSetM_[0].rootSignature.Get()); };
	std::function<void()>SetMaterialTexM = [=]() {SetMaterialLightMTexSkinModel(
		dissolveTextureHandleL, specularMapTextureHandleL, normalMapTextureHandleL); };

	//���b�V���̃I�t�Z�b�g�f�[�^�Z�b�g
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

	const int32_t START_INDEX = 0;//�Ƃ肠������Ԗڂ̃A�j���[�V�����̂�
	const int32_t END_INDEX = 0;//�Ƃ肠������Ԗڂ̃A�j���[�V�����̂�

	//��ԑO�̃A�j���[�V����
	const std::vector<ModelFBX::Animation>& ANIMATIONS =
		model->GetAnimations();
	const ModelFBX::Animation& START_ANIMATION =
		ANIMATIONS[START_INDEX];

	//��Ԍ�̃A�j���[�V����
	const ModelFBX::Animation& END_ANIMATION =
		ANIMATIONS[END_INDEX];

	//��ԑO�̃L�[�t���[��
	const std::vector<ModelFBX::Keyframe>& START_KEY_FRAMES = START_ANIMATION.keyframes;

	//��Ԍ�̃L�[�t���[��
	const std::vector<ModelFBX::Keyframe>& END_KEY_FRAMES = END_ANIMATION.keyframes;

	//�m�[�h�̃p�����[�^
	std::vector<XMVECTOR> startScales;
	std::vector<XMVECTOR> startRotates;
	std::vector<XMVECTOR> startTranslates;
	//�v�Z������
	bool isStartCalc = false;

	//��ԑO
	int32_t startKeyNum = static_cast<int32_t>(START_KEY_FRAMES.size());
	for (int32_t i = 0; i < startKeyNum - 1; i++)
	{
		//��ԑO
		const ModelFBX::Keyframe& START_KEY_FRAME0 = START_KEY_FRAMES[i];
		const ModelFBX::Keyframe& START_KEY_FRAME1 = START_KEY_FRAMES[i + 1];

		//��ԑO�v�Z(�C���f�b�N�X�̃L�[�t���[���Ǝ��̃L�[�t���[���̊Ԃ��ǂ���)
		if (animeDatas_[START_INDEX].currentTime_ >= START_KEY_FRAME0.seconds &&
			animeDatas_[START_INDEX].currentTime_ < START_KEY_FRAME1.seconds
			|| (i + 1 == startKeyNum - 1 && animeDatas_[START_INDEX].currentTime_ <= START_KEY_FRAME1.seconds))
		{
			//�v�Z�ς݂Ƃ���
			isStartCalc = true;
			//�L�[�t���[���Ԃ̊������v�Z
			float rate = (float)((animeDatas_[START_INDEX].currentTime_ - START_KEY_FRAME0.seconds) /
				(START_KEY_FRAME1.seconds - START_KEY_FRAME0.seconds));

			//�m�[�h�S�ĂɓK�p
			int32_t nodeCount = static_cast<int32_t>(nodes_->size());
			for (int32_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
			{
				//�L�[�t���[��0��1�̑S�Ẵm�[�h
				const ModelFBX::NodeKeyData& key0 = START_KEY_FRAME0.nodeKeys[nodeIndex];
				const ModelFBX::NodeKeyData& key1 = START_KEY_FRAME1.nodeKeys[nodeIndex];

				XMVECTOR scale1 = DirectX::XMLoadFloat3(&key1.scale);
				XMVECTOR rotate1 = DirectX::XMLoadFloat4(&key1.rotate);
				XMVECTOR translate1 = DirectX::XMLoadFloat3(&key1.trans);

				XMVECTOR scale0 = DirectX::XMLoadFloat3(&key0.scale);
				XMVECTOR rotate0 = DirectX::XMLoadFloat4(&key0.rotate);
				XMVECTOR translate0 = DirectX::XMLoadFloat3(&key0.trans);

				//���`�⊮�Ńp�����[�^�擾����
				startScales.push_back(DirectX::XMVectorLerp(scale0, scale1, rate));
				startRotates.push_back(DirectX::XMQuaternionSlerp(rotate0, rotate1, rate));
				startTranslates.push_back(DirectX::XMVectorLerp(translate0, translate1, rate));
			}
			//�p�����[�^�擾�����̂Ŕ�����
			break;
		}
	}
	//�Ȃ������ꍇ�̓p�����[�^�����̂܂܂ɂ���
	if (!isStartCalc)
	{
		return;
	}

	//�ϐ�
	std::vector<DirectX::XMVECTOR> endScales;
	std::vector<DirectX::XMVECTOR> endRotates;
	std::vector<DirectX::XMVECTOR> endTranslates;
	bool isEndCalc = false;

	//��Ԍ�
	int32_t endKeyCount = static_cast<int32_t>(END_KEY_FRAMES.size());
	for (int32_t keyIndex = 0; keyIndex < endKeyCount; keyIndex++)
	{
		//��Ԍ�
		const ModelFBX::Keyframe& endKeyframe0 = END_KEY_FRAMES.at(keyIndex);
		const ModelFBX::Keyframe& endKeyframe1 = END_KEY_FRAMES.at(keyIndex + 1);

		//��Ԍ�v�Z
		if (animeDatas_[END_INDEX].currentTime_ >= endKeyframe0.seconds &&
			animeDatas_[END_INDEX].currentTime_ < endKeyframe1.seconds
			|| (keyIndex + 1 == endKeyCount - 1 && animeDatas_[END_INDEX].currentTime_ <= endKeyframe1.seconds))
		{
			isEndCalc = true;
			//�L�[�t���[���Ԃ̊���
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
	//�L�[�t���[���Ԃɓ��Ă͂܂�Ȃ���Δ�����
	if (!isEndCalc)
	{
		return;
	}

	//���//�����Ńm�[�h�̃p�����[�^�ς��Ă�irate�Łj��������t���[�����Ƃɍs��ɂ��Ă�
	int32_t nodeCount = static_cast<int32_t>(startScales.size());
	//����
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

		//���`�⊮�����p�����[�^���擾
		Node& node = (*nodes_)[nodeIndex];
		node.scaling = Scale;
		node.rotation = Rotate;
		node.translation = Translate;
	}
}

void ObjectFBX::AnimationUpdate()
{
	if (!model_ || !model_->GetIsFbx())
	{
		return;
	}

	//�L�[�t���[���i�Ή������S�m�[�h�������j�̔z����擾
	const std::vector<ModelFBX::Keyframe>& KEY_FRAMES = dynamic_cast<ModelFBX*>(model_)->GetAnimations()[animeIndex_].keyframes;
	int32_t keyCount = static_cast<int32_t>(KEY_FRAMES.size());
	for (int32_t i = 0; i < keyCount - 1; i++)
	{
		//��ԑO
		const ModelFBX::Keyframe& KEY_FRAME0 = KEY_FRAMES[i];
		const ModelFBX::Keyframe& KEY_FRAME1 = KEY_FRAMES[i + 1];

		//��ԑO�v�Z(�C���f�b�N�X�̃L�[�t���[����,���̎��̃L�[�t���[���̊Ԃ��ǂ���)
		if (animeDatas_[animeIndex_].currentTime_ >= KEY_FRAME0.seconds &&
			animeDatas_[animeIndex_].currentTime_ < KEY_FRAME1.seconds)
		{
			//�L�[�t���[���Ԃ̊������v�Z
			float rate = (float)((animeDatas_[animeIndex_].currentTime_ - KEY_FRAME0.seconds) /
				(KEY_FRAME1.seconds - KEY_FRAME0.seconds));

			//�I�u�W�F�N�g�N���X�������Ă���m�[�h�S�ĂɓK�p
			int32_t nodeCount = static_cast<int32_t>(nodes_->size());
			for (int32_t j = 0; j < nodeCount; j++)
			{
				//�L�[�t���[��0��1�̃C���f�b�N�X�̃m�[�h���擾
				const ModelFBX::NodeKeyData& KEY_0 = KEY_FRAME0.nodeKeys[j];
				const ModelFBX::NodeKeyData& KEY_1 = KEY_FRAME1.nodeKeys[j];

				//�Q��
				auto& node = (*nodes_)[j];

				XMVECTOR scale1 = DirectX::XMLoadFloat3(&KEY_1.scale);
				XMVECTOR rotate1 = DirectX::XMLoadFloat4(&KEY_1.rotate);
				XMVECTOR translate1 = DirectX::XMLoadFloat3(&KEY_1.trans);

				XMVECTOR scale0 = DirectX::XMLoadFloat3(&KEY_0.scale);
				XMVECTOR rotate0 = DirectX::XMLoadFloat4(&KEY_0.rotate);
				XMVECTOR translate0 = DirectX::XMLoadFloat3(&KEY_0.trans);

				//���`�⊮�Ńp�����[�^�擾����
				XMVECTOR scale = (DirectX::XMVectorLerp(scale0, scale1, rate));
				XMVECTOR rotate = (DirectX::XMQuaternionSlerp(rotate0, rotate1, rate));
				XMVECTOR trans = (DirectX::XMVectorLerp(translate0, translate1, rate));
				//���
				node.scaling = scale;
				node.rotation = rotate;
				node.translation = trans;
			}
		}
	}
}

void ObjectFBX::UpdateFBXNodeMat()
{
	//�I�u�W�F�N�g�N���X�������Ă�fbx���f���̃m�[�h�S��
	for (Node& node : *nodes_)
	{
		//�s��̍쐬(�A�j���[�V�����Ȃǂŕω������p�����[�^���g�p)
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

		//�e�����邩(���炭�m�[�h�̏�̕����珇�ɓo�^����A�������Ă���̂ł���ł��ł���)
		DirectX::XMMATRIX parent_transform;
		if (node.parent != nullptr)
		{
			parent_transform = node.parent->globalTransform;
		}
		else
		{
			parent_transform = DirectX::XMMatrixIdentity();
		}

		//���f���̃m�[�h�̃��[���h
		DirectX::XMMATRIX world_transform = local_transform * parent_transform;

		//�s��ۑ�
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

	//�A�j���[�V�����̃��Z�b�g
	AnimationReset(animeIndex);

	//�Đ������
	animeDatas_[animeIndex].isPlay_ = true;
	//���[�v
	animeDatas_[animeIndex].isLoop_ = isLoop;
	//�t�Đ�
	animeDatas_[animeIndex].isReverse_ = isReverse;
}

void ObjectFBX::AnimationReset(int32_t animeIndex)
{
	if (model_ == nullptr || !model_->GetIsFbx())
	{
		return;
	}

	//�q�N���X�ɕϊ�
	ModelFBX* model = dynamic_cast<ModelFBX*>(model_);

	//�A�j���[�V������1�����Ȃ��O��
	FbxScene* fbxScene = model->GetFbxScene();
	//�A�j���[�V�����擾
	FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(animeIndex);
	//�A�j���[�V�����Ȃ�������
	if (animStack == nullptr) { return; }

	//�A�j���[�V�����̖��O�擾
	const char* ANIM_STACK_NAME = animStack->GetName();
	//�A�j���[�V�����̎��ԏ��
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(ANIM_STACK_NAME);

	//�J�n���Ԏ擾
	animeDatas_[animeIndex].currentTime_ = dynamic_cast<ModelFBX*>(model_)->GetAnimations()[animeIndex].startTime;
}

void ObjectFBX::PlayAnimation(bool isLoop, int32_t animeIndex)
{
	PlayAnimationInternal(animeIndex, isLoop);
}

void ObjectFBX::PlayReverseAnimation(bool isLoop, int32_t animeIndex)
{
	PlayAnimationInternal(isLoop, true);
}

//---------------------------------------------------------------------------
void ObjectFBX::CalcNodeMatBoneMatInternal(ModelFBX* model)
{
	//�Ƃ肠������Ԃ̃A�j���@��
	auto anime = model->GetAnimations()[animeIndex_];

	AnimationData& animeData = animeDatas_[animeIndex_];

	//�A�j���[�V����
	if (animeData.isPlay_)
	{
		ModelFBX* model = dynamic_cast<ModelFBX*>(model_);

		//�Ō�܂ōĐ�������
		if ((!animeData.isReverse_ && animeData.currentTime_ >= anime.endTime)
			|| (animeData.isReverse_ && animeData.currentTime_ <= anime.startTime))
		{
			if (!animeData.isReverse_)
			{
				//�擪�ɖ߂�
				animeData.currentTime_ = anime.startTime;
			}
			else
			{
				//�Ō�ɖ߂�
				animeData.currentTime_ = anime.endTime;
			}

			//�I��
			if (!animeData.isLoop_)
			{
				animeData.isPlay_ = false;
			}
		}

		else
		{
			//�t�Đ�
			if (animeData.isReverse_)
			{
				animeData.currentTime_ -= anime.addTime * animeData.animationSpeed_;
			}
			else
			{
				//1�t���[���i�߂�
				animeData.currentTime_ += anime.addTime * animeData.animationSpeed_;
			}

			//�͈͓��Ɏ��߂�
			animeData.currentTime_ = min(max(animeData.currentTime_, anime.startTime), anime.endTime);
		}
	}

	if (animeData.isPlay_)
	{
		//�A�j���[�V�����̃A�b�v�f�[�g
		BlendAnimationUpdate();
	}
	//�m�[�h�̍s��X�V
	UpdateFBXNodeMat();

	//�}�b�s���O
	MappingBoneData(model);
}

void ObjectFBX::MappingBoneData(ModelFBX* model)
{
	//�萔�o�b�t�@�փf�[�^�]��
	HRESULT result = constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin);
	for (int32_t i = 0; i < model->GetBoneNodeIndices().size(); i++)
	{
		//�s��v�Z
		constMapSkin->bones[i] = GetCalcSkinMat(model_, i);
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
	//�{�[���s�񂪂��������I
	if (!(model && model->GetIsFbx()))
	{
		return XMMatrixIdentity();
	}

	ModelFBX* modelL = dynamic_cast<ModelFBX*>(model);

	uint64_t indexL = modelL->GetBoneNodeIndices()[index];

	//�A�j���[�V�����ōX�V�������[���h
	DirectX::XMMATRIX worldTransform =
		(*nodes_)[indexL].globalTransform;
	//�����p���Ȃǂ��������s��
	DirectX::XMMATRIX offsetTransform =
		modelL->GetOffsetTransforms()[index];
	//�{�[���s��(��̓���|�����킹��)
	DirectX::XMMATRIX boneTransform =
		offsetTransform * worldTransform;

	////�X�P�[�����Ȃ�����
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

			ImGui::Checkbox("isPlay", &animeData.isPlay_);
			ImGui::Checkbox("isLoop", &animeData.isLoop_);
			ImGui::Checkbox("isReverse", &animeData.isReverse_);
			if (animeData.currentTime_ - anime.startTime > 0 && anime.endTime - anime.startTime > 0)
			{
				ImGui::Text("animationTimeRatio: %.2f", (animeData.currentTime_ - anime.startTime) / (anime.endTime - anime.startTime));
			}
		}
	};
	
	//�e�N���X��
	IObject3D::DrawImGui(f);
}

//------------------------------------------------------------------------------------------
void ObjectFBX::SetModel(IModel* model)
{
	//���ʂ̏�����
	IObject3D::SetModel(model);

	//�A�j���[�V�����̃f�[�^�擾
	if (model && model->GetIsFbx())
	{
		ModelFBX* modelL = dynamic_cast<ModelFBX*>(model);

		animeDatas_.clear();
		animeDatas_.resize(modelL->GetAnimations().size());

		//���Ńm�[�h�̃|�C���^�n��
		nodes_ = modelL->GetNodes();
	}
}
