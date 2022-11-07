#pragma once
#include "Sprite.h"


//���\�[�X�ݒ�
extern D3D12_RESOURCE_DESC resDesc;
//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
extern ComPtr < ID3D12DescriptorHeap> srvHeap;
extern D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

extern ComPtr<ID3D12Resource> texBuff[512];



//SRV�̍ő��
const size_t kMaxSRVCount = 2056;
//�f�X�N���v�^�q�[�v�̐ݒ�
static D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {
srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
srvHeapDesc.NumDescriptors = kMaxSRVCount,
srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE//�V�F�[�_�[���猩����悤��
};

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

	//�}�e���A��
	Material material;


public:
	//"�t�H���_���̂�"���w�肷���mtl,obj,textuer��ǂ݂��ށi���ׂē������O�ł���΁j
	void CreateModel(const char* folderName);
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

};




void LoadGraph(const wchar_t* name, UINT64& textureHandle);