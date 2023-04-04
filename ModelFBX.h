#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>


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
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�t�����h�N���X
	friend class FbxLoader;

public://�T�u�N���X
	//���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos;//���W
		DirectX::XMFLOAT3 normal;//�@���x�N�g��
		DirectX::XMFLOAT2 uv;//uv���W
	};

#pragma region �ϐ�
private:
	//���f����
	std::string name;
	//�m�[�h�s��
	std::vector<Node> nodes;
	//���b�V�������m�[�h
	Node* mashNode = nullptr;
	//���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	//���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;

	// �A���r�G���g�e���x
	XMFLOAT3 ambient;	
	// �f�B�t���[�Y�e���x
	XMFLOAT3 diffuse;	
	//�e�N�X�`�����^�f�[�^
	DirectX::TexMetadata metaData = {};
	//�X�N���b�`�C���[�W
	DirectX::ScratchImage scratchImg = {};
	

public:

#pragma endregion

#pragma region �֐�
private:

public:
#pragma endregion
};

