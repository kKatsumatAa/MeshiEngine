#include "Model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>

//���\�[�X�ݒ�
 D3D12_RESOURCE_DESC resDesc;
//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
 ComPtr < ID3D12DescriptorHeap> srvHeap;
 D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

 ComPtr<ID3D12Resource> texBuff[512];

int count = 0;

void Model::CreateModel(const char* folderName)
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
			//�}�e���A���̃t�@�C�����ǂݍ���
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
			int count = 1;

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
				//�C���f�b�N�X�f�[�^�̒ǉ�
				if (count >= 4)
				{
					indicesM.emplace_back((unsigned short)indicesM.size() - 1 - indexCount);
					indicesM.emplace_back((unsigned short)indicesM.size() - 3 - indexCount);
					indicesM.emplace_back((unsigned short)indicesM.size() - 2 - indexCount);
					indexCount += 2;
					break;
				}
				else
					indicesM.emplace_back((unsigned short)indicesM.size() - indexCount);
				count++;
			}
		}
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
			LoadGraph(wchar, material.textureHandle);
		}
	}
	file.close();
}


void LoadGraph(const wchar_t* name, UINT64& textureHandle)
{
	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WIC�̃e�N�X�`���̃��[�h
	Directx::GetInstance().result = LoadFromWICFile(
		name,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(Directx::GetInstance().result));

	ScratchImage mipChain{};
	//mipmap����
	Directx::GetInstance().result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(Directx::GetInstance().result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// 04_03

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;						//���_�f�[�^�S�̂̃T�C�Y
	textureResourceDesc.Height = (UINT)metadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//�e�N�X�`���o�b�t�@�̐���
	//ID3D12Resource* texBuff = nullptr;
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&textureHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff[count]));
	assert(SUCCEEDED(Directx::GetInstance().result));

	//04_03
	// �S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		Directx::GetInstance().result = texBuff[count]->WriteToSubresource(
			(UINT)i,
			nullptr,//�S�̈�փR�s�[
			img->pixels,//���f�[�^�A�h���X
			(UINT)img->rowPitch,//1���C���T�C�Y
			(UINT)img->slicePitch//�S�T�C�Y
		);
		assert(SUCCEEDED(Directx::GetInstance().result));
	}
	////���f�[�^���
	//delete[] imageData;



	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	if (count == 0)
	{                                                          //desc�͐ݒ�
		Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(srvHeap.GetAddressOf()));
		assert(SUCCEEDED(Directx::GetInstance().result));
	}
	//SRV�q�[�v�̐擪�n���h�����擾
	if (count == 0)srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	else srvHandle.ptr += Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);

	//04_03
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;
	//�n���h���̂����ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	Directx::GetInstance().GetDevice()->CreateShaderResourceView(texBuff[count].Get(), &srvDesc, srvHandle);

	int count2 = count;
	count++;

	//04_02(�摜�\��p�̃A�h���X��������)
	//SRV�q�[�v�̐ݒ�R�}���h
	Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
	//SRV�q�[�v�̃n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	textureHandle = srvGpuHandle.ptr + (Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type) * count2);
}