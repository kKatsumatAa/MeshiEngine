#include "Sprite.h"

int count = 0;

void LoadGraph(const wchar_t* name, UINT64& textureHandle)
{
	assert(count <= srvCount - 1);

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

	//04_02(�摜�\��p�̃A�h���X��������)
	//SRV�q�[�v�̐ݒ�R�}���h
	Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
	//SRV�q�[�v�̃n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	textureHandle = srvGpuHandle.ptr + (Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type) * count);


	count++;
}

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size)
{
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = size;						//���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource, ID3D12Resource** buff)
{
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&heap,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resource,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buff));
	assert(SUCCEEDED(Directx::GetInstance().result));
}


void Sprite::Initialize()
{
	//sprite�p
	{
		UINT sizeVB;
		D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
		// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
		sizeVB = static_cast<UINT>(sizeof(vertices[0]) * 4.0);
		//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

		ResourceProperties(resDesc, sizeVB);
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		//���_�o�b�t�@�̐���
		BuffProperties(heapProp, resDesc, vertBuff.GetAddressOf());

		// ���_�o�b�t�@�r���[�̍쐬
		// GPU���z�A�h���X
		vbView.BufferLocation = vertBuff.Get()->GetGPUVirtualAddress();
		// ���_�o�b�t�@�̃T�C�Y
		vbView.SizeInBytes = sizeVB;
		// ���_1���̃f�[�^�T�C�Y
		vbView.StrideInBytes = sizeof(vertices[0]);
	}
}

void SpriteCommonBeginDraw(PipeLineSet* pipelineSet)
{
	Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineSet->pipelineState.Get());

	Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(pipelineSet->rootSignature.Get());

	Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::SpriteDraw()
{
	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	VertexSprite* vertMap = nullptr;
	Directx::GetInstance().result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(Directx::GetInstance().result));
	// �S���_�ɑ΂���
	for (int i = 0; i < 4; i++) {
		vertMap[i] = vertices[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff->Unmap(0, nullptr);

	Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	Directx::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

void Sprite::Update(const Vec3& pos, const float& scale,
	XMFLOAT4 color, const UINT64 textureHandle, const Vec2& ancorUV,
	const bool& isReverseX,const bool& isReverseY, float rotation,
	ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial)
{
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = texBuff[(textureHandle - srvGpuHandle.ptr) / Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type)]->GetDesc();

	Vec2 length = { (float)resDesc.Width ,(float)resDesc.Height };

	//���]
	if (isReverseX)length.x *= -1;
	if (isReverseY)length.y *= -1;

	vertices[0] = { {-(float)(length.x * scale * ancorUV.x),+(float)(length.y * scale * (1.0f - ancorUV.y)),0.0f},{0.0f,1.0f} };//����
	vertices[1] = { {-(float)(length.x * scale * ancorUV.x),-(float)(length.y * scale * (ancorUV.y)),0.0f},{0.0f,0.0f} };//����
	vertices[2] = { {+(float)(length.x * scale * (1.0f - ancorUV.x)),+(float)(length.y * scale * (1.0f - ancorUV.y)),0.0f},{1.0f,1.0f} };//�E��
	vertices[3] = { {+(float)(length.x * scale * (1.0f - ancorUV.x)),-(float)(length.y * scale * (ancorUV.y)),0.0f},{1.0f,0.0f} };//�E��


	constMapMaterial->color = color;

	//���[���h�s��
	WorldMat worldMat;

	worldMat.rot.z = AngletoRadi(rotation);
	worldMat.trans = { pos.x /*+ length.x * ancorUV.x * scale*/,pos.y/* + length.y * ancorUV.y * scale*/,0.0f };
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

	cbt->constMapTransform->mat = matW * view.matView * projection.matProjection;
}

void Sprite::UpdateClipping(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	XMFLOAT4 color, const UINT64 textureHandle, bool isPosLeftTop,
	const bool& isReverseX,const bool& isReverseY, float rotation, ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial)
{
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_RESOURCE_DESC resDesc{};
	resDesc = texBuff[(textureHandle - srvGpuHandle.ptr) / Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type)]->GetDesc();

	Vec2 length = { (float)resDesc.Width ,(float)resDesc.Height };

	//���]
	if (isReverseX)length.x *= -1;
	if (isReverseY)length.y *= -1;

	float texLeft = UVleftTop.x * +(float)length.x * scale;
	float texRight = (UVleftTop.x + UVlength.x) * +(float)length.x * scale;
	float texTop = UVleftTop.y * +(float)length.y * scale;
	float texBottom = (UVleftTop.y + UVlength.y) * +(float)length.y * scale;

	if (isPosLeftTop)
	{
		//���ォ��̍��W
		vertices[0] = { {0,UVlength.y * length.y * scale,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//����
		vertices[1] = { {0,0,0.0f},{UVleftTop.x,UVleftTop.y} };//����
		vertices[2] = { {UVlength.x * length.x * scale,UVlength.y * length.y * scale,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//�E��
		vertices[3] = { {UVlength.x * length.x * scale,0,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//�E��
	}
	else
	{
		//�؂蔲������̉摜�̒��S����̈ʒu�I�I�I�I�I�I�I�I
		vertices[0] = { {-UVlength.x * length.x * scale / 2.0f,UVlength.y * length.y * scale / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y + UVlength.y} };//����
		vertices[1] = { {-UVlength.x * length.x * scale / 2.0f,-UVlength.y * length.y * scale / 2.0f,0.0f},{UVleftTop.x,UVleftTop.y} };//����
		vertices[2] = { {UVlength.x * length.x * scale / 2.0f,UVlength.y * length.y * scale / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y + UVlength.y} };//�E��
		vertices[3] = { {UVlength.x * length.x * scale / 2.0f,-UVlength.y * length.y * scale / 2.0f,0.0f},{UVleftTop.x + UVlength.x,UVleftTop.y} };//�E��
	}
	constMapMaterial->color = color;

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
		worldMat.trans = { leftTop.x + texLeft + UVlength.x * (float)length.x * scale / 2.0f,
			leftTop.y + texTop + UVlength.y * (float)length.y * scale / 2.0f,
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

	cbt->constMapTransform->mat = matW * view.matView * projection.matProjection;
}



