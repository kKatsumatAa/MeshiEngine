#pragma once
#include"IObject3D.h"
#include <FbxLoader.h>
#include "LightManager.h"
#include "Mesh.h"


class ObjectFBX : public IObject3D
{
public:
	//�{�[���̍ő吔
	static const int32_t S_MAX_BONES_ = 90;
	//�萔�o�b�t�@�p�f�[�^�\���́i�X�L�j���O�j
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[S_MAX_BONES_];
	};

public:
	//�A�j���[�V�����f�[�^
	struct AnimationData
	{
		//�A�j���[�V�������ݎ���
		double currentTime_;
		//�A�j���[�V�����t���O
		bool isPlay_ = false;
		//���[�v
		bool isLoop_ = false;
		//�t�Đ�
		bool isReverse_ = false;
		//�Đ��̃X�s�[�h�{��
		float animationSpeed_ = 1.0f;
	};

private:
	// ���_���C�A�E�g�̐ݒ�
// ���_���C�A�E�g
	static D3D12_INPUT_ELEMENT_DESC sInputLayout_[7];
	//�p�C�v���C���Ȃǂ̐ݒ�
	static RootPipe pipelineSetM_[2];

protected:
	//�萔�o�b�t�@�i�X�L���j
	ComPtr<ID3D12Resource> constBuffSkin_ = nullptr;
	ConstBufferDataSkin* constMapSkin = nullptr;

private://fbx���f���n
	//�A�j���[�V����
	std::vector<AnimationData>animeDatas_;
	//�A�j���[�V�����̃C���f�b�N�X
	int32_t animeIndex_ = 0;
	std::vector<Node>* nodes_;

public://�֐�
	ObjectFBX();
	virtual ~ObjectFBX();

public:
	//���ʂ̏�����
	static void CommonInitialize();

private:
	//�}�e���A���A�{�[���s��Ȃǂ��Z�b�g
	void SetMaterialLightMTexSkinModel(uint64_t dissolveTexHandle, uint64_t specularMapTexhandle,
		uint64_t normalMapTexHandle);
	//�����`�揈��
	void DrawModelInternal(int32_t pipelineNum)override;

public:
	virtual void Update()override;
	//imgui
	void DrawImGui(std::function<void()>imguiF = NULL)override;

public:
	void SetModel(IModel* model)override;

public:
	//�A�j���[�V�����J�n
	void PlayAnimation(bool isLoop, int32_t animeIndex = 0);
	void PlayReverseAnimation(bool isLoop, int32_t animeIndex = 0);
	//�A�j���[�V�����t���O
	void SetIsPlayAnimation(bool isPlay, int32_t animeIndex = 0) { animeDatas_[animeIndex].isPlay_ = isPlay; }
	void SetIsLoopAnimation(bool isLoop, int32_t animeIndex = 0) { animeDatas_[animeIndex].isLoop_ = isLoop; }
	void SetIsReverseAnimation(bool isReverse, int32_t animeIndex = 0) { animeDatas_[animeIndex].isReverse_ = isReverse; }
	//�A�j���[�V�����X�s�[�h
	void SetAnimationSpeed(float speed) {
		animeDatas_[animeIndex_].animationSpeed_ = speed;
	}
	//�{�[���𓾂�
	const XMMATRIX* GetModelBones()const { return constMapSkin->bones; }
	//�I�u�W�F�N�g�N���X�������Ă�fbx���f���̃m�[�h
	const std::vector<Node>& GetNodes();
	//�{�[���̃f�[�^�]��
	void MappingBoneData(ModelFBX* model);

public:
	//�m�[�h�̍s����A�j���[�V�����ɍ��킹�ĕύX
	void BlendAnimationUpdate();
	//fbx�̃m�[�h�̍s��X�V
	void UpdateFBXNodeMat();
	//�A�j���[�V�����̃A�b�v�f�[�g
	void AnimationUpdate();

private:
	//�A�j���[�V�����J�n
	void PlayAnimationInternal(int32_t animeIndex,
		bool isLoop = false, bool isReverse = false);
	//�A�j���[�V�������Z�b�g
	void AnimationReset(int32_t animeIndex);

	//�A�j���[�V�����ŕω������m�[�h��{�[���̏���
	void CalcNodeMatBoneMatInternal(ModelFBX* model);
	//�{�[���̍s����v�Z
	XMMATRIX GetCalcSkinMat(IModel* model, int32_t index);

};