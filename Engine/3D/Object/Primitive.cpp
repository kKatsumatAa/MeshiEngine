#include "Primitive.h"
#include "Util.h"

using namespace DirectX;


uint16_t Primitive::sIndicesBox_[6] =
{
	0,1,2,//�O�p�`1��
	2,1,3,//�O�p�`2��
};
uint16_t Primitive::sIndicesTriangle_[3] =
{
	0,1,2//�O�p�`2��
};
uint16_t Primitive::sIndicesCube_[36] =
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

uint16_t Primitive::sIndicesCircle_[] =
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

uint16_t Primitive::sIndicesLine_[2] =
{
	0,1//�O�p�`2��
};

uint16_t Primitive::sIndicesSphere_[Primitive::S_SPHERE_INDEX_NUM_ * 36];


uint32_t Primitive::sizeVB_;

//---------------------------------------------

void Primitive::Initialize()
{
	InitializeTriangle();
	InitializeBox();
	InitializeCircle();
	InitializeLine();
	InitializeCube();
	InitializeSphere();
}

void Primitive::InitializeTriangle()
{
	HRESULT result = {};

	float angle = PI * 2.0f + PI * 2.0f / 3.0f * 2.0f;
	float angle2 = PI * 2.0f;
	float angle3 = PI * 2.0f + PI * 2.0f / 3.0f;

	verticesTriangle_[0] = { {sinf(angle),cosf(angle),0},{verticesTriangle_[0].normal},{},{0.0f,1.0f} };//����
	verticesTriangle_[1] = { {0,cosf(angle2),0},{verticesTriangle_[1].normal},{},{0.5f,0.0f} };//��
	verticesTriangle_[2] = { {sinf(angle3),cosf(angle3),0},{verticesTriangle_[2].normal},{},{1.0f,1.0f} };//�E��


	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesTriangle_[0]) * _countof(verticesTriangle_));

	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//���_�o�b�t�@�̐���
	BuffProperties(heapProp_, resDesc, &vertBuffTriangle_);

	// ���_�o�b�t�@�r���[�̍쐬
	// GPU���z�A�h���X
	vbTriangleView_.BufferLocation = vertBuffTriangle_->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbTriangleView_.SizeInBytes = sizeVB_;
	// ���_1���̃f�[�^�T�C�Y
	vbTriangleView_.StrideInBytes = sizeof(verticesTriangle_[0]);

	{
		//03_04
		//�C���f�b�N�X�f�[�^
		//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesTriangle_));

		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//�C���f�b�N�X�o�b�t�@�̍쐬
		ID3D12Resource* indexBuff = nullptr;//GPU���̃�����
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//�C���f�b�N�X�o�b�t�@���}�b�s���O
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//�S�C���f�b�N�X�ɑ΂���
		for (int32_t i = 0; i < _countof(sIndicesTriangle_); i++)
		{
			indexMap[i] = sIndicesTriangle_[i];//�C���f�b�N�X���R�s�[

		}
		//�}�b�s���O������
		indexBuff->Unmap(0, nullptr);

		//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
		ibViewTriangle_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewTriangle_.Format = DXGI_FORMAT_R16_UINT;
		ibViewTriangle_.SizeInBytes = sizeIB;
	}

	//06_03
	{
		for (int32_t i = 0; i < _countof(sIndicesTriangle_) / 3; i++)
		{//�O�p�`����ƂɌv�Z
			//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
			uint16_t index0 = sIndicesTriangle_[i * 3 + 0];
			uint16_t index1 = sIndicesTriangle_[i * 3 + 1];
			uint16_t index2 = sIndicesTriangle_[i * 3 + 2];
			//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
			XMVECTOR p0 = XMLoadFloat3(&verticesTriangle_[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesTriangle_[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesTriangle_[index2].pos);
			//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//�O�ρi�����ȃx�N�g���j
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//���߂��@���𒸓_�f�[�^�ɑ��
			XMStoreFloat3(&verticesTriangle_[index0].normal, normal);
			XMStoreFloat3(&verticesTriangle_[index1].normal, normal);
			XMStoreFloat3(&verticesTriangle_[index2].normal, normal);
		}
	}
}

void Primitive::InitializeBox()
{
	HRESULT result = {};

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesBox_[0]) * _countof(verticesBox_));

	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//���_�o�b�t�@�̐���
	BuffProperties(heapProp_, resDesc, &vertBuffBox_);

	// ���_�o�b�t�@�r���[�̍쐬
	// GPU���z�A�h���X
	vbBoxView_.BufferLocation = vertBuffBox_->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbBoxView_.SizeInBytes = sizeVB_;
	// ���_1���̃f�[�^�T�C�Y
	vbBoxView_.StrideInBytes = sizeof(verticesBox_[0]);

	{
		//03_04
		//�C���f�b�N�X�f�[�^
		//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesBox_));

		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//�C���f�b�N�X�o�b�t�@�̍쐬
		ID3D12Resource* indexBuff = nullptr;//GPU���̃�����
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//�C���f�b�N�X�o�b�t�@���}�b�s���O
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//�S�C���f�b�N�X�ɑ΂���
		for (int32_t i = 0; i < _countof(sIndicesBox_); i++)
		{
			indexMap[i] = sIndicesBox_[i];//�C���f�b�N�X���R�s�[

		}
		//�}�b�s���O������
		indexBuff->Unmap(0, nullptr);

		//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
		ibViewBox_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewBox_.Format = DXGI_FORMAT_R16_UINT;
		ibViewBox_.SizeInBytes = sizeIB;
	}

	//06_03
	{
		for (int32_t i = 0; i < _countof(sIndicesBox_) / 3; i++)
		{//�O�p�`����ƂɌv�Z
			//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
			uint16_t index0 = sIndicesBox_[i * 3 + 0];
			uint16_t index1 = sIndicesBox_[i * 3 + 1];
			uint16_t index2 = sIndicesBox_[i * 3 + 2];
			//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
			XMVECTOR p0 = XMLoadFloat3(&verticesBox_[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesBox_[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesBox_[index2].pos);
			//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//�O�ρi�����ȃx�N�g���j
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//���߂��@���𒸓_�f�[�^�ɑ��
			XMStoreFloat3(&verticesBox_[index0].normal, normal);
			XMStoreFloat3(&verticesBox_[index1].normal, normal);
			XMStoreFloat3(&verticesBox_[index2].normal, normal);
		}
	}
}

void Primitive::InitializeCircle()
{
	HRESULT result = {};

	verticesCircle_[0].pos = { 0.0f,0.0f,0.0f };

	static float sCount = _countof(verticesCircle_) - 2;//���S�_�Ə����̓_�̓J�E���g���Ȃ�

	for (int32_t i = 1; i < _countof(verticesCircle_); i++)
	{
		verticesCircle_[i].pos = { 1.0f * cosf(AngletoRadi(360 / sCount) * (i - 1)),1.0f * sinf(AngletoRadi(360 / sCount) * (i - 1)),0 };
	}

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesCircle_[0]) * _countof(verticesCircle_));

	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//���_�o�b�t�@�̐���
	BuffProperties(heapProp_, resDesc, &vertBuffCircle_);

	// ���_�o�b�t�@�r���[�̍쐬
	// GPU���z�A�h���X
	vbCircleView_.BufferLocation = vertBuffCircle_->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbCircleView_.SizeInBytes = sizeVB_;
	// ���_1���̃f�[�^�T�C�Y
	vbCircleView_.StrideInBytes = sizeof(verticesCircle_[0]);

	{
		//03_04
		//�C���f�b�N�X�f�[�^
		//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesCircle_));

		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//�C���f�b�N�X�o�b�t�@�̍쐬
		ID3D12Resource* indexBuff = nullptr;//GPU���̃�����
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//�C���f�b�N�X�o�b�t�@���}�b�s���O
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//�S�C���f�b�N�X�ɑ΂���
		for (int32_t i = 0; i < _countof(sIndicesCircle_); i++)
		{
			indexMap[i] = sIndicesCircle_[i];//�C���f�b�N�X���R�s�[

		}
		//�}�b�s���O������
		indexBuff->Unmap(0, nullptr);

		//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
		ibViewCircle_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewCircle_.Format = DXGI_FORMAT_R16_UINT;
		ibViewCircle_.SizeInBytes = sizeIB;
	}

	//06_03
	{
		for (int32_t i = 0; i < _countof(sIndicesCircle_) / 3; i++)
		{//�O�p�`����ƂɌv�Z
			//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
			uint16_t index0 = sIndicesCircle_[i * 3 + 0];
			uint16_t index1 = sIndicesCircle_[i * 3 + 1];
			uint16_t index2 = sIndicesCircle_[i * 3 + 2];
			//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
			XMVECTOR p0 = XMLoadFloat3(&verticesCircle_[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesCircle_[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesCircle_[index2].pos);
			//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//�O�ρi�����ȃx�N�g���j
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//���߂��@���𒸓_�f�[�^�ɑ��
			XMStoreFloat3(&verticesCircle_[index0].normal, normal);
			XMStoreFloat3(&verticesCircle_[index1].normal, normal);
			XMStoreFloat3(&verticesCircle_[index2].normal, normal);
		}
	}
}

void Primitive::InitializeCube()
{
	HRESULT result = {};

	{
		//��O
		verticesCube_[0] = { {-1.0f,-1.0f,-1.0f},{verticesCube_[0].normal},{},{0.0f,1.0f} };//����
		verticesCube_[1] = { {-1.0f,1.0f, -1.0f},{verticesCube_[1].normal},{},{0.0f,0.0f} };//����
		verticesCube_[2] = { {1.0f,-1.0f, -1.0f},{verticesCube_[2].normal},{},{1.0f,1.0f} };//�E��
		verticesCube_[3] = { {1.0f,1.0f,  -1.0f},{verticesCube_[3].normal},{},{1.0f,0.0f} };//�E��

		verticesCube_[4] = { {-1.0f,-1.0f,1.0f},{verticesCube_[4].normal},{},{0.0f,1.0f} };//����
		verticesCube_[5] = { {-1.0f,1.0f, 1.0f},{verticesCube_[5].normal},{},{0.0f,0.0f} };//����
		verticesCube_[6] = { {1.0f,-1.0f, 1.0f},{verticesCube_[6].normal},{},{1.0f,1.0f} };//�E��
		verticesCube_[7] = { {1.0f,1.0f,  1.0f},{verticesCube_[7].normal},{},{1.0f,0.0f} };//�E��
		//��
		verticesCube_[8] = { {1.0f,1.0f,-1.0f},{verticesCube_[8].normal},{},{0.0f,1.0f} };//����
		verticesCube_[9] = { {1.0f,1.0f, 1.0f},{verticesCube_[9].normal},{},{0.0f,0.0f} };//����
		verticesCube_[10] = { {-1.0f,1.0f, -1.0f},{verticesCube_[10].normal},{},{1.0f,1.0f} };//�E��
		verticesCube_[11] = { {-1.0f,1.0f, 1.0f},{verticesCube_[11].normal},{},{1.0f,0.0f} };//�E��

		verticesCube_[12] = { {1.0f,-1.0f,-1.0f},{verticesCube_[12].normal},{},{0.0f,1.0f} };//����
		verticesCube_[13] = { {1.0f,-1.0f, 1.0f},{verticesCube_[13].normal},{},{0.0f,0.0f} };//����
		verticesCube_[14] = { {-1.0f,-1.0f, -1.0f},{verticesCube_[14].normal},{},{1.0f,1.0f} };//�E��
		verticesCube_[15] = { {-1.0f,-1.0f, 1.0f},{verticesCube_[15].normal},{},{1.0f,0.0f} };//�E��

		verticesCube_[16] = { {-1.0f,-1.0f,-1.0f},{verticesCube_[16].normal},{},{0.0f,1.0f} };//����
		verticesCube_[17] = { {-1.0f,-1.0f, 1.0f},{verticesCube_[17].normal},{},{0.0f,0.0f} };//����
		verticesCube_[18] = { {-1.0f,1.0f, -1.0f},{verticesCube_[18].normal},{},{1.0f,1.0f} };//�E��
		verticesCube_[19] = { {-1.0f,1.0f,  1.0f},{verticesCube_[19].normal},{},{1.0f,0.0f} };//�E��

		verticesCube_[20] = { {1.0f,-1.0f,-1.0f},{verticesCube_[20].normal},{},{0.0f,1.0f} };//����
		verticesCube_[21] = { {1.0f,-1.0f, 1.0f},{verticesCube_[21].normal},{},{0.0f,0.0f} };//����
		verticesCube_[22] = { {1.0f,1.0f, -1.0f},{verticesCube_[22].normal},{},{1.0f,1.0f} };//�E��
		verticesCube_[23] = { {1.0f,1.0f,  1.0f},{verticesCube_[23].normal},{},{1.0f,0.0f} };//�E��;//����
	}

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesCube_[0]) * _countof(verticesCube_));

	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//���_�o�b�t�@�̐���
	BuffProperties(heapProp_, resDesc, &vertBuffCube_);

	// ���_�o�b�t�@�r���[�̍쐬
	// GPU���z�A�h���X
	vbCubeView_.BufferLocation = vertBuffCube_->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbCubeView_.SizeInBytes = sizeVB_;
	// ���_1���̃f�[�^�T�C�Y
	vbCubeView_.StrideInBytes = sizeof(verticesCube_[0]);

	{
		//03_04
		//�C���f�b�N�X�f�[�^
		//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesCube_));

		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//�C���f�b�N�X�o�b�t�@�̍쐬
		ID3D12Resource* indexBuff = nullptr;//GPU���̃�����
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//�C���f�b�N�X�o�b�t�@���}�b�s���O
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//�S�C���f�b�N�X�ɑ΂���
		for (int32_t i = 0; i < _countof(sIndicesCube_); i++)
		{
			indexMap[i] = sIndicesCube_[i];//�C���f�b�N�X���R�s�[

		}
		//�}�b�s���O������
		indexBuff->Unmap(0, nullptr);

		//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
		ibViewCube_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewCube_.Format = DXGI_FORMAT_R16_UINT;
		ibViewCube_.SizeInBytes = sizeIB;
	}


	//06_03
		/*if (indexNum == SPHERE)*/
	{
		for (int32_t i = 0; i < _countof(sIndicesCube_) / 3; i++)
		{//�O�p�`����ƂɌv�Z
			//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
			uint16_t index0 = sIndicesCube_[i * 3 + 0];
			uint16_t index1 = sIndicesCube_[i * 3 + 1];
			uint16_t index2 = sIndicesCube_[i * 3 + 2];
			//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
			XMVECTOR p0 = XMLoadFloat3(&verticesCube_[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesCube_[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesCube_[index2].pos);
			//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//�O�ρi�����ȃx�N�g���j
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//���߂��@���𒸓_�f�[�^�ɑ��
			XMStoreFloat3(&verticesCube_[index0].normal, normal);
			XMStoreFloat3(&verticesCube_[index1].normal, normal);
			XMStoreFloat3(&verticesCube_[index2].normal, normal);
		}
	}
}

void Primitive::InitializeLine()
{
	HRESULT result = {};

	verticesLine_[0] = { {-1.0,0,0},{verticesLine_[0].normal},{},{0.0f,1.0f} };//����
	verticesLine_[1] = { {1.0,0,0},  {verticesLine_[1].normal},{},{0.5f,0.0f} };//��

	{
		//03_04
		//�C���f�b�N�X�f�[�^
		//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesLine_));

		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeIB);
		//�C���f�b�N�X�o�b�t�@�̍쐬
		ID3D12Resource* indexBuff = nullptr;//GPU���̃�����
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//�C���f�b�N�X�o�b�t�@���}�b�s���O
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//�S�C���f�b�N�X�ɑ΂���
		for (int32_t i = 0; i < _countof(sIndicesLine_); i++)
		{
			indexMap[i] = sIndicesLine_[i];//�C���f�b�N�X���R�s�[

		}
		//�}�b�s���O������
		indexBuff->Unmap(0, nullptr);

		//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
		ibViewLine_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewLine_.Format = DXGI_FORMAT_R16_UINT;
		ibViewLine_.SizeInBytes = sizeIB;
	}

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
	sizeVB_ = static_cast<uint32_t>(sizeof(verticesLine_[0]) * _countof(verticesLine_));

	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
	ResourceProperties(resDesc, sizeVB_);

	//���_�o�b�t�@�̐���
	BuffProperties(heapProp_, resDesc, &vertBuffLine_);

	// ���_�o�b�t�@�r���[�̍쐬
	// GPU���z�A�h���X
	vbLineView_.BufferLocation = vertBuffLine_->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbLineView_.SizeInBytes = sizeVB_;
	// ���_1���̃f�[�^�T�C�Y
	vbLineView_.StrideInBytes = sizeof(verticesLine_[0]);
}

void Primitive::InitializeSphere()
{
	HRESULT result = {};

	//���̗p
	{
		// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
		sizeVB_ = static_cast<uint32_t>(sizeof(verticesSphere_[0]) * (_countof(verticesSphere_)));

		//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
		heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

		D3D12_RESOURCE_DESC resDesc = TextureManager::GetResDesc();
		ResourceProperties(resDesc, sizeVB_);

		//���_�o�b�t�@�̐���
		BuffProperties(heapProp_, resDesc, vertBuffSphere_.GetAddressOf());

		// ���_�o�b�t�@�r���[�̍쐬
		// GPU���z�A�h���X
		vbViewSphere_.BufferLocation = vertBuffSphere_->GetGPUVirtualAddress();
		// ���_�o�b�t�@�̃T�C�Y
		vbViewSphere_.SizeInBytes = sizeVB_;
		// ���_1���̃f�[�^�T�C�Y
		vbViewSphere_.StrideInBytes = sizeof(verticesSphere_[0]);


		WorldMat worldMat;
		Vec3 vec = { 0,-1.0f,0 };

		//���_���
		verticesSphere_[0] = { {vec.x_,vec.y_,vec.z_},{},{},{1.0f,0.0f} };//��
		verticesSphere_[1] = { {vec.x_,-vec.y_,vec.z_},{},{},{1.0f,0.0f} };//��

		for (int32_t i = 0; i < 36; i++)//��
		{
			worldMat.rot_.y_ = (float)i * AngletoRadi(360.0f / 35.0f);


			for (int32_t j = 0; j < 34; j++)//�c
			{
				worldMat.rot_.x_ = ((float)(j + 1) * (PI / 35.0f));
				worldMat.CalcWorldMat();
				vec = { 0,-1.0f,0 };
				Vec3xM4(vec, worldMat.matWorld_, false);

				int32_t p = i * 34 + j;
				verticesSphere_[(2) + i * 34 + j] = { {vec.x_,vec.y_,vec.z_},{},{},{1.0f,0.0f} };
			}
		}

		int32_t count = 0;
		int32_t count2 = 0;
		int32_t count3 = 0;
		int32_t count4 = 0;
		bool isLast = false;
		//�C���f�b�N�X
		for (int32_t i = 0; i < _countof(sIndicesSphere_); i++)
		{
			if (i % (S_SPHERE_INDEX_NUM_ * 35) == 0 && i != 0)//�Ō�̏c�̗�
			{
				isLast = true;
			}

			if (i % S_SPHERE_INDEX_NUM_ == 0 || i % (S_SPHERE_INDEX_NUM_ * (count + 1) - 3) == 0)
			{

			}
			else if (1)//��ԉ����ザ��Ȃ����
			{
				if (count2 % 2 == 0)
				{
					if (isLast)
					{
						sIndicesSphere_[i] = 2 + 34 * count + (count3);
						sIndicesSphere_[i + 1] = 2 + 0 + (count3);//������Ă����̂ň�ԍŏ��̗���g��
						sIndicesSphere_[i + 2] = 2 + 34 * count + (count3 + 1);

						count3++;
						i += 2;
					}
					else
					{
						sIndicesSphere_[i] = 2 + 34 * count + (count3);
						sIndicesSphere_[i + 1] = 2 + 34 * (count + 1) + (count3);
						sIndicesSphere_[i + 2] = 2 + 34 * count + (count3 + 1);

						count3++;
						i += 2;
					}
				}
				else if (count2 % 2 == 1)
				{
					if (isLast)
					{
						sIndicesSphere_[i] = 2 + 0 + (count4 + 1);//������Ă����̂ň�ԍŏ��̗���g��
						sIndicesSphere_[i + 1] = 2 + 34 * count + (count4 + 1);
						sIndicesSphere_[i + 2] = 2 + 0 + (count4);//������Ă����̂ň�ԍŏ��̗���g��

						count4++;
						i += 2;
					}
					else
					{
						sIndicesSphere_[i] = 2 + 34 * (count + 1) + (count4 + 1);
						sIndicesSphere_[i + 1] = 2 + 34 * count + (count4 + 1);
						sIndicesSphere_[i + 2] = 2 + 34 * (count + 1) + (count4);

						count4++;
						i += 2;
					}
				}

				count2++;
			}
			if (i % S_SPHERE_INDEX_NUM_ == 0 || i % (S_SPHERE_INDEX_NUM_ * (count + 1) - 3) == 0)
			{
				if (i % S_SPHERE_INDEX_NUM_ == 0)//��ԉ��̎O�p�`
				{
					if (isLast)
					{
						sIndicesSphere_[i] = 0;
						sIndicesSphere_[i + 1] = 2 + 0;
						sIndicesSphere_[i + 2] = 2 + 34 * count;

						i += 2;
					}
					else
					{
						sIndicesSphere_[i] = 2 + 34 * (count + 1);
						sIndicesSphere_[i + 1] = 2 + 34 * count;
						sIndicesSphere_[i + 2] = 0;

						i += 2;
					}
				}
				else if (i % (S_SPHERE_INDEX_NUM_ * (count + 1) - 3) == 0)//��ԏ�̎O�p�`
				{
					if (isLast)
					{
						sIndicesSphere_[i] = 1 + 34 * (count + 1);
						sIndicesSphere_[i + 1] = 35;
						sIndicesSphere_[i + 2] = 1;

						i += 2;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
					else
					{
						sIndicesSphere_[i] = 1 + 34 * (count + 1);
						sIndicesSphere_[i + 1] = 1 + 34 * (count + 2);
						sIndicesSphere_[i + 2] = 1;

						i += 2;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
				}
			}
		}

		uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * _countof(sIndicesSphere_));

		//���\�[�X�ݒ�
		ResourceProperties(resDesc, sizeIB);
		//�C���f�b�N�X�o�b�t�@�̍쐬
		ID3D12Resource* indexBuff = nullptr;//GPU���̃�����
		BuffProperties(heapProp_, resDesc, &indexBuff);
		//�C���f�b�N�X�o�b�t�@���}�b�s���O
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//�S�C���f�b�N�X�ɑ΂���
		for (int32_t i = 0; i < _countof(sIndicesSphere_); i++)
		{
			indexMap[i] = sIndicesSphere_[i];//�C���f�b�N�X���R�s�[

		}
		//�}�b�s���O������
		indexBuff->Unmap(0, nullptr);

		//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
		ibViewSphere_.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewSphere_.Format = DXGI_FORMAT_R16_UINT;
		ibViewSphere_.SizeInBytes = sizeIB;


		//06_03
		/*if (indexNum == SPHERE)*/
		{
			for (int32_t i = 0; i < _countof(sIndicesSphere_) / 3; i++)
			{//�O�p�`����ƂɌv�Z
				//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
				uint16_t index0 = sIndicesSphere_[i * 3 + 0];
				uint16_t index1 = sIndicesSphere_[i * 3 + 1];
				uint16_t index2 = sIndicesSphere_[i * 3 + 2];
				//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
				XMVECTOR p0 = XMLoadFloat3(&verticesSphere_[index0].pos);
				XMVECTOR p1 = XMLoadFloat3(&verticesSphere_[index1].pos);
				XMVECTOR p2 = XMLoadFloat3(&verticesSphere_[index2].pos);
				//p0->p1�x�N�g���Ap0->p2�x�N�g�����v�Z
				XMVECTOR v1 = XMVectorSubtract(p1, p0);
				XMVECTOR v2 = XMVectorSubtract(p2, p0);
				//�O�ρi�����ȃx�N�g���j
				XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
				//���߂��@���𒸓_�f�[�^�ɑ��
				XMStoreFloat3(&verticesSphere_[index0].normal, normal);
				XMStoreFloat3(&verticesSphere_[index1].normal, normal);
				XMStoreFloat3(&verticesSphere_[index2].normal, normal);
			}
		}
	}
}

void Primitive::DrawCommandPrimitive(ID3D12Resource* vertBuff, int32_t vertexCount, Vertex* vertex, const D3D12_PRIMITIVE_TOPOLOGY& primitiveTopology,
	const D3D12_VERTEX_BUFFER_VIEW& buffView, const D3D12_INDEX_BUFFER_VIEW& ibView, int32_t indicesCount,
	const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	HRESULT result = {};

	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// �S���_�ɑ΂���
	for (int32_t i = 0; i < vertexCount; i++) {

		vertMap[i] = vertex[i]; // ���W���R�s�[

	}
	// �q���������
	vertBuff->Unmap(0, nullptr);

	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	setRootParam();

	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(primitiveTopology);

	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &buffView);

	DirectXWrapper::GetInstance().GetCommandList()->IASetIndexBuffer(&ibView);

	//�}�e���A���Ƃ��e�N�X�`��
	setMaterialLightTex();

	DirectXWrapper::GetInstance().GetCommandList()->DrawIndexedInstanced((uint32_t)indicesCount, 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void Primitive::TriangleDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffTriangle_.Get(), _countof(verticesTriangle_), verticesTriangle_, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		vbTriangleView_, ibViewTriangle_,
		_countof(sIndicesTriangle_),
		setRootParam, setMaterialLightTex);
}

void Primitive::BoxDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffBox_.Get(), _countof(verticesBox_), verticesBox_, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		vbBoxView_, ibViewBox_,
		_countof(sIndicesBox_),
		setRootParam, setMaterialLightTex);
}

void Primitive::CircleDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffCircle_.Get(), _countof(verticesCircle_), verticesCircle_, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		vbCircleView_, ibViewCircle_,
		_countof(sIndicesCircle_),
		setRootParam, setMaterialLightTex);
}

void Primitive::CubeDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffCube_.Get(), _countof(verticesCube_), verticesCube_, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		vbCubeView_, ibViewCube_,
		_countof(sIndicesCube_),
		setRootParam, setMaterialLightTex);
}

void Primitive::LineDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffLine_.Get(), _countof(verticesLine_), verticesLine_, D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		vbLineView_, ibViewLine_,
		_countof(sIndicesLine_),
		setRootParam, setMaterialLightTex);
}

void Primitive::SphereDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	DrawCommandPrimitive(vertBuffSphere_.Get(), _countof(verticesSphere_), verticesSphere_, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		vbViewSphere_, ibViewSphere_,
		_countof(sIndicesSphere_),
		setRootParam, setMaterialLightTex);
}


