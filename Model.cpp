#include "Model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>

void Model::LoadFromOBJInternal(const char* folderName, const bool smoothing)
{
	//�t�@�C���X�g���[��
	std::ifstream file;
	//
	const std::string modelname = folderName;
	const std::string filename = modelname + ".obj";//"�t�@�C����.obj"
	const std::string directoryPath = "Resources/" + modelname + "/";// "Resources/�t�H���_��/"
	//.obj�t�@�C�����J��
	file.open(directoryPath + filename);// "Resources/�t�H���_��/�t�@�C����.obj"
	//�t�@�C���I�[�v�����s���`�F�b�N
	assert(file.is_open());

	std::stringstream all;
	//�t�@�C���̓��e�𕶎���X�g���[���ɃR�s�[
	all << file.rdbuf();

	//�t�@�C�������
	file.close();


	//vertex�p
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;

	//1�s���̕����������ϐ�
	std::string line;

	int indexCount = 0;

	while (std::getline(all, line))
	{
		//��s���̕�������X�g���[���ɕϊ����ĉ�͂��₷��
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		getline(line_stream, key, ' ');



		//�擪������"mtllib"�Ȃ�}�e���A��
		if (key == "mtllib")
		{
			//�}�e���A���̃t�@�C�����ǂݍ��݁E�E
			std::string filename;
			line_stream >> filename;
			//�}�e���A���ǂݍ���
			LoadMaterial(directoryPath, filename);
		}
		//'v'�Ȃ璸�_���W
		if (key == "v")
		{
			//x,y,z���W�ǂݍ���
			XMFLOAT3 pos{};
			line_stream >> pos.x;
			line_stream >> pos.y;
			line_stream >> pos.z;
			//���W�f�[�^�ɒǉ�
			positions.emplace_back(pos);
		}
		//�e�N�X�`��
		if (key == "vt")
		{
			//UV�����ǂݍ���
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V�������]
			texcoord.y = 1.0f - texcoord.y;
			//�e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);
		}
		//�@���x�N�g��
		if (key == "vn")
		{
			//X,Y,Z�����ǂݍ���
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//�@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}
		//�|���S��
		if (key == "f")
		{
			int count = 0;

			//���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			std::string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				//���_�C���f�b�N�X����̕�������X�g���[���ɕϊ����ĉ�͂��₷��
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				//���_�C���f�b�N�X���ɒǉ�(���_�C���f�b�N�X�A�@���Auv�̏�)
				//(1// 2// 3//�@��("//")�ɂ͔�Ή�)
				index_stream.seekg(1, std::ios_base::cur);//�X���b�V��(/)���΂�
				index_stream >> indexTexcoord;
				index_stream.seekg(1, std::ios_base::cur);//�X���b�V�����΂�
				index_stream >> indexNormal;

				//���_�f�[�^�̒ǉ�
				Vertex vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				verticesM.emplace_back(vertex);

				//�G�b�W�������p�̃f�[�^��ǉ�
				if (smoothing)
				{
					//v�L�[�i���W�f�[�^�j�̔ԍ��ƁA�S�č����������_�̃C���f�b�N�X���Z�b�g�œo�^����
					AddSmoothData(indexPosition, (unsigned short)GetVertexCount() - 1);
				}

				//�C���f�b�N�X�f�[�^�̒ǉ�
				count++;

				indicesM.emplace_back((unsigned short)indicesM.size() - indexCount);
				if (count >= 4)
				{
					indicesM.emplace_back((unsigned short)indicesM.size() - 4 - indexCount);
					indicesM.emplace_back((unsigned short)indicesM.size() - 3 - indexCount);
					indexCount += 2;
					count = 0;
					break;
				}

			}
		}
	}
	//���_�@���̕��ςɂ��G�b�W�̕�����
	if (smoothing)
	{
		CalculateSmoothedVertexNormals();
	}
}

void Model::CreateBuffers()
{
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};

	//AL4_02_02
	{
		ResourceProperties(cbResourceDesc,
			((UINT)sizeof(ConstBufferDataMaterial2) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/);
		//�萔�o�b�t�@�̐���
		BuffProperties(cbHeapProp, cbResourceDesc, &constBuffMaterial2);
		//�萔�o�b�t�@�̃}�b�s���O
		Directx::GetInstance().result = constBuffMaterial2->Map(0, nullptr, (void**)&constMapMaterial2);//�}�b�s���O
		assert(SUCCEEDED(Directx::GetInstance().result));
		//�}�e���A�����Z�b�g(���t���[���ς��l�Ȃ��̂���Ȃ��̂ŁAupdate�ɓ���Ȃ�)
		constMapMaterial2->ambient = material.ambient;
		constMapMaterial2->diffuse = material.diffuse;
		constMapMaterial2->specular = material.specular;
		constMapMaterial2->alpha = material.alpha;

		constBuffMaterial2->Unmap(0, nullptr);
	}


	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * verticesM.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indicesM.size());

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// ���_�o�b�t�@����
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuffM));
	assert(SUCCEEDED(Directx::GetInstance().result));


	// ���_�o�b�t�@�ւ̃f�[�^�]��
	Vertex* vertMap = nullptr;
	Directx::GetInstance().result = vertBuffM->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(Directx::GetInstance().result)) {
		std::copy(verticesM.begin(), verticesM.end(), vertMap);
		vertBuffM->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbViewM.BufferLocation = vertBuffM->GetGPUVirtualAddress();
	vbViewM.SizeInBytes = sizeVB;
	vbViewM.StrideInBytes = sizeof(Vertex);

	// ���\�[�X�ݒ�
	resourceDesc.Width = sizeIB;

	// �C���f�b�N�X�o�b�t�@����
	BuffProperties(heapProps, resourceDesc, &indexBuffM);

	// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	Directx::GetInstance().result = indexBuffM->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(Directx::GetInstance().result)) {

		std::copy(indicesM.begin(), indicesM.end(), indexMap);

		indexBuffM->Unmap(0, nullptr);
	}

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibViewM.BufferLocation = indexBuffM->GetGPUVirtualAddress();
	ibViewM.Format = DXGI_FORMAT_R16_UINT;
	ibViewM.SizeInBytes = sizeIB;
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	//�t�@�C���X�g���[��
	std::ifstream file;
	//�}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) assert(0);

	//��s���ǂݍ���
	std::string line;
	while (std::getline(file, line))
	{
		//��s���̕�������X�g���[���ɕϊ�
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		std::getline(line_stream, key, ' ');

		//�擪��tab�����͖���
		if (key[0] == '\t')
		{
			key.erase(key.begin());
		}

		//�擪������"newmtl"�Ȃ�}�e���A����
		if (key == "newmtl")
		{
			//�}�e���A�����ǂݍ���
			line_stream >> material.name;
		}
		//"Ka"�Ȃ�A���r�G���g�F
		if (key == "Ka")
		{
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}
		//"Kd"�Ȃ�f�B�t���[�Y�F
		if (key == "Kd")
		{
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}
		//"Ks"�Ȃ�X�y�L�����[�F
		if (key == "Ks")
		{
			line_stream >> material.specular.x;
			line_stream >> material.specular.y;
			line_stream >> material.specular.z;
		}
		//"map_Kd"�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd")
		{
			//�e�N�X�`���t�@�C�����ǂݍ���
			line_stream >> material.textureFilename;
			//�e�N�X�`���ǂݍ���
			std::string nameS = directoryPath + material.textureFilename;
			//
			const char* name = nameS.c_str();
			wchar_t wchar[128];
			size_t size = _countof(wchar);
			mbstowcs_s(&size, wchar, name, size);
			TextureManager::GetInstance().LoadGraph(wchar, material.textureHandle);
		}
	}
	file.close();
}


Model* Model::LoadFromOBJ(const char* folderName, const bool smoothing)
{
	//�V����Model�^�̃C���X�^���X�̃��������m��
	Model* model = new Model();
	//�ǂݍ���
	model->LoadFromOBJInternal(folderName, smoothing);
	//�o�b�t�@����
	model->CreateBuffers();

	return model;
}

void Model::AddSmoothData(unsigned short indexPosition, unsigned short indexVertex)
{
	smoothData_[indexPosition].emplace_back(indexVertex);
}

void Model::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData_.begin();
	for (; itr != smoothData_.end(); itr++)
	{
		//�e�ʗp�̋��ʒ��_�R���N�V����
		std::vector<unsigned short>& v = itr->second;
		//�S���_�̖@���𕽋ς���
		XMVECTOR normal = {};
		for (unsigned short index : v)
		{
			normal += XMVectorSet(verticesM[index].normal.x, verticesM[index].normal.y, verticesM[index].normal.z, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());
		//���ʖ@�����g�p���邷�ׂẴf�[�^�ɏ�������
		for (unsigned short index : v)
		{
			verticesM[index].normal = { normal.m128_f32[0],normal.m128_f32[1] ,normal.m128_f32[2] };
		}
	}
}
