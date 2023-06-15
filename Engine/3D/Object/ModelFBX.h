#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <fbxsdk.h>
#include "Material.h"


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

class ModelFBX
{
private:// �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImg = DirectX::ScratchImage;
	using string = std::string;
	template <class T> using vector = std::vector<T>;


public:
	//�t�����h�N���X
	friend class FbxLoader;

public://�萔
	//�{�[���C���f�b�N�X�i�e�����󂯂�{�[���j�̍ő吔
	static const int32_t S_MAX_BONE_INDICES_ = 4;//hlsl��float4�ɑΉ����邽��"4"

public://�T�u�N���X
	//���_�f�[�^�\����
	struct VertexPosNormalUvSkin
	{
		DirectX::XMFLOAT3 pos;//���W
		DirectX::XMFLOAT3 normal;//�@���x�N�g��
		DirectX::XMFLOAT2 uv;//uv���W
		uint32_t boneIndex[S_MAX_BONE_INDICES_] = { 0 };//�e�����󂯂�{�[���@�ԍ�
		float boneWeight[S_MAX_BONE_INDICES_] = { 1.0f,0,0,0 };//�{�[���@�d��
	};

	//�{�[���\����
	struct Bone
	{
		//���O
		std::string name_;
		//�����p���̋t�s��
		DirectX::XMMATRIX invInitialPose;
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
	//�m�[�h�s��
	std::vector<Node> nodes_;
	//���b�V�������m�[�h
	Node* meshNode_ = nullptr;
	//���_�f�[�^�z��
	std::vector<VertexPosNormalUvSkin> vertices_;
	//���_�C���f�b�N�X�z��
	std::vector<uint16_t> indices_;

	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	//�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff_;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff_;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV_;

	//�{�[���z��
	std::vector<Bone> bones_;

	//FBX�V�[��
	FbxScene* fbxScene_ = nullptr;

public:
	//�A���r�G���g�Ȃ�
	std::unique_ptr<Material> material_;


#pragma endregion

#pragma region �֐�
private:


public:
	~ModelFBX();

	//���f���̕ό`�s����擾
	const XMMATRIX& GetModelFBXTransform() { return meshNode_->globalTransform; }

	//�e��o�b�t�@����
	void CreateBuffers();

	//�`��
	void Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);

public:
	//getter
	std::vector<Bone>& GetBones() { return bones_; }

	FbxScene* GetFbxScene() { return fbxScene_; }

#pragma endregion
};

