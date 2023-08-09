#pragma once
#include"IModel.h"


struct Vertex
{
	DirectX::XMFLOAT3 pos;   //xyz���W
	DirectX::XMFLOAT3 normal;//�@���x�N�g��
	DirectX::XMFLOAT4 tangent;//�@���̐ڐ�
	DirectX::XMFLOAT2 uv;    //uv���W
};
//���_�f�[�^


class ModelObj : public IModel
{
private:
	//�Q�Ɛ�̃p�X�ꕔ�iobj��fbx���j
	static const std::string S_TYPE_DIRECTORY_;


private:
	//OBJ�t�@�C������3D���f����ǂݍ���
	void LoadFromOBJInternal(const std::string& folderName, bool smoothing = false);


	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void LoadTextures() override;

	//������(�p���̊֌W��)
	void InitializeInternal(const std::string& folderName, bool smoothing);

public:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

public://�ÓI�����o�֐�
	//OBJ�t�@�C������3D���f����ǂݍ���
	//"�t�H���_���̂�"���w�肷���mtl,obj,textuer��ǂ݂��ށi���ׂē������O�ł���΁j
	static std::unique_ptr<ModelObj> LoadFromOBJ(const std::string& folderName, bool smoothing = false);
};


