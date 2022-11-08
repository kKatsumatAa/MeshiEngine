#include "Primitive.h"
#include "Util.h"



UINT sizeVB;

void Primitive::Initialize()
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
		Vec3 vec = { 0,-5.0f,0 };

		//���_���
		verticesSphere[0] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };//��
		verticesSphere[1] = { {vec.x,-vec.y,vec.z},{},{1.0f,0.0f} };//��

		for (int i = 0; i < 36; i++)//��
		{
			worldMat.rot.y = (float)i * AngletoRadi(360.0f / 35.0f);


			for (int j = 0; j < 34; j++)//�c
			{
				worldMat.rot.x = ((float)(j + 1) * (pi / 35.0f));
				worldMat.SetWorld();
				vec = { 0,-5.0f,0 };
				Vec3xM4(vec, worldMat.matWorld, false);

				int p = i * 34 + j;
				verticesSphere[(2) + i * 34 + j] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };
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

	{
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
}
