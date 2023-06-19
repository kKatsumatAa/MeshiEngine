#include "Model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>
#include <algorithm>

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
const std::string Model::S_BASE_DIRECTORY_ = "Resources/";
uint32_t Model::sDescriptorHandleIncrementSize_ = 0;


void Model::LoadFromOBJInternal(const std::string& folderName, bool smoothing/*, bool modelType*/)
{
	const std::string FILE_NAME = folderName + ".obj";
	const std::string DIRECTORY_PATH = S_BASE_DIRECTORY_ + folderName + "/";

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
	meshes_.emplace_back(std::move(std::make_unique < Mesh>()));
	//���L������������^����
	std::unique_ptr<Mesh> mesh = std::move(meshes_.back());
	int32_t indexCount = 0;

	std::vector<XMFLOAT3> positions;	// ���_���W
	std::vector<XMFLOAT3> normals;	// �@���x�N�g��
	std::vector<XMFLOAT2> texcoords;	// �e�N�X�`��UV
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
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//�t�����ɓǂݍ��܂ꂿ�Ⴄ�̂�
			//position.x *= -1.0f;
			position.z *= -1.0f;
			positions.emplace_back(position);
		}
		// �擪������vt�Ȃ�e�N�X�`��
		if (key == "vt")
		{
			// U,V�����ǂݍ���
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			// V�������]
			texcoord.y = 1.0f - texcoord.y;
			// �e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);
		}
		// �擪������vn�Ȃ�@���x�N�g��
		if (key == "vn") {
			// X,Y,Z�����ǂݍ���
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//�t�����ɓǂݍ��܂ꂿ�Ⴄ�̂�
			//normal.x *= -1.0f;
			normal.z *= -1.0f;
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

	//���L���߂�
	meshes_[meshes_.size() - 1] = std::move(mesh);
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
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
			size_t pos1;
			pos1 = material->textureFilename_.rfind('\\');
			if (pos1 != std::string::npos) {
				material->textureFilename_ = material->textureFilename_.substr(pos1 + 1, material->textureFilename_.size() - pos1 - 1);
			}

			pos1 = material->textureFilename_.rfind('/');
			if (pos1 != std::string::npos) {
				material->textureFilename_ = material->textureFilename_.substr(pos1 + 1, material->textureFilename_.size() - pos1 - 1);
			}

		}
	}
	// �t�@�C�������
	file.close();

	if (material.get()) {
		// �}�e���A����o�^�i���L����n���j
		AddMaterial(std::move(material));
	}
}

void Model::AddMaterial(std::unique_ptr<Material> material)
{
	// �R���e�i�ɓo�^
	materials_.emplace(material->name_, std::move(material));
}

void Model::LoadTextures()
{
	std::string directoryPath = S_BASE_DIRECTORY_ + name_ + "/";

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	for (auto& m : materials_) {
		Material* material = m.second.get();
		srvGpuHandle.ptr = material->textureHandle_;

		// �e�N�X�`������
		if (material->textureFilename_.size() > 0) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV; cpuDescHandleSRV.ptr = 0;
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV; gpuDescHandleSRV.ptr = 0;
			// �}�e���A���Ƀe�N�X�`���ǂݍ���
			material->LoadTexture(directoryPath, cpuDescHandleSRV, gpuDescHandleSRV);
		}
		// �e�N�X�`���Ȃ�
		else {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV; cpuDescHandleSRV.ptr = 0;
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV; gpuDescHandleSRV.ptr = 0;
			// �}�e���A���Ƀe�N�X�`���ǂݍ���
			material->LoadTexture(S_BASE_DIRECTORY_, cpuDescHandleSRV, gpuDescHandleSRV);
		}
	}
}


std::unique_ptr<Model> Model::LoadFromOBJ(const std::string& folderName, bool smoothing, bool modelType)
{
	//�V����Model�^�̃C���X�^���X�̃��������m��
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(folderName, smoothing);

	//���L���n��
	return std::move(model);
}


Model::~Model()
{
	meshes_.clear();
	materials_.clear();
}

void Model::StaticInitialize()
{
	Mesh::StaticInitialize(DirectXWrapper::GetInstance().GetDevice());
}

void Model::Initialize(const std::string& foldername, bool smoothing)
{
	// ���f���ǂݍ���
	LoadFromOBJInternal(foldername.c_str(), smoothing);

	// ���b�V���̃}�e���A���`�F�b�N
	for (auto& m : meshes_) {
		// �}�e���A���̊��蓖�Ă��Ȃ�
		if (m && m->GetMaterial() == nullptr) {
			if (defaultMaterial_ == nullptr) {
				// �f�t�H���g�}�e���A���𐶐�
				defaultMaterial_ = Material::Create();
				defaultMaterial_->name_ = "no material";

				// �f�t�H���g�}�e���A�����Z�b�g(���b�V���ɓn���̂̓|�C���^�̂݁i���L���͓n���Ȃ��j)
				m->SetMaterial(defaultMaterial_.get());

				//�ǉ��i���L���n���j
				materials_.emplace(defaultMaterial_->name_, std::move(defaultMaterial_));
			}
			//// �f�t�H���g�}�e���A�����Z�b�g(���b�V���ɓn���̂̓|�C���^�̂݁i���L���͓n���Ȃ��j)
			//m->SetMaterial(defaultMaterial_.get());
		}
	}

	// ���b�V���̃o�b�t�@����
	for (auto& m : meshes_) {
		if (m) {
			m->CreateBuffers();
		}
	}
	// �}�e���A���̐��l��萔�o�b�t�@�ɔ��f
	for (auto& m : materials_) {
		m.second->Update();
	}

	// �e�N�X�`���̓ǂݍ���
	LoadTextures();
}

void Model::Draw(uint32_t indexNum)
{
	// �f�X�N���v�^�q�[�v�̔z��
	if (TextureManager::GetInstance().sSrvHeap_) {
		ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::GetInstance().sSrvHeap_.Get() };
		DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(indexNum, ppHeaps);
	}

	// �S���b�V����`��
	for (auto& mesh : meshes_) {
		mesh->Draw(DirectXWrapper::GetInstance().GetCommandList());
	}
}