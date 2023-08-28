#include "ModelObj.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>
#include <algorithm>

using namespace DirectX;


//�Q�Ɛ�̃p�X�ꕔ�iobj��fbx���j
const std::string ModelObj::S_TYPE_DIRECTORY_ = "obj/";


std::unique_ptr<ModelObj> ModelObj::LoadFromOBJ(const std::string& folderName, bool smoothing)
{
	//�V����Model�^�̃C���X�^���X�̃��������m��
	std::unique_ptr<ModelObj> model = std::make_unique<ModelObj>();
	model->InitializeInternal(folderName, smoothing);

	//���L���n��
	return std::move(model);
}

void ModelObj::InitializeInternal(const std::string& folderName, bool smoothing)
{
	//���f���ǂݍ���
	LoadFromOBJInternal(folderName, smoothing);

	//�p�������������֐�
	IModel::Initialize();
}

void ModelObj::LoadTextures()
{
	std::string directoryPathL = IModel::S_BASE_DIRECTORY_ + S_TYPE_DIRECTORY_ + name_ + "/";

	IModel::LoadTexturesInternal(directoryPathL);
}

void ModelObj::LoadFromOBJInternal(const std::string& folderName, bool smoothing)
{
	const std::string FILE_NAME = folderName + ".obj";
	const std::string DIRECTORY_PATH = IModel::S_BASE_DIRECTORY_ + S_TYPE_DIRECTORY_ + folderName + "/";

	// �t�@�C���X�g���[��
	std::ifstream file;
	// .obj�t�@�C�����J��
	file.open(DIRECTORY_PATH + FILE_NAME);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	name_ = folderName;

	// ���b�V������
	meshes_.emplace_back(std::move(std::make_unique <Mesh>()));
	//���L������������^����
	std::unique_ptr<Mesh> mesh = std::move(meshes_.back());
	int32_t indexCount = 0;

	std::vector<Vec3> positions;	// ���_���W
	std::vector<Vec3> normals;	// �@���x�N�g��
	std::vector<Vec2> texcoords;	// �e�N�X�`��UV
	// 1�s���ǂݍ���
	std::string line;
	while (getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		getline(line_stream, key, ' ');

		//�}�e���A��
		if (key == "mtllib")
		{
			// �}�e���A���̃t�@�C�����ǂݍ���
			std::string filename;
			line_stream >> filename;
			// �}�e���A���ǂݍ���
			LoadMaterial(DIRECTORY_PATH, filename);
		}
		// �擪������g�Ȃ�O���[�v�̊J�n
		if (key == "g") {

			// �J�����g���b�V���̏�񂪑����Ă���Ȃ�
			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
				// ���_�@���̕��ςɂ��G�b�W�̕�����
				if (smoothing) {
					mesh->CalculateSmoothedVertexNormals();
				}
				//���b�V���̐ڐ�
				mesh->CalculateTangent();

				//���L���߂�
				meshes_[meshes_.size() - 1] = std::move(mesh);

				// ���̃��b�V������
				meshes_.emplace_back(std::move(std::make_unique<Mesh>()));
				mesh = std::move(meshes_.back());
				indexCount = 0;
			}

			// �O���[�v���ǂݍ���
			std::string groupName;
			line_stream >> groupName;

			// ���b�V���ɖ��O���Z�b�g
			mesh->SetName(groupName);
		}
		// �擪������v�Ȃ璸�_���W
		if (key == "v") {
			// X,Y,Z���W�ǂݍ���
			Vec3 position{};
			line_stream >> position.x_;
			line_stream >> position.y_;
			line_stream >> position.z_;
			//�t�����ɓǂݍ��܂ꂿ�Ⴄ�̂�
			position.z_ *= -1.0f;
			positions.emplace_back(position);
		}
		// �擪������vt�Ȃ�e�N�X�`��
		if (key == "vt")
		{
			// U,V�����ǂݍ���
			Vec2 texcoord{};
			line_stream >> texcoord.x_;
			line_stream >> texcoord.y_;
			// V�������]
			texcoord.y_ = 1.0f - texcoord.y_;
			// �e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);
		}
		// �擪������vn�Ȃ�@���x�N�g��
		if (key == "vn") {
			// X,Y,Z�����ǂݍ���
			Vec3 normal{};
			line_stream >> normal.x_;
			line_stream >> normal.y_;
			line_stream >> normal.z_;
			//�t�����ɓǂݍ��܂ꂿ�Ⴄ�̂�
			normal.z_ *= -1.0f;
			// �@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}
		// �擪������usemtl�Ȃ�}�e���A�������蓖�Ă�
		if (key == "usemtl")
		{
			if (mesh->GetMaterial() == nullptr) {
				// �}�e���A���̖��ǂݍ���
				std::string materialName;
				line_stream >> materialName;

				// �}�e���A�����Ō������A�}�e���A�������蓖�Ă�
				auto itr = materials_.find(materialName);
				if (itr != materials_.end()) {
					mesh->SetMaterial(itr->second.get());
				}
			}
		}
		// �擪������f�Ȃ�|���S���i�O�p�`�j
		if (key == "f")
		{
			int indexNum = 0;
			std::vector<uint16_t> indices;

			//���p�X�y�[�X��؂�ōs��ǂݍ���
			std::string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				std::istringstream index_stream(index_string);
				unsigned short indexPos, indexNormal, indexUV;
				//���W�Ȃǂ�ϐ��ɓǂݍ���
				index_stream >> indexPos;
				index_stream.seekg(1, std::ios_base::cur);
				index_stream >> indexUV;
				index_stream.seekg(1, std::ios_base::cur);
				index_stream >> indexNormal;

				Mesh::VertexPosNormalUvSkin vertex{};
				//���W�Ȃǂ��Z�b�g
				vertex.pos = positions[indexPos - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexUV - 1];
				vertex.boneWeight[0] = 1.0f;
				//���b�V����o�^
				mesh->AddVertex(vertex);

				//�X���[�W���O
				if (smoothing) {
					mesh->AddSmoothData(indexPos, (unsigned short)mesh->GetVertexCount() - 1);
				}
				//�l�p���b�V����������[2,3,0]�̏��œ����
				if (indexNum >= 3) {
					indices.emplace_back((unsigned short)(indexCount - 1));
					indices.emplace_back((unsigned short)indexCount);
					indices.emplace_back((unsigned short)(indexCount - 3));
				}
				else
				{
					indices.emplace_back((unsigned short)indexCount);
				}
				indexNum++;
				indexCount++;
			}
			// �C���f�b�N�X�f�[�^�̒ǉ�
			//�O�p���b�V��
			if (indices.size() == 3)
			{
				mesh->AddIndex(indices[0]);
				mesh->AddIndex(indices[2]);
				mesh->AddIndex(indices[1]);
			}
			//�l�p���b�V����������
			else {
				mesh->AddIndex(indices[0]);
				mesh->AddIndex(indices[2]);
				mesh->AddIndex(indices[1]);
				mesh->AddIndex(indices[3]);
				mesh->AddIndex(indices[5]);
				mesh->AddIndex(indices[4]);
			}
		}
	}
	file.close();

	// ���_�@���̕��ςɂ��G�b�W�̕�����
	if (smoothing) {
		mesh->CalculateSmoothedVertexNormals();
	}

	//���b�V���̐ڐ�
	mesh->CalculateTangent();

	//���L���߂�
	meshes_[meshes_.size() - 1] = std::move(mesh);
}

void ModelObj::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	// �t�@�C���X�g���[��
	std::ifstream file;
	// �}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	std::unique_ptr<Material> material;

	// 1�s���ǂݍ���
	std::string line;
	while (getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		getline(line_stream, key, ' ');

		// �擪�̃^�u�����͖�������
		if (key[0] == '\t') {
			key.erase(key.begin()); // �擪�̕������폜
		}

		// �擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl") {

			// ���Ƀ}�e���A���������
			if (material.get()) {
				// �}�e���A�����R���e�i�ɓo�^
				AddMaterial(std::move(material));
			}

			// �V�����}�e���A���𐶐�
			material = Material::Create();
			// �}�e���A�����ǂݍ���
			line_stream >> material->name_;
		}
		// �擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka") {
			line_stream >> material->ambient_.x;
			line_stream >> material->ambient_.y;
			line_stream >> material->ambient_.z;
		}
		// �擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd") {
			line_stream >> material->diffuse_.x;
			line_stream >> material->diffuse_.y;
			line_stream >> material->diffuse_.z;
		}
		// �擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks") {
			line_stream >> material->specular_.x;
			line_stream >> material->specular_.y;
			line_stream >> material->specular_.z;
		}
		// �擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd") {
			//�e�N�X�`���t�@�C�����ǂݍ���
			line_stream >> material->textureFilename_;

			// �t���p�X����t�@�C���������o��
			//'\\'�ȊO���擾
			material->textureFilename_ = MySubstr(material->textureFilename_, '\\');

			//'/'�ȊO���擾
			material->textureFilename_ = MySubstr(material->textureFilename_, '/');
		}
	}
	// �t�@�C�������
	file.close();

	if (material.get()) {
		// �}�e���A����o�^�i���L����n���j
		AddMaterial(std::move(material));
	}
}