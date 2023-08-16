#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "Material.h"
#include <vector>
#include <unordered_map>
#include "Camera.h"
#include "ConstBuffTransform.h"
#include "WorldMat.h"


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


/// <summary>
/// �`��f�[�^
/// </summary>
class Mesh
{
public:
	//�t�����h�N���X
	friend class FbxLoader;

private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;


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
		DirectX::XMFLOAT4 tangent;//�@���̐ڐ�
	};

public: // �ÓI�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* device);

private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* sDevice_;

public: // �����o�֐�
	Mesh();


	/// <summary>
	/// ���O���擾
	/// </summary>
	/// <returns>���O</returns>
	const std::string& GetName() { return name_; }

	/// <summary>
	/// ���O���Z�b�g
	/// </summary>
	/// <param name="name_">���O</param>
	void SetName(const std::string& name);

	/// <summary>
	/// ���_�f�[�^�̒ǉ�
	/// </summary>
	/// <param name="vertex">���_�f�[�^</param>
	void AddVertex(const VertexPosNormalUvSkin& vertex);

	/// <summary>
	/// ���_�C���f�b�N�X�̒ǉ�
	/// </summary>
	/// <param name="index">�C���f�b�N�X</param>
	void AddIndex(uint16_t index);

	void PopIndex();

	/// <summary>
	/// ���_�f�[�^�̐����擾
	/// </summary>
	/// <returns>���_�f�[�^�̐�</returns>
	inline size_t GetVertexCount() { return vertices_.size(); }

	/// <summary>
	/// �G�b�W�������f�[�^�̒ǉ�
	/// </summary>
	/// <param name="indexPosition">���W�C���f�b�N�X</param>
	/// <param name="indexVertex">���_�C���f�b�N�X</param>
	void AddSmoothData(uint16_t indexPosition, uint16_t indexVertex);

	/// <summary>
	/// ���������ꂽ���_�@���̌v�Z
	/// </summary>
	void CalculateSmoothedVertexNormals();

	//���b�V���̐ڐ����v�Z
	void CalculateTangent();

	/// <summary>
	/// �}�e���A���̎擾
	/// </summary>
	/// <returns>�}�e���A��</returns>
	Material* GetMaterial() { return material_; }

	/// <summary>
	/// �}�e���A���̊��蓖��
	/// </summary>
	/// <param name="material">�}�e���A��</param>
	void SetMaterial(Material* material);

	/// <summary>
	/// �o�b�t�@�̐���
	/// </summary>
	void CreateBuffers();

	void SendingMat(Vec3 materialExtend, const ConstBuffTransform& cbt);

	/// <summary>
	/// ���_�o�b�t�@�擾
	/// </summary>
	/// <returns>���_�o�b�t�@</returns>
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }

	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�擾
	/// </summary>
	/// <returns>�C���f�b�N�X�o�b�t�@</returns>
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList">���ߔ��s��R�}���h���X�g</param>
	void Draw(Vec3 materialExtend, const ConstBuffTransform& cbt,
		const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);

public:
	/// <summary>
	/// ���_�z����擾
	/// </summary>
	/// <returns></returns>
	inline const std::vector<VertexPosNormalUvSkin>& GetVertices() { return vertices_; }

	/// <summary>
	/// �C���f�b�N�X�z����擾
	/// </summary>
	/// <returns></returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }


private: // �����o�ϐ�
	// ���O
	std::string name_;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff_;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUvSkin> vertices_;
	// ���_�C���f�b�N�X�z��
	std::vector<uint16_t> indices_;
	// ���_�@���X���[�W���O�p�f�[�^
	std::unordered_map<uint16_t, std::vector<uint16_t>> smoothData_;
	// �}�e���A��
	Material* material_ = nullptr;

	//���b�V�������m�[�h(fbx)
	Node* meshNode_ = nullptr;

	//�O���[�o���ό`�s��i�e�̉e�����܂߂��j
	DirectX::XMMATRIX globalTransform_ = {};

	ConstBuffTransform cbt_;

public:
	//getter
//���f���̕ό`�s����擾
	const XMMATRIX& GetMeshFBXTransform() { return meshNode_->globalTransform; }

	const Node& GetMeshNode() { return *meshNode_; }

	//setter
	void SetMeshNode(Node* node) { meshNode_ = node; }
};
