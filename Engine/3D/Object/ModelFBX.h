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


#pragma endregion

#pragma region �֐�
private:
	void LoadTextures()override;

	void Initialize()override;

public:
	~ModelFBX();


public:
	std::vector<Bone>& GetBones() { return bones_; }

	const std::vector<Node>& GetNodes() { return nodes_; }
	//�m�[�h�𖼑O�Ŏw�肵�Ď擾
	const Node* GetNode(const std::string& name);
	//�m�[�h��

	FbxScene* GetFbxScene() { return fbxScene_; }

	int32_t GetBoneIndex(const std::string& name) const;

#pragma endregion
};

