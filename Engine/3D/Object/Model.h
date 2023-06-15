#pragma once
#include "Sprite.h"
#include "Mesh.h"
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
	static const std::string S_BASE_DIRECTORY_;
	// �f�X�N���v�^�T�C�Y
	static uint32_t sDescriptorHandleIncrementSize_;

private:
	// ���O
	std::string name_;
	// ���b�V���R���e�i
	std::vector<std::unique_ptr<Mesh>> meshes_;
	// �}�e���A���R���e�i
	std::unordered_map<std::string, std::unique_ptr<Material>> materials_;
	// �f�t�H���g�}�e���A��
	std::unique_ptr<Material> defaultMaterial_;


private:
	//OBJ�t�@�C������3D���f����ǂݍ���
	void LoadFromOBJInternal(const std::string& folderName, const bool smoothing = false, bool modelType = false);

	/// <summary>
	/// �}�e���A���o�^
	/// </summary>
	void AddMaterial(std::unique_ptr<Material> material);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void LoadTextures();

public:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

public://�ÓI�����o�֐�
	//OBJ�t�@�C������3D���f����ǂݍ���
	//"�t�H���_���̂�"���w�肷���mtl,obj,textuer��ǂ݂��ށi���ׂē������O�ł���΁j
	static std::unique_ptr<Model> LoadFromOBJ(const std::string& folderName, bool smoothing = false, bool modelType = false);

public: // �����o�֐�
	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Model();

	static void StaticInitialize();


	/// <summary>
	/// ������
	/// </summary>
	/// <param name="modelname">���f����</param>
	/// <param name="modelname">�G�b�W�������t���O</param>
	void Initialize(const std::string& foldername, bool smoothing);

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList">���ߔ��s��R�}���h���X�g</param>
	void Draw(uint32_t indexNum = 1);
};


