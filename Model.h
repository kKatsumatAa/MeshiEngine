#pragma once
#include "Sprite.h"
#include <unordered_map>


struct Vertex
{
	XMFLOAT3 pos;   //xyz���W
	XMFLOAT3 normal;//�@���x�N�g��
	XMFLOAT2 uv;    //uv���W
};
//���_�f�[�^


class Model
{
private:
	// ���_�@���X���[�W���O�p�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData_;

	//�}�e���A��
	struct Material
	{
		std::string name;//�}�e���A����
		XMFLOAT3 ambient;//�A���r�G���g�e���x
		XMFLOAT3 diffuse;//�f�B�t���[�Y�V
		XMFLOAT3 specular;//�X�y�L�����[�V
		float alpha;//�A���t�@
		std::string textureFilename;//�e�N�X�`���t�@�C����
		UINT64 textureHandle;

		//�R���X�g���N�^
		Material()
		{
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

	struct ConstBufferDataMaterial2
	{
		XMFLOAT3 ambient;
		float pad1;       //�p�f�B���O
		XMFLOAT3 diffuse;
		float pad2;       //�p�f�B���O
		XMFLOAT3 specular;
		float alpha;
	};

public:
	//���f���p
//���_�f�[�^�z��
	std::vector<Vertex> verticesM;
	//���_�C���f�b�N�X�z��
	std::vector<unsigned short> indicesM;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuffM;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuffM;
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbViewM{};
	D3D12_INDEX_BUFFER_VIEW ibViewM{};

	//AL4_02_02
	ComPtr <ID3D12Resource> constBuffMaterial2 = nullptr;
	ConstBufferDataMaterial2* constMapMaterial2 = nullptr;

	//�}�e���A��
	Material material;


private:
	//OBJ�t�@�C������3D���f����ǂݍ���
	void LoadFromOBJInternal(const char* folderName, const bool smoothing = false);
	//�e��o�b�t�@����
	void CreateBuffers();

public:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

public://�ÓI�����o�֐�
	//OBJ�t�@�C������3D���f����ǂݍ���
	//"�t�H���_���̂�"���w�肷���mtl,obj,textuer��ǂ݂��ށi���ׂē������O�ł���΁j
	static Model* LoadFromOBJ(const char* folderName, const bool smoothing = false);

	/// <summary>
/// ���_�f�[�^�̐����擾
/// </summary>
/// <returns>���_�f�[�^�̐�</returns>
	inline size_t GetVertexCount() { return verticesM.size(); }

	/// <summary>
/// �G�b�W�������f�[�^�̒ǉ�
/// </summary>
/// <param name="indexPosition">���W�C���f�b�N�X</param>
/// <param name="indexVertex">���_�C���f�b�N�X</param>
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	/// <summary>
	/// ���������ꂽ���_�@���̌v�Z
	/// </summary>
	void CalculateSmoothedVertexNormals();
};