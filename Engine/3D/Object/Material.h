#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#include "DirectXWrapper.h"

/// <summary>
/// �}�e���A��
/// </summary>
class Material
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X
	struct ConstBufferDataMaterial2
	{
		XMFLOAT3 ambient;
		float pad1;       //�p�f�B���O
		XMFLOAT3 diffuse;
		float pad2;       //�p�f�B���O
		XMFLOAT3 specular;
		float alpha;
	};

public: // �ÓI�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* pDevice);

	/// <summary>
	/// �}�e���A������
	/// </summary>
	/// <returns>�������ꂽ�}�e���A��</returns>
	static std::unique_ptr<Material> Create();

private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* pSDevice_;

public:
	std::string name_;	// �}�e���A����
	XMFLOAT3 ambient_;	// �A���r�G���g�e���x
	XMFLOAT3 diffuse_;	// �f�B�t���[�Y�e���x
	XMFLOAT3 specular_;	// �X�y�L�����[�e���x
	float alpha_;		// �A���t�@
	std::string textureFilename_;	// �e�N�X�`���t�@�C����
	uint64_t textureHandle_;//�摜�n���h��

public:
	~Material();
	// �R���X�g���N�^
	Material() {
		ambient_ = { 0.3f, 0.3f, 0.3f };
		diffuse_ = { 0.0f, 0.0f, 0.0f };
		specular_ = { 0.0f, 0.0f, 0.0f };
		alpha_ = 1.0f;
		textureHandle_ = NULL;
	}

	/// <summary>
	/// �萔�o�b�t�@�̎擾
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetConstantBuffer() { return constBuff_.Get(); }

	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="directoryPath">�ǂݍ��݃f�B���N�g���p�X</param>
	/// <param name="cpuHandle">CPU�f�X�N���v�^�n���h��</param>
	/// <param name="gpuHandle">GPU�f�X�N���v�^�n���h��</param>
	void LoadTexture(const std::string& directoryPath, const CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle);


	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	const CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return cpuDescHandleSRV_; }
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle();

private:
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff_;
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff_;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;

private:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	void CreateConstantBuffer();
};

