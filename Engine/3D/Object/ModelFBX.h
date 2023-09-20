#pragma once
#include"IModel.h"
#include"Vec3.h"
#include"Vec4.h"


class ModelFBX :public IModel
{
public:
	//�t�����h�N���X
	friend class FbxLoader;

private:
	//�Q�Ɛ�̃p�X(fbx��obj��)
	static const std::string S_TYPE_DIRECTORY_;

public://�T�u�N���X
	//�L�[�t���[�����Ƃ̏��i�X�P�[���Ȃǁj
	struct NodeKeyData
	{
		XMFLOAT3 scale;
		XMFLOAT4 rotate;
		XMFLOAT3 trans;
		template<class T>
		void serialize(T& archive)
		{
			archive(scale, rotate, trans);
		}
	};

	//�L�[�t���[�����Ƃ̃m�[�h�S�Ă̏��
	struct Keyframe
	{
		//�J�n���牽�b��
		float seconds;
		//�L�[�t���[���ɑΉ������S�Ẵm�[�h�̏��
		std::vector<NodeKeyData> nodeKeys;

		template<class T>
		void serialize(T& archive)
		{
			archive(seconds, nodeKeys);
		}
	};

	//�X�L�j���O�s��ɕK�v�ȍs��Z�b�g
	struct SkinningMatSet
	{
		//���b�V���m�[�h�̃O���[�o���g�����X�t�H�[��
		DirectX::XMMATRIX globalTransform;
		//�����p���̋t�s��
		DirectX::XMMATRIX invInitTransform;
	};

	//�A�j���[�V�������
	struct Animation
	{
		std::string name;
		//�A�j���[�V�����̒���
		float secondsLength;
		//fps
		float frameRate;
		//�J�n����
		float startTime;
		//�I������
		float endTime;
		//1�t���[�����Ƃɉ��Z���鎞��
		float addTime;
		//�L�[�t���[�����Ƃ̃m�[�h�S�Ă̏��̔z��
		std::vector<Keyframe> keyframes;

		template<class T>
		void serialize(T& archive)
		{
			archive(name, secondsLength, frameRate, keyframes);
		}
	};


private:
	//�X�L�j���O�s��ɕK�v�ȍs��̃Z�b�g�̔z��
	std::vector<SkinningMatSet> skinningMatSets_;
	//�{�[���̃m�[�h�̃C���f�b�N�X
	std::vector<uint64_t> boneNodeIndices_;
	std::vector<Animation>	animationClips_;
	//�{�[���̃C���f�b�N�X�Ȃǂ����߂邽�߂Ɏg��

#pragma region �ϐ�
private:
	//���f����
	std::string name_;
	//�m�[�h�z��
	std::vector<Node> nodes_;
	//FBX�V�[��
	FbxScene* fbxScene_ = nullptr;


#pragma endregion

#pragma region �֐�
private:
	void LoadTextures()override;

	void Initialize()override;

public:
	~ModelFBX();


public:
	const std::vector<Node>& GetNodes() { return nodes_; }
	//�m�[�h�𖼑O�Ŏw�肵�Ď擾
	const Node* GetNode(const std::string& name);

	FbxScene* GetFbxScene() { return fbxScene_; }

	uint64_t GetBoneIndex(const std::string& name) const;


	std::vector<SkinningMatSet>& GetSkinningMatSets() { return skinningMatSets_; }
	std::vector<uint64_t>& GetBoneNodeIndices() { return boneNodeIndices_; }
	std::vector<Animation>& GetAnimations() { return animationClips_; }

#pragma endregion
};

