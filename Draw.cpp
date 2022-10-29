#include "Draw.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>


//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
ComPtr < ID3D12DescriptorHeap> srvHeap = nullptr;
D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
int count = 0;

ComPtr<ID3D12Resource> texBuff[512];

//SRV�̍ő��
const size_t kMaxSRVCount = 2056;
//�f�X�N���v�^�q�[�v�̐ݒ�
static D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {
srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
srvHeapDesc.NumDescriptors = kMaxSRVCount,
srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE//�V�F�[�_�[���猩����悤��
};
//���\�[�X�ݒ�
D3D12_RESOURCE_DESC resDesc{};

//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
D3D12_INDEX_BUFFER_VIEW ibView{};
//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
D3D12_INDEX_BUFFER_VIEW ibView2{};
D3D12_INDEX_BUFFER_VIEW ibView3{};

static unsigned short indices[6] =
{
	0,1,2,//�O�p�`1��
	2,1,3,//�O�p�`2��
};
static unsigned short indices2[3] =
{
	0,1,2//�O�p�`2��
};
static unsigned short indicesCube[36] =
{
	//�O
	0,1,2,//�O�p�`1��
	2,1,3,//�O�p�`2��
	//��
	6,5,4,//�O�p�`1��
	7,5,6,//�O�p�`2��
	//��
	10,9,8,//�O�p�`1��
	11,9,10,//�O�p�`2��
	//��
	12,13,14,//�O�p�`1��
	14,13,15,//�O�p�`2��
	//��
	16,17,18,//�O�p�`1��
	18,17,19,//�O�p�`2��
	//�E
	22,21,20,//�O�p�`1��
	23,21,22,//�O�p�`2��
};
static unsigned short indicesCircle[] =
{
	2,1,0,
	3,2,0,
	4,3,0,
	5,4,0,
	6,5,0,
	7,6,0,
	8,7,0,
	9,8,0,
	10,9,0,
	11,10,0,
	12,11,0,
	13,12,0,
	14,13,0,
	15,14,0,
	16,15,0,
	17,16,0,
	18,17,0,
	19,18,0,
	20,19,0,
	21,20,0,
	22,21,0,
	23,22,0,
};

static unsigned short indicesLine[2] =
{
	0,1//�O�p�`2��
};


//�����Ȑ}�`�p
Vertex vertices[24] = {
	//��O
	{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//����
	{{-5.0f,5.0f, -5.0f},{},{0.0f,0.0f}},//����
	{{5.0f,-5.0f, -5.0f},{},{1.0f,1.0f}},//�E��
	{{5.0f,5.0f,  -5.0f},{},{1.0f,0.0f}},//�E��
	//��
	{{-5.0f,-5.0f,5.0f},{},{0.0f,1.0f}},//����
	{{-5.0f,5.0f, 5.0f},{},{0.0f,0.0f}},//����
	{{5.0f,-5.0f, 5.0f},{},{1.0f,1.0f}},//�E��
	{{5.0f,5.0f,  5.0f},{},{1.0f,0.0f}},//�E��
	//��
	{{5.0f,5.0f,-5.0f},{},{0.0f,1.0f}},//����
	{{5.0f,5.0f, 5.0f},{},{0.0f,0.0f}},//����
	{{-5.0f,5.0f, -5.0f},{},{1.0f,1.0f}},//�E��
	{{-5.0f,5.0f, 5.0f},{},{1.0f,0.0f}},//�E��
	//��
	{{5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//����
	{{5.0f,-5.0f, 5.0f},{},{0.0f,0.0f}},//����
	{{-5.0f,-5.0f, -5.0f},{},{1.0f,1.0f}},//�E��
	{{-5.0f,-5.0f, 5.0f},{},{1.0f,0.0f}},//�E��
	//��
	{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//����
	{{-5.0f,-5.0f, 5.0f},{},{0.0f,0.0f}},//����
	{{-5.0f,5.0f, -5.0f},{},{1.0f,1.0f}},//�E��
	{{-5.0f,5.0f,  5.0f},{},{1.0f,0.0f}},//�E��
	//�E
	{{5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//����
	{{5.0f,-5.0f, 5.0f},{},{0.0f,0.0f}},//����
	{{5.0f,5.0f, -5.0f},{},{1.0f,1.0f}},//�E��
	{{5.0f,5.0f,  5.0f},{},{1.0f,0.0f}},//�E��
};

//�X�v���C�g�p
VertexSprite verticesS[4];

//����
Vertex verticesSphere[2 + 34 * 36];
// ���_�o�b�t�@�r���[�̍쐬
D3D12_VERTEX_BUFFER_VIEW vbView2{};
const int PAPA = 66 * 3 + 6;
//���_�o�b�t�@�̐���
ComPtr<ID3D12Resource> vertBuff2 = nullptr;
static unsigned short indicesSphere[PAPA * 36];

//�f�X�N���v�^�����W�̐ݒ�
D3D12_DESCRIPTOR_RANGE descriptorRange;
//�e�N�X�`��

// ���_���C�A�E�g
D3D12_INPUT_ELEMENT_DESC inputLayout[3] = {
{//xyz���W
 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
}, // (1�s�ŏ������ق������₷��)

	{//�@���x�N�g��
 "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
}, // (1�s�ŏ������ق������₷��)

{//uv���W
 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
} // (1�s�ŏ������ق������₷��)
};

//sprite
D3D12_INPUT_ELEMENT_DESC inputLayoutSprite[2] = {
	{//xyz���W
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1�s�ŏ������ق������₷��)

	{//uv���W
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	} // (1�s�ŏ������ق������₷��)
};
SpriteSet pipelineSet;

//���[�g�p�����[�^�̐ݒ�
D3D12_ROOT_PARAMETER rootParams[3] = {};
UINT sizeVB;

// �p�C�v�����X�e�[�g�̐���
ComPtr < ID3D12PipelineState> pipelineState[3] = { nullptr };
// ���[�g�V�O�l�`��
ComPtr<ID3D12RootSignature> rootSignature;
// �O���t�B�b�N�X�p�C�v���C���ݒ�
D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
ID3DBlob* vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
ID3DBlob* psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
ID3DBlob* errorBlob = nullptr; // �G���[�I�u�W�F�N�g

//���_�o�b�t�@�̐ݒ�
D3D12_HEAP_PROPERTIES heapProp{};
// 2.�`���̕ύX
	// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;




void DrawInitialize()
{
	

	//���̗p
	{
		// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
		sizeVB = static_cast<UINT>(sizeof(verticesSphere[0]) * (_countof(verticesSphere)));

		//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

		ResourceProperties(resDesc, sizeVB);

		//���_�o�b�t�@�̐���
		BuffProperties(heapProp, resDesc, vertBuff2.GetAddressOf());

		// ���_�o�b�t�@�r���[�̍쐬
		// GPU���z�A�h���X
		vbView2.BufferLocation = vertBuff2->GetGPUVirtualAddress();
		// ���_�o�b�t�@�̃T�C�Y
		vbView2.SizeInBytes = sizeVB;
		// ���_1���̃f�[�^�T�C�Y
		vbView2.StrideInBytes = sizeof(verticesSphere[0]);


		WorldMat worldMat;
		Vec3 vec={ 0,-5.0f,0 };

		//���_���
		verticesSphere[0] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };//��
		verticesSphere[1] = { {vec.x,-vec.y,vec.z},{},{1.0f,0.0f} };//��

		for (int i = 0; i < 36; i++)//��
		{
			worldMat.rot.y = (float)i * AngletoRadi(360.0f / 35.0f);


			for (int j = 0; j < 34; j++)//�c
			{
				worldMat.rot.x = ((float)(j+1) * (pi / 35.0f));
				worldMat.SetWorld();
				vec = { 0,-5.0f,0 };
				Vec3xM4(vec, worldMat.matWorld, false);

				int p = i * 34 + j;
				verticesSphere[(2) + i * 34 + j ] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };
			}
		}

		int count = 0;
		int count2 = 0;
		int count3 = 0;
		int count4 = 0;
		bool isLast = false;
		//�C���f�b�N�X
		for (int i = 0; i < _countof(indicesSphere); i++)
		{
			if (i % (PAPA * 35) == 0 && i != 0)//�Ō�̏c�̗�
			{
				isLast = true;
			}

			if (i % PAPA == 0 || i % (PAPA * (count + 1) - 3) == 0)
			{

			}
			else if (1)//��ԉ����ザ��Ȃ����
			{
				if (count2 % 2 == 0)
				{
					if (isLast)
					{
						indicesSphere[i] = 2 + 34 * count + (count3);
						indicesSphere[i + 1] = 2 + 0 + (count3);//������Ă����̂ň�ԍŏ��̗���g��
						indicesSphere[i + 2] = 2 + 34 * count + (count3 + 1);

						count3++;
						i += 2;
					}
					else
					{
						indicesSphere[i] = 2 + 34 * count + (count3);
						indicesSphere[i + 1] = 2 + 34 * (count + 1) + (count3);
						indicesSphere[i + 2] = 2 + 34 * count + (count3 + 1);

						count3++;
						i += 2;
					}
				}
				else if (count2 % 2 == 1)
				{
					if (isLast)
					{
						indicesSphere[i] = 2 + 0 + (count4 + 1);//������Ă����̂ň�ԍŏ��̗���g��
						indicesSphere[i + 1] = 2 + 34 * count + (count4 + 1);
						indicesSphere[i + 2] = 2 + 0 + (count4);//������Ă����̂ň�ԍŏ��̗���g��

						count4++;
						i += 2;
					}
					else
					{
						indicesSphere[i] = 2 + 34 * (count + 1) + (count4 + 1);
						indicesSphere[i + 1] = 2 + 34 * count + (count4 + 1);
						indicesSphere[i + 2] = 2 + 34 * (count + 1) + (count4);

						count4++;
						i += 2;
					}
				}

				count2++;
			}
			if (i % PAPA == 0 || i % (PAPA * (count + 1) - 3) == 0)
			{
				if (i % PAPA == 0)//��ԉ��̎O�p�`
				{
					if (isLast)
					{
						indicesSphere[i] = 0;
						indicesSphere[i + 1] = 2 + 0;
						indicesSphere[i + 2] = 2 + 34 * count;

						i += 2;
					}
					else
					{
						indicesSphere[i] = 2 + 34 * (count + 1);
						indicesSphere[i + 1] = 2 + 34 * count;
						indicesSphere[i + 2] = 0;

						i += 2;
					}
				}
				else if (i % (PAPA * (count + 1) - 3) == 0)//��ԏ�̎O�p�`
				{
					if (isLast)
					{
						indicesSphere[i] = 1 + 34 * (count + 1);
						indicesSphere[i + 1] = 35;
						indicesSphere[i + 2] = 1;

						i += 2;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
					else
					{
						indicesSphere[i] = 1 + 34 * (count + 1);
						indicesSphere[i + 1] = 1 + 34 * (count + 2);
						indicesSphere[i + 2] = 1;

						i += 2;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
				}
			}
			/*if (i % 198 == 0)
			{
				count++;
				count2 = 0;
				count3 = 0;
				count4 = 0;
			}

			if (count2 % 2 == 0)
			{
				indicesSphere[i] = 35 * count - (count3 + 1);
				indicesSphere[i + 1] = 35 * (count + 1) - count3;
				indicesSphere[i + 2] = 35 * count - (count3);

				count3++;
				i += 2;
			}
			else if (count2 % 2 == 1)
			{
				indicesSphere[i] = 35 * (count + 1) - (count4);
				indicesSphere[i + 1] = 35 * count - (count4 + 1);
				indicesSphere[i + 2] =  35 * (count + 1) - (count4 + 1);

				count4++;
				i += 2;*/
				//}

		}

		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesSphere));

		//���\�[�X�ݒ�
		ResourceProperties(resDesc, sizeIB);
		//�C���f�b�N�X�o�b�t�@�̍쐬
		ID3D12Resource* indexBuff = nullptr;//GPU���̃�����
		BuffProperties(heapProp, resDesc, &indexBuff);
		//�C���f�b�N�X�o�b�t�@���}�b�s���O
		uint16_t* indexMap = nullptr;
		Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//�S�C���f�b�N�X�ɑ΂���
		for (int i = 0; i < _countof(indicesSphere); i++)
		{
			indexMap[i] = indicesSphere[i];//�C���f�b�N�X���R�s�[

		}
		//�}�b�s���O������
		indexBuff->Unmap(0, nullptr);

		//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
		ibView3.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView3.Format = DXGI_FORMAT_R16_UINT;
		ibView3.SizeInBytes = sizeIB;
	}


	//�f�X�N���v�^�����W�̐ݒ�
	descriptorRange.NumDescriptors = 100;   //��x�̕`��Ɏg���e�N�X�`���̖���
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;  //�e�N�X�`�����W�X�^0��(t0)
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//���[�g�p�����[�^�̐ݒ�
	//�萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0;//�萔�o�b�t�@�ԍ�(b0)
	rootParams[0].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//�f�X�N���v�^
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;//�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//�V��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams[2].Descriptor.ShaderRegister = 1;//�萔�o�b�t�@�ԍ�(b1)
	rootParams[2].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����


	// �p�C�v�����X�e�[�g�̐���
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineState->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob);

	PipeLineState(D3D12_FILL_MODE_WIREFRAME, (pipelineState + 1)->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob);

	//line
	PipeLineState(D3D12_FILL_MODE_WIREFRAME, (pipelineState + 2)->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob, LINE);

	//sprite�p
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSet.pipelineState.GetAddressOf(),
		pipelineSet.rootSignature.GetAddressOf(), pipelineSet.vsBlob, 
		pipelineSet.psBlob, SPRITE);

	//03_04
	//�C���f�b�N�X�f�[�^
	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	//int p= _countof(indicesCube);
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesCircle));

	//���\�[�X�ݒ�
	ResourceProperties(resDesc, sizeIB);
	//�C���f�b�N�X�o�b�t�@�̍쐬
	ID3D12Resource* indexBuff = nullptr;//GPU���̃�����
	ID3D12Resource* indexBuff2 = nullptr;//GPU���̃�����
	BuffProperties(heapProp, resDesc, &indexBuff);
	BuffProperties(heapProp, resDesc, &indexBuff2);
	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	uint16_t* indexMap2 = nullptr;
	Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	Directx::GetInstance().result = indexBuff2->Map(0, nullptr, (void**)&indexMap2);
	//�S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < _countof(indicesCube); i++)
	{
		indexMap[i] = indicesCube[i];//�C���f�b�N�X���R�s�[

	}
	for (int i = 0; i < _countof(indicesCircle); i++)
	{
		indexMap2[i] = indicesCircle[i];//�C���f�b�N�X���R�s�[
	}
	//�}�b�s���O������
	indexBuff->Unmap(0, nullptr);
	indexBuff2->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	ibView2.BufferLocation = indexBuff2->GetGPUVirtualAddress();
	ibView2.Format = DXGI_FORMAT_R16_UINT;
	ibView2.SizeInBytes = sizeIB;

	//06_03
	/*if (indexNum == SPHERE)*/
	{
		for (int i = 0; i < _countof(indicesSphere) / 3; i++)
		{//�O�p�`����ƂɌv�Z
			//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
			unsigned short index0 = indicesSphere[i * 3 + 0];
			unsigned short index1 = indicesSphere[i * 3 + 1];
			unsigned short index2 = indicesSphere[i * 3 + 2];
			//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
			XMVECTOR p0 = XMLoadFloat3(&verticesSphere[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesSphere[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesSphere[index2].pos);
			//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//�O�ρi�����ȃx�N�g���j
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//���߂��@���𒸓_�f�[�^�ɑ��
			XMStoreFloat3(&verticesSphere[index0].normal, normal);
			XMStoreFloat3(&verticesSphere[index1].normal, normal);
			XMStoreFloat3(&verticesSphere[index2].normal, normal);
		}
	}
}

Draw::Draw()
{
	//sprite�p
	{
		UINT sizeVB;
		D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
		// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
		sizeVB = static_cast<UINT>(sizeof(verticesS[0]) * 4.0);
		//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

		ResourceProperties(resDesc, sizeVB);
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		//���_�o�b�t�@�̐���
		BuffProperties(heapProp, resDesc, vertBuffS.GetAddressOf());

		// ���_�o�b�t�@�r���[�̍쐬
		// GPU���z�A�h���X
		vbViewS.BufferLocation = vertBuffS.Get()->GetGPUVirtualAddress();
		// ���_�o�b�t�@�̃T�C�Y
		vbViewS.SizeInBytes = sizeVB;
		// ���_1���̃f�[�^�T�C�Y
		vbViewS.StrideInBytes = sizeof(verticesS[0]);//kokogamatigatteita(vertices[0]ni!)
	}

	//�s��
	cbt.Initialize(Directx::GetInstance());


	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
	sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p
	
	ResourceProperties(resDesc, sizeVB);

	//���_�o�b�t�@�̐���
	BuffProperties(heapProp, resDesc, &vertBuff);

	// ���_�o�b�t�@�r���[�̍쐬
	// GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	// ���_1���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]);

	//03_02
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	ResourceProperties(cbResourceDesc,
		((UINT)sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/);
	//�萔�o�b�t�@�̐���
	BuffProperties(cbHeapProp, cbResourceDesc, &constBuffMaterial);
	//�萔�o�b�t�@�̃}�b�s���O
	Directx::GetInstance().result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//�}�b�s���O
	assert(SUCCEEDED(Directx::GetInstance().result));
	//constBuffTransfer({ 1.0f,1.0f,1.0f,1.0f });
	
	{
		//04_02
		////�������s�N�Z����
		//const size_t textureWidth = 256;
		////�c�����s�N�Z����
		//const size_t textureHeight = 256;
		////�z��̗v�f��
		//const size_t imageDataCount = textureWidth * textureHeight;
		////�摜�C���[�W�f�[�^�z��
		//XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];//���ƂŕK������I�I
		////�S�s�N�Z���̐F��������
		//for (size_t i = 0; i < imageDataCount; i++)
		//{
		//	imageData[i].x = 1.0f;//R
		//	imageData[i].y = 0.0f;//G
		//	imageData[i].z = 0.0f;//B
		//	imageData[i].w = 1.0f;//A
		//}




		////05_02
		//{
		//	//�q�[�v�ݒ�
		//	D3D12_HEAP_PROPERTIES cbHeapProp{};
		//	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
		//	//���\�[�X�ݒ�
		//	D3D12_RESOURCE_DESC cbResourceDesc{};
		//	ResourceProperties(cbResourceDesc, (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/);
		//	//�萔�o�b�t�@�̐���
		//	BuffProperties(cbHeapProp, cbResourceDesc, &constBuffTransform);
		//}
		////�萔�o�b�t�@�̃}�b�s���O
		//Directx::GetInstance().result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);//�}�b�s���O
		//assert(SUCCEEDED(Directx::GetInstance().result));
		//�P�ʍs�����
		//SetNormDigitalMat(constMapTransform->mat, win);

		////05_03
		////���s���e�ϊ��i�X�v���C�g�`��?�j
		//constMapTransform->mat =
		//	XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().window_width, WindowsApp::GetInstance().window_height, 0.0, 0.0f, 1.0f);


		////05_04
		//ViewMat view ( 0.0f, 0.0f, -100.0f
		//	, 0.0f, 0.0f, 0.0f
		//	, 0.f, 1.f, 0.f );

		////�萔�o�b�t�@�ɓ]��
		//constMapTransform->mat = view.matView * matProjection;

		//05_05
		/*XMMATRIX matWorld;
		matWorld = XMMatrixIdentity();

		constMapTransform=matWorld**/
		/*constMapMaterial->color = { 0,0,0,1 };*/

		//06_03
		//������̂����Ń`�J�`�J����I�iDraw �̃C���X�^���X����������邽�тɁA�@�����㏑������邹���I�j
		///*else if (indexNum == TRIANGLE)*/
		//{
		//	for (int i = 0; i < _countof(indices2) / 3; i++)
		//	{//�O�p�`����ƂɌv�Z
		//		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		//		unsigned short index0 = indices2[i * 3 + 0];
		//		unsigned short index1 = indices2[i * 3 + 1];
		//		unsigned short index2 = indices2[i * 3 + 2];
		//		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		//		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		//		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		//		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//		//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
		//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//		//�O�ρi�����ȃx�N�g���j
		//		XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
		//		//���߂��@���𒸓_�f�[�^�ɑ��
		//		XMStoreFloat3(&vertices[index0].normal, normal);
		//		XMStoreFloat3(&vertices[index1].normal, normal);
		//		XMStoreFloat3(&vertices[index2].normal, normal);
		//	}
		//}
		///*else if (indexNum == BOX)*/
		//{
		//	for (int i = 0; i < _countof(indices) / 3; i++)
		//	{//�O�p�`����ƂɌv�Z
		//		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		//		unsigned short index0 = indices[i * 3 + 0];
		//		unsigned short index1 = indices[i * 3 + 1];
		//		unsigned short index2 = indices[i * 3 + 2];
		//		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		//		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		//		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		//		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//		//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
		//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//		//�O�ρi�����ȃx�N�g���j
		//		XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
		//		//���߂��@���𒸓_�f�[�^�ɑ��
		//		XMStoreFloat3(&vertices[index0].normal, normal);
		//		XMStoreFloat3(&vertices[index1].normal, normal);
		//		XMStoreFloat3(&vertices[index2].normal, normal);
		//	}
		//}
		///*else if (indexNum == CUBE)*/
		//{
		//	for (int i = 0; i < _countof(indicesCube) / 3; i++)
		//	{//�O�p�`����ƂɌv�Z
		//		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		//		unsigned short index0 = indicesCube[i * 3 + 0];
		//		unsigned short index1 = indicesCube[i * 3 + 1];
		//		unsigned short index2 = indicesCube[i * 3 + 2];
		//		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		//		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		//		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		//		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//		//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
		//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//		//�O�ρi�����ȃx�N�g���j
		//		XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
		//		//���߂��@���𒸓_�f�[�^�ɑ��
		//		XMStoreFloat3(&vertices[index0].normal, normal);
		//		XMStoreFloat3(&vertices[index1].normal, normal);
		//		XMStoreFloat3(&vertices[index2].normal, normal);
		//	}
		//}
		///*else if (indexNum == CIRCLE)*/
		//{
		//	for (int i = 0; i < _countof(indicesCircle) / 3; i++)
		//	{//�O�p�`����ƂɌv�Z
		//		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		//		unsigned short index0 = indicesCircle[i * 3 + 0];
		//		unsigned short index1 = indicesCircle[i * 3 + 1];
		//		unsigned short index2 = indicesCircle[i * 3 + 2];
		//		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		//		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		//		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		//		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//		//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
		//		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//		//�O�ρi�����ȃx�N�g���j
		//		XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
		//		//���߂��@���𒸓_�f�[�^�ɑ��
		//		XMStoreFloat3(&vertices[index0].normal, normal);
		//		XMStoreFloat3(&vertices[index1].normal, normal);
		//		XMStoreFloat3(&vertices[index2].normal, normal);
		//	}
		//}
		///*else if (indexNum == LINE)*/
		//{
		//	//���߂��@���𒸓_�f�[�^�ɑ��
		//	vertices[0].normal = XMFLOAT3(-1.0f / 3.0f, 1.0f / 3.0f, -1.0f / 3.0f);//���̐����Ȃ̂Ō�ŕύX
		//	vertices[1].normal = XMFLOAT3(-1.0f / 3.0f, 1.0f / 3.0f, -1.0f / 3.0f);//�V

		//}
	}
}

void Draw::CreateModel(const char* fileName)
{
	//�t�@�C���X�g���[��
	std::ifstream file;
	//.obj�t�@�C�����J��
	file.open(fileName);
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

	while (std::getline(all,line))
	{
		//��s���̕�������X�g���[���ɕϊ����ĉ�͂��₷��
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		getline(line_stream, key, ' ');

		//�擪������'v'�Ȃ璸�_���W
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
				indicesM.emplace_back((unsigned short)indicesM.size());
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
	Directx::GetInstance().result = Directx::GetInstance().device->CreateCommittedResource(
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
	vbViewM.StrideInBytes = sizeof(vertices[0]);

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
	////���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC textureResourceDesc{};
	//textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//textureResourceDesc.Width = textureWidth;						//���_�f�[�^�S�̂̃T�C�Y
	//textureResourceDesc.Height = textureHeight;
	//textureResourceDesc.DepthOrArraySize = 1;
	//textureResourceDesc.MipLevels = 1;
	//textureResourceDesc.SampleDesc.Count = 1;
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
	Directx::GetInstance().result = Directx::GetInstance().device->CreateCommittedResource(
		&textureHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff[count]));
	assert(SUCCEEDED(Directx::GetInstance().result));

	////�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	//Directx::GetInstance().result = texBuff->WriteToSubresource(
	//	0,
	//	nullptr,//�S�̈�փR�s�[
	//	imageData,//���f�[�^�A�h���X
	//	sizeof(XMFLOAT4) * textureWidth,//1���C���T�C�Y
	//	sizeof(XMFLOAT4) * imageDataCount//�S�T�C�Y
	//);

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
		Directx::GetInstance().result = Directx::GetInstance().device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(srvHeap.GetAddressOf()));
		assert(SUCCEEDED(Directx::GetInstance().result));
	}
	//SRV�q�[�v�̐擪�n���h�����擾
	if (count == 0)srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	else srvHandle.ptr += Directx::GetInstance().device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//srvDesc.Shader4ComponentMapping =
	//	D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	//srvDesc.Texture2D.MipLevels = 1;
	////�n���h���̂����ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	//device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

	//04_03
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;
	//�n���h���̂����ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	Directx::GetInstance().device->CreateShaderResourceView(texBuff[count].Get(), &srvDesc, srvHandle);

	int count2 = count;
	count++;

	//04_02(�摜�\��p�̃A�h���X��������)
	//SRV�q�[�v�̐ݒ�R�}���h
	Directx::GetInstance().commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
	//SRV�q�[�v�̃n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	textureHandle = srvGpuHandle.ptr + (Directx::GetInstance().device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type) * count2);
}

void Draw::Update(const int& indexNum, const int& pipelineNum, const UINT64 textureHandle, const ConstBuffTransform& constBuffTransform,
	const bool& primitiveMode)
{
	worldMat->SetWorld();

	//�ϊ��s���GPU�ɑ��M
	if (worldMat->parent != nullptr && indexNum != SPRITE)//�e������ꍇ
	{
		worldMat->matWorld *= worldMat->parent->matWorld;
		XMMATRIX matW;
		matW = { (float)worldMat->matWorld.m[0][0],(float)worldMat->matWorld.m[0][1],(float)worldMat->matWorld.m[0][2],(float)worldMat->matWorld.m[0][3],
				 (float)worldMat->matWorld.m[1][0],(float)worldMat->matWorld.m[1][1],(float)worldMat->matWorld.m[1][2],(float)worldMat->matWorld.m[1][3],
				 (float)worldMat->matWorld.m[2][0],(float)worldMat->matWorld.m[2][1],(float)worldMat->matWorld.m[2][2],(float)worldMat->matWorld.m[2][3],
				 (float)worldMat->matWorld.m[3][0],(float)worldMat->matWorld.m[3][1],(float)worldMat->matWorld.m[3][2],(float)worldMat->matWorld.m[3][3] };

		cbt.constMapTransform->mat = matW * view->matView * projection->matProjection;
	}
	else if(indexNum != SPRITE)//�e�����Ȃ��ꍇ
	{
		XMMATRIX matW;
		matW = { (float)worldMat->matWorld.m[0][0],(float)worldMat->matWorld.m[0][1],(float)worldMat->matWorld.m[0][2],(float)worldMat->matWorld.m[0][3],
				 (float)worldMat->matWorld.m[1][0],(float)worldMat->matWorld.m[1][1],(float)worldMat->matWorld.m[1][2],(float)worldMat->matWorld.m[1][3],
				 (float)worldMat->matWorld.m[2][0],(float)worldMat->matWorld.m[2][1],(float)worldMat->matWorld.m[2][2],(float)worldMat->matWorld.m[2][3],
				 (float)worldMat->matWorld.m[3][0],(float)worldMat->matWorld.m[3][1],(float)worldMat->matWorld.m[3][2],(float)worldMat->matWorld.m[3][3] };

		cbt.constMapTransform->mat = matW * view->matView * projection->matProjection;
	}

	

	if (indexNum != SPHERE && indexNum != SPRITE)
	{
		// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
		Vertex* vertMap = nullptr;
		Directx::GetInstance().result = vertBuff->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(Directx::GetInstance().result));
		// �S���_�ɑ΂���
		for (int i = 0; i < _countof(vertices); i++) {
			vertMap[i] = vertices[i]; // ���W���R�s�[
		}
		// �q���������
		vertBuff->Unmap(0, nullptr);
	}
	else//���̂̎�
	{
		// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
		Vertex* vertMap = nullptr;
		Directx::GetInstance().result = vertBuff2->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(Directx::GetInstance().result));
		// �S���_�ɑ΂���
		for (int i = 0; i < _countof(verticesSphere); i++) {

			vertMap[i] = verticesSphere[i]; // ���W���R�s�[

		}
		// �q���������
		vertBuff2->Unmap(0, nullptr);
	}
	
	// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	Directx::GetInstance().commandList->RSSetViewports(1, &WindowsApp::GetInstance().viewport);

	// �V�U�[��`
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // �؂蔲�����W��
	scissorRect.right = (LONG)(scissorRect.left + WindowsApp::GetInstance().window_width); // �؂蔲�����W�E
	scissorRect.top = 0; // �؂蔲�����W��
	scissorRect.bottom = (LONG)(scissorRect.top + WindowsApp::GetInstance().window_height); // �؂蔲�����W��
	// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	Directx::GetInstance().commandList->RSSetScissorRects(1, &scissorRect);

	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	if (indexNum == LINE)
	{
		Directx::GetInstance().commandList->SetPipelineState(pipelineState[2].Get());
	}
	else if (indexNum == SPRITE)
	{
		Directx::GetInstance().commandList->SetPipelineState(pipelineSet.pipelineState.Get());
	}
	else
	{
		Directx::GetInstance().commandList->SetPipelineState(pipelineState[pipelineNum].Get());
	}

	if (indexNum == SPRITE)
	{
		Directx::GetInstance().commandList.Get()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());
	}
	else if (indexNum != SPRITE)
	{
		Directx::GetInstance().commandList->SetGraphicsRootSignature(rootSignature.Get());
	}

	if (indexNum != SPHERE)
	{
		if (indexNum == SPRITE)
		{
			// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
			VertexSprite* vertMap = nullptr;
			Directx::GetInstance().result = vertBuffS->Map(0, nullptr, (void**)&vertMap);
			assert(SUCCEEDED(Directx::GetInstance().result));
			// �S���_�ɑ΂���
			for (int i = 0; i < 4; i++) {
				vertMap[i] = verticesS[i]; // ���W���R�s�[
			}
			// �q���������
			vertBuffS->Unmap(0, nullptr);

			Directx::GetInstance().commandList.Get()->IASetVertexBuffers(0, 1, &vbViewS);


			Directx::GetInstance().commandList.Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		}
		else if (indexNum == MODEL)
		{
			Directx::GetInstance().commandList.Get()->IASetVertexBuffers(0, 1, &vbViewM);
			Directx::GetInstance().commandList.Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		else
		{
			// �v���~�e�B�u�`��̐ݒ�R�}���h
			if (indexNum != LINE)
				Directx::GetInstance().commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p���X�g
			else//���̎��iLINE�j
				Directx::GetInstance().commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // ��

			// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
			Directx::GetInstance().commandList->IASetVertexBuffers(0, 1, &vbView);
		}
	}
	else//���̂̎�
	{
		Directx::GetInstance().commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�X�g���b�v
		// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
		Directx::GetInstance().commandList->IASetVertexBuffers(0, 1, &vbView2);
	}
	

	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	Directx::GetInstance().commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//04_02
	/*if (textureHandle != NULL)*/
	{
		//SRV�q�[�v�̐ݒ�R�}���h
		Directx::GetInstance().commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
		//SRV�q�[�v�̐擪�n���h�����擾
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
		srvGpuHandle.ptr = textureHandle;
		//(�C���X�^���X�œǂݍ��񂾃e�N�X�`���p��SRV���w��)
		Directx::GetInstance().commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	}
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	Directx::GetInstance().commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	if (indexNum != SPRITE)
	{
		if (indexNum == SPHERE)Directx::GetInstance().commandList->IASetIndexBuffer(&ibView3);
		else if (indexNum == MODEL) Directx::GetInstance().commandList->IASetIndexBuffer(&ibViewM);
		else if (indexNum != CIRCLE) Directx::GetInstance().commandList->IASetIndexBuffer(&ibView);
		else if (indexNum == CIRCLE)Directx::GetInstance().commandList->IASetIndexBuffer(&ibView2);
		
	}

	// �`��R�}���h
	switch (indexNum)
	{
	case TRIANGLE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indices2), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
		break;
	case BOX:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
		break;
	case CUBE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indicesCube), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
		break;
	case LINE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indicesLine), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
		break;//ibview�i+vbview�j��}�`���Ƃɗp�ӂ���I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I
	case CIRCLE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indicesCircle), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
		break;
	case SPHERE:
		Directx::GetInstance().commandList->DrawIndexedInstanced(_countof(indicesSphere), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
		break;
	case SPRITE:
		Directx::GetInstance().commandList->DrawInstanced(4, 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
		break;
	case MODEL:
		Directx::GetInstance().commandList->DrawIndexedInstanced((UINT)indicesM.size(), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
		break;
	}
}

void Draw::DrawTriangle(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,
	WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	vertices[0] = { pos1,{vertices[0].normal},{0.0f,1.0f} };//����
	vertices[1] = { pos2,{vertices[1].normal},{0.5f,0.0f} };//��
	vertices[2] = { pos3,{vertices[2].normal},{1.0f,1.0f} };//�E��
	vertices[3] = vertices[1];//�E��

	/*if (color.x != NULL && color.y != NULL && color.z != NULL && color.w != NULL)*/ constMapMaterial->color = color;
	
	Update(TRIANGLE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawBox(WorldMat* world, ViewMat* view, ProjectionMat* projection,/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, */
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	vertices[0].normal = { 0.0f,0.0f,-1.0f };//����
	vertices[1].normal = { 0.0f,0.0f,-1.0f };//����
	vertices[2].normal = { 0.0f,0.0f,-1.0f };//�E��
	vertices[3].normal = { 0.0f,0.0f,-1.0f };//�E��
	
	vertices[0].pos = {-5.0f,-5.0f,-5.0f};//����
	vertices[1].pos = {-5.0f,5.0f, -5.0f};//����
	vertices[2].pos = {5.0f,-5.0f, -5.0f};//�E��
	vertices[3].pos = {5.0f,5.0f,  -5.0f};//�E��

	/*if (color.x != NULL && color.y != NULL && color.z != NULL && color.w != NULL)*/ constMapMaterial->color = color;
	
	Update(BOX, pipelineNum, textureHandle,cbt);
}

void Draw::DrawBoxSprite(const Vec3& pos, const float& scale,
	XMFLOAT4 color , const UINT64 textureHandle, const Vec2& ancorUV, float rotation, const int& pipelineNum)
{
	//if (widthHeight.x != NULL && widthHeight.y != NULL)
	//{
	//	sprite.vertices[0] = { {-widthHeight.x / 2.0f,+widthHeight.y / 2.0f,0.0f},{0.0f,1.0f} };//����
	//	sprite.vertices[1] = { {-widthHeight.x / 2.0f,-widthHeight.y / 2.0f,0.0f},{0.0f,0.0f} };//����
	//	sprite.vertices[2] = { {+widthHeight.x / 2.0f,+widthHeight.y / 2.0f,0.0f},{1.0f,1.0f} };//�E��
	//	sprite.vertices[3] = { {+widthHeight.x / 2.0f,-widthHeight.y / 2.0f,0.0f},{1.0f,0.0f} };//�E��
	//}
	//else
	
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = texBuff[(textureHandle - srvGpuHandle.ptr) / Directx::GetInstance().device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type)]->GetDesc();

	verticesS[0] = { {-(float)(resDesc.Width * scale * ancorUV.x),+(float)(resDesc.Height * scale * (1.0f - ancorUV.y)),0.0f},{0.0f,1.0f} };//����
	verticesS[1] = { {-(float)(resDesc.Width * scale * ancorUV.x),-(float)(resDesc.Height * scale * (ancorUV.y)),0.0f},{0.0f,0.0f} };//����
	verticesS[2] = { {+(float)(resDesc.Width * scale * (1.0f - ancorUV.x)),+(float)(resDesc.Height * scale * (1.0f - ancorUV.y)),0.0f},{1.0f,1.0f} };//�E��
	verticesS[3] = { {+(float)(resDesc.Width * scale * (1.0f - ancorUV.x)),-(float)(resDesc.Height * scale * (ancorUV.y)),0.0f},{1.0f,0.0f} };//�E��
	

	/*if(color.x!=NULL&& color.y != NULL&& color.z != NULL&& color.w != NULL)*/ constMapMaterial->color = color;
	
	//���[���h�s��
	WorldMat worldMat;

	worldMat.rot.z = AngletoRadi(rotation);
	worldMat.trans = { pos.x /*+ resDesc.Width * ancorUV.x * scale*/,pos.y/* + resDesc.Height * ancorUV.y * scale*/,0.0f };
	worldMat.SetWorld();

	//�e��������
	if (worldMat.parent != nullptr)
	{
		worldMat.matWorld *= worldMat.parent->matWorld;
	}

	XMMATRIX matW;
	matW = { (float)worldMat.matWorld.m[0][0],(float)worldMat.matWorld.m[0][1],(float)worldMat.matWorld.m[0][2],(float)worldMat.matWorld.m[0][3],
			 (float)worldMat.matWorld.m[1][0],(float)worldMat.matWorld.m[1][1],(float)worldMat.matWorld.m[1][2],(float)worldMat.matWorld.m[1][3],
			 (float)worldMat.matWorld.m[2][0],(float)worldMat.matWorld.m[2][1],(float)worldMat.matWorld.m[2][2],(float)worldMat.matWorld.m[2][3],
			 (float)worldMat.matWorld.m[3][0],(float)worldMat.matWorld.m[3][1],(float)worldMat.matWorld.m[3][2],(float)worldMat.matWorld.m[3][3] };

	//view
	ViewMat view;
	view.matView = XMMatrixIdentity();


	//���s���e�̎ˉe�s�񐶐�
	ProjectionMat projection;

	projection.matProjection = XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().window_width, 
		WindowsApp::GetInstance().window_height, 0.0, 0.0f, 1.0f);

	cbt.constMapTransform->mat = matW * view.matView * projection.matProjection;

	Update(SPRITE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawClippingBoxSprite(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	XMFLOAT4 color, const UINT64 textureHandle, bool isPosLeftTop, float rotation, const int& pipelineNum)
{
	
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = texBuff[(textureHandle - srvGpuHandle.ptr) / Directx::GetInstance().device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type)]->GetDesc();

	float texLeft = UVleftTop.x * +(float)resDesc.Width * scale;
	float texRight = (UVleftTop.x + UVlength.x) * +(float)resDesc.Width * scale;
	float texTop = UVleftTop.y * +(float)resDesc.Height * scale;
	float texBottom = (UVleftTop.y + UVlength.y) * +(float)resDesc.Height * scale;

	if (isPosLeftTop)
	{
		//���ォ��̍��W
		verticesS[0] = { {0,UVlength.y * resDesc.Height * scale,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//����
		verticesS[1] = { {0,0,0.0f},{UVleftTop.x,UVleftTop.y} };//����
		verticesS[2] = { {UVlength.x * resDesc.Width * scale,UVlength.y * resDesc.Height * scale,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//�E��
		verticesS[3] = { {UVlength.x * resDesc.Width * scale,0,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//�E��
	}
	else
	{
		//�؂蔲������̉摜�̒��S����̈ʒu�I�I�I�I�I�I�I�I
		verticesS[0] = { {-UVlength.x * resDesc.Width * scale / 2.0f,UVlength.y * resDesc.Height * scale / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//����
		verticesS[1] = { {-UVlength.x * resDesc.Width * scale / 2.0f,-UVlength.y * resDesc.Height * scale / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y} };//����
		verticesS[2] = { {UVlength.x * resDesc.Width * scale / 2.0f,UVlength.y * resDesc.Height * scale / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//�E��
		verticesS[3] = { {UVlength.x * resDesc.Width * scale / 2.0f,-UVlength.y * resDesc.Height * scale / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//�E��
	}
	/*if(color.x!=NULL&& color.y != NULL&& color.z != NULL&& color.w != NULL)*/ constMapMaterial->color = color;
	
	//���[���h�s��
	WorldMat worldMat;

	worldMat.rot.z = AngletoRadi(rotation);

	if (isPosLeftTop)
	{
		//�����̍�����W��ݒ�
		worldMat.trans = { leftTop.x,leftTop.y,0 };
	}
	else
	{
		//�؂蔲������̉摜�̒��S��ݒ�I�I�I!!!!!!!!!!!!!!!!
		worldMat.trans = { leftTop.x + texLeft + UVlength.x * (float)resDesc.Width * scale / 2.0f,
			leftTop.y + texTop + UVlength.y * (float)resDesc.Height * scale / 2.0f,
			leftTop.z };
	}
	worldMat.SetWorld();


	//�e��������
	if (worldMat.parent != nullptr)
	{
		worldMat.matWorld *= worldMat.parent->matWorld;
	}

	XMMATRIX matW;
	matW = { (float)worldMat.matWorld.m[0][0],(float)worldMat.matWorld.m[0][1],(float)worldMat.matWorld.m[0][2],(float)worldMat.matWorld.m[0][3],
			 (float)worldMat.matWorld.m[1][0],(float)worldMat.matWorld.m[1][1],(float)worldMat.matWorld.m[1][2],(float)worldMat.matWorld.m[1][3],
			 (float)worldMat.matWorld.m[2][0],(float)worldMat.matWorld.m[2][1],(float)worldMat.matWorld.m[2][2],(float)worldMat.matWorld.m[2][3],
			 (float)worldMat.matWorld.m[3][0],(float)worldMat.matWorld.m[3][1],(float)worldMat.matWorld.m[3][2],(float)worldMat.matWorld.m[3][3] };

	//view
	ViewMat view;
	view.matView = XMMatrixIdentity();

	//���s���e�̎ˉe�s�񐶐�
	ProjectionMat projection;
	projection.matProjection = XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().window_width, 
		WindowsApp::GetInstance().window_height, 0.0, 0.0f, 1.0f);

	cbt.constMapTransform->mat = matW * view.matView * projection.matProjection;

	Update(SPRITE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawCube3D(WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	//��O
	vertices[0] = { {-5.0f,-5.0f,-5.0f},{vertices[0].normal},{0.0f,1.0f} };//����
	vertices[1] = { {-5.0f,5.0f, -5.0f},{vertices[1].normal},{0.0f,0.0f} };//����
	vertices[2] = { {5.0f,-5.0f, -5.0f},{vertices[2].normal},{1.0f,1.0f} };//�E��
	vertices[3] = { {5.0f,5.0f,  -5.0f},{vertices[3].normal},{1.0f,0.0f} };//�E��

	vertices[4] = { {-5.0f,-5.0f,5.0f},{vertices[4].normal},{0.0f,1.0f} };//����
	vertices[5] = { {-5.0f,5.0f, 5.0f},{vertices[5].normal},{0.0f,0.0f} };//����
	vertices[6] = { {5.0f,-5.0f, 5.0f},{vertices[6].normal},{1.0f,1.0f} };//�E��
	vertices[7] = { {5.0f,5.0f,  5.0f},{vertices[7].normal},{1.0f,0.0f} };//�E��
		//��
	vertices[8] = { {5.0f,5.0f,-5.0f},{vertices[8].normal},{0.0f,1.0f} };//����
	vertices[9] = { {5.0f,5.0f, 5.0f},{vertices[9].normal},{0.0f,0.0f} };//����
	vertices[10] = { {-5.0f,5.0f, -5.0f},{vertices[10].normal},{1.0f,1.0f} };//�E��
	vertices[11] = { {-5.0f,5.0f, 5.0f},{vertices[11].normal},{1.0f,0.0f} };//�E��

	vertices[12] = { {5.0f,-5.0f,-5.0f},{vertices[12].normal},{0.0f,1.0f} };//����
	vertices[13] = { {5.0f,-5.0f, 5.0f},{vertices[13].normal},{0.0f,0.0f} };//����
	vertices[14] = { {-5.0f,-5.0f, -5.0f},{vertices[14].normal},{1.0f,1.0f} };//�E��
	vertices[15] = { {-5.0f,-5.0f, 5.0f},{vertices[15].normal},{1.0f,0.0f} };//�E��

	vertices[16] = { {-5.0f,-5.0f,-5.0f},{vertices[16].normal},{0.0f,1.0f} };//����
	vertices[17] = { {-5.0f,-5.0f, 5.0f},{vertices[17].normal},{0.0f,0.0f} };//����
	vertices[18] = { {-5.0f,5.0f, -5.0f},{vertices[18].normal},{1.0f,1.0f} };//�E��
	vertices[19] = { {-5.0f,5.0f,  5.0f},{vertices[19].normal},{1.0f,0.0f} };//�E��

	vertices[20] = { {5.0f,-5.0f,-5.0f},{vertices[20].normal},{0.0f,1.0f} };//����
	vertices[21] = { {5.0f,-5.0f, 5.0f},{vertices[21].normal},{0.0f,0.0f} };//����
	vertices[22] = { {5.0f,5.0f, -5.0f},{vertices[22].normal},{1.0f,1.0f} };//�E��
	vertices[23] = { {5.0f,5.0f,  5.0f},{vertices[23].normal},{1.0f,0.0f} };//�E��;//����
	
	/*if (color.x != NULL && color.y != NULL && color.z != NULL && color.w != NULL)*/ constMapMaterial->color = color;
	
	Update(CUBE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawLine(const Vec3& pos1, const Vec3& pos2, WorldMat* world, ViewMat* view, ProjectionMat* projection,const XMFLOAT4& color,
	const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	vertices[0].pos = { pos1.x,pos1.y,pos1.z };
	vertices[1].pos = { pos2.x,pos2.y,pos2.z };
	vertices[0].normal = { 0,0,-1.0f };//normal�̏����i�����ȊO���j�ǂ��ɂ�����I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I
	vertices[1].normal = { 0,0,-1.0f };

	Update(LINE, pipelineNum, textureHandle, cbt, false);
}

void Draw::DrawCircle(float radius, WorldMat* world, ViewMat* view, ProjectionMat* projection, 
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	/*constBuffTransfer({ 0,0.01f,0,0 });*/
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	vertices[0].pos = {0.0f,0.0f,0.0f} ;

	static float count = _countof(vertices) - 2;//���S�_�Ə����̓_�̓J�E���g���Ȃ�

	for (int i = 1; i < _countof(vertices); i++)
	{
		vertices[i].pos = { radius * cosf(AngletoRadi(360 / count) * (i - 1)),radius * sinf(AngletoRadi(360 / count) * (i - 1)),0 };
	}
	//vertices[1] = { {-5.0f,5.0f, -5.0f},{},{0.0f,0.0f} };//����
	//vertices[2] = { {5.0f,-5.0f, -5.0f},{},{1.0f,1.0f} };//�E��
	//vertices[3] = { {5.0f,5.0f,  -5.0f},{},{1.0f,0.0f} };//�E��

	constMapMaterial->color = color;

	Update(CIRCLE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawSphere(WorldMat* world, ViewMat* view, ProjectionMat* projection,
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	/*constBuffTransfer({ 0,0.01f,0,0 });*/
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(SPHERE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawModel(WorldMat* world, ViewMat* view, ProjectionMat* projection,
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	/*constBuffTransfer({ 0,0.01f,0,0 });*/
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(MODEL, pipelineNum, textureHandle, cbt);
}

void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, const int& indexNum)
{
	if (indexNum == SPRITE)
	{
		// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/SpriteVS.hlsl", // �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&vsBlob, &errorBlob);

		// �G���[�Ȃ�
		Error(FAILED(Directx::GetInstance().result));

		// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/SpritePS.hlsl", // �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&psBlob, &errorBlob);

		// �G���[�Ȃ�
		Error(FAILED(Directx::GetInstance().result));
	}
	else
	{
		// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/BasicVS.hlsl", // �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&vsBlob, &errorBlob);

		// �G���[�Ȃ�
		Error(FAILED(Directx::GetInstance().result));

		// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/BasicPS.hlsl", // �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&psBlob, &errorBlob);

		// �G���[�Ȃ�
		Error(FAILED(Directx::GetInstance().result));
	}

	// �V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	if (indexNum == SPRITE)
	{
		pipelineDesc.RasterizerState = D3D12_RASTERIZER_DESC();
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; 
	}
	else
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // �w�ʃJ�����O
	
	pipelineDesc.RasterizerState.FillMode = fillMode; // �|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	Blend(D3D12_BLEND_OP_ADD, false, true);

	// ���_���C�A�E�g�̐ݒ�
	if (indexNum == SPRITE)
	{
		pipelineDesc.InputLayout.pInputElementDescs = inputLayoutSprite;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayoutSprite);
	}
	else
	{
		pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	}

	// �}�`�̌`��ݒ�
	if (indexNum == LINE)
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	else
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	//04_02
	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //���J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //�c�J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //���s���J��Ԃ��i�^�C�����O�j
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;    //�S�ă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                  //�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;                               //�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_�[����̂ݎg�p�\

	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;//���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);//���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	Directx::GetInstance().result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(Directx::GetInstance().result));
	Directx::GetInstance().result = Directx::GetInstance().device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSig));
	assert(SUCCEEDED(Directx::GetInstance().result));
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = *rootSig;

	//06_01
	//�f�v�X�X�e���V���X�e�[�g
	pipelineDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC();
	pipelineDesc.DepthStencilState.DepthEnable = true;//�[�x�e�X�g���s��
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
	if(indexNum == SPRITE)
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//��������΍��i
	else
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//��������΍��i
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g

	Directx::GetInstance().result = Directx::GetInstance().device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelineState));
	//assert(SUCCEEDED(Directx::GetInstance().result));
}

void Blend(const D3D12_BLEND_OP& blendMode, const bool& Inversion, const bool& Translucent)
{
	//���ʐݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc.BlendState.RenderTarget[0];
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA�S�Ẵ`�����l����`��
	blendDesc.BlendEnable = true;//�u�����h��L��
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//���Z
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;//�f�X�g�̒l��0%�g��
	//��������
	blendDesc.BlendOp = blendMode;
	if (Inversion)//���]
	{
		blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-�f�X�g�J���[�̒l
		blendDesc.DestBlend = D3D12_BLEND_ZERO;//�g��Ȃ�
	}
	else if (Translucent)//������
	{
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//1.0f-�f�X�g�J���[�̒l
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-�\�[�X�̃A���t�@�l
	}
	else
	{
		blendDesc.SrcBlend = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
		blendDesc.DestBlend = D3D12_BLEND_ONE;//�f�X�g�̒l��100%�g��
	}
}

void Draw::constBuffTransfer(const XMFLOAT4& plusRGBA)
{
	if (constMapMaterial->color.x <= 1.0f && constMapMaterial->color.x >= 0.0f)
		constMapMaterial->color.x += plusRGBA.x;
	if (constMapMaterial->color.y <= 1.0f && constMapMaterial->color.y >= 0.0f)
		constMapMaterial->color.y += plusRGBA.y;
	if (constMapMaterial->color.z <= 1.0f && constMapMaterial->color.z >= 0.0f)
		constMapMaterial->color.z += plusRGBA.z;
	if (constMapMaterial->color.w <= 1.0f && constMapMaterial->color.w >= 0.0f)
		constMapMaterial->color.w += plusRGBA.w;
}



void SetNormDigitalMat(XMMATRIX& mat)
{
	mat.r[0].m128_f32[0] = 2.0f / WindowsApp::GetInstance().window_width;
	mat.r[1].m128_f32[1] = -2.0f / WindowsApp::GetInstance().window_height;
	mat.r[3].m128_f32[0] = -1.0f;
	mat.r[3].m128_f32[1] = 1.0f;
}

void Error(const bool& filed)
{
	if (filed)
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}

