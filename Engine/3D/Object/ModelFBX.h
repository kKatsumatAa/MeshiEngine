#pragma once
#include"IModel.h"


class ModelFBX :public IModel
{
public:
	//�t�����h�N���X
	friend class FbxLoader;

private:
	//�Q�Ɛ�̃p�X(fbx��obj��)
	static const std::string S_TYPE_DIRECTORY_;

public://�T�u�N���X
	//�{�[���\����
	struct Bone
	{
		//���O
		std::string name_;
		//�����p���̋t�s��
		DirectX::XMMATRIX invInitialPose;
		//�m�[�h�̃O���[�o���g�����X�t�H�[��
		DirectX::XMMATRIX globalTransform;
		//�N���X�^�[(FBX���̃{�[�����)
		FbxCluster* fbxCluster;
		//�R���X�g���N�^
		Bone(const std::string& name)
		{
			name_ = name;
		}
	};


#pragma region �ϐ�
private:
	//���f����
	std::string name_;
	//�m�[�h�z��
	std::vector<Node> nodes_;
	//�{�[���z��
	std::vector<Bone> bones_;
	//FBX�V�[��
	FbxScene* fbxScene_ = nullptr;

	//�{�[���̍ő吔
	static const int32_t S_MAX_BONES_ = 90;
	//�萔�o�b�t�@�i�X�L���j
	ComPtr<ID3D12Resource> constBuffSkin_ = nullptr;

	//1�t���[���̎���
	FbxTime frameTime_;
	//�A�j���[�V�����J�n����
	FbxTime startTime_;
	//�A�j���[�V�����I������
	FbxTime endTime_;
	//�A�j���[�V�������ݎ���
	FbxTime currentTime_;
	//�A�j���[�V�����t���O
	bool isPlay_ = false;
	//���[�v
	bool isLoop_ = false;
	//�t�Đ�
	bool isReverse_ = false;
	//�Đ��̃X�s�[�h�{��
	float animationSpeed_ = 1.0f;

public:
	//�萔�o�b�t�@�p�f�[�^�\���́i�X�L�j���O�j
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[S_MAX_BONES_];
	};

#pragma endregion

#pragma region �֐�
private:
	void LoadTextures()override;

	//
	void SendingBoneData();

	void Initialize()override;

public:
	~ModelFBX();
	ModelFBX();


public:
	std::vector<Bone>& GetBones() { return bones_; }

	FbxScene* GetFbxScene() { return fbxScene_; }

public:
	//�A�j���[�V�����J�n
	virtual void PlayAnimationInternal(FbxTime& sTime, FbxTime& eTime,
		bool isLoop = false, bool isReverse = false);
	//�A�j���[�V�������Z�b�g
	virtual void AnimationReset(FbxTime& sTime, FbxTime& eTime);
	//�A�j���[�V�����J�n
	virtual void PlayAnimation(bool isLoop = false);
	virtual void PlayReverseAnimation(bool isLoop = false);
	//�A�j���[�V�����t���O
	virtual void SetIsPlayAnimation(bool isPlay) { isPlay_ = isPlay; }
	virtual void SetIsLoopAnimation(bool isLoop) { isLoop_ = isLoop; }
	virtual void SetIsReverseAnimation(bool isReverse) { isReverse_ = isReverse; }

	//�A�j���[�V�����X�s�[�h
	virtual void SetAnimationSpeed(float speed) { animationSpeed_ = speed; }

public:
	void Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex
		, const ConstBuffTransform& cbt) override;

	void DrawImGui(const std::function<void()>& childImguiF = nullptr)override;

#pragma endregion
};

