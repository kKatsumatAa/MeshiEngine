#pragma once
#include"IModel.h"


struct Node
{
	//���O
	std::string name = {};
	//���[�J���X�P�[��
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	//���[�J����]�p
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	//���[�J���ړ�
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	//���[�J���ό`�s��
	DirectX::XMMATRIX transform = {};
	//�O���[�o���ό`�s��i�e�̉e�����܂߂��j
	DirectX::XMMATRIX globalTransform = {};
	//�e�m�[�h
	Node* parent = nullptr;
};

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
	//���b�V�������m�[�h
	Node* meshNode_ = nullptr;
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

	void Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex) override;

public:
	//getter
		//���f���̕ό`�s����擾
	const XMMATRIX& GetModelFBXTransform() { return meshNode_->globalTransform; }

	std::vector<Bone>& GetBones() { return bones_; }

	FbxScene* GetFbxScene() { return fbxScene_; }

	const Node& GetMeshNode() { return *meshNode_; }

#pragma endregion
};

