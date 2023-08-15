#pragma once
#include"TextureManager.h"
#include<memory>
#include"Vec3.h"

//�A�t�B���ϊ��s��p
class ConstBuffTransform
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private:
	//�萔�o�b�t�@�p�f�[�^�\��
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj; //�r���[�v���W�F�N�V�����s��
		XMMATRIX world;    //���[���h�s��
		Vec3 cameraPos;//�J�������W�i���[���h���W�j
		float pad;
	};


public:
	//�ۑ����Ă����f�[�^
	struct ConstBuffTransformSaveData
	{
		bool isUsing = false;//���g�p����Ă��邩
		ConstBufferDataTransform* constMapTransform = nullptr;//�}�b�s���O
		ComPtr<ID3D12Resource> constBuffTransform = nullptr;//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
	};
	//�`�掞�Ɏg���f�[�^
	struct ConstBuffTransformDrawData
	{
		D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUHandle;//gpu�̃n���h��
		ConstBufferDataTransform* constMapTransform = nullptr;//�}�b�s���O
	};
	//�f�[�^�܂Ƃ�
	struct ConstBuffTransformAllData
	{
		ConstBuffTransformSaveData saveData;
		ConstBuffTransformDrawData drawData;
	};

private://�Ǘ��p�ÓI�ϐ�
	static D3D12_DESCRIPTOR_RANGE sDescRange_;
	//�������Ǘ��p
	static std::map<uint64_t, ConstBuffTransformSaveData> sCbtDatas_;

private://�`��p
	//�`�掞�ɃZ�b�g����gpu�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUHandle_;
	ConstBufferDataTransform* constMapTransform_ = nullptr;//�萔�o�b�t�@�̃}�b�s���O�p�|�C���^

public:
	ConstBuffTransform() { ; }
	~ConstBuffTransform();

	static void StaticInitialize();
	void Initialize();
	void DrawCommand(int32_t index);

public:
	void SetViewProjMat(const XMMATRIX& mat) { constMapTransform_->viewproj = mat; }
	void SetWorldMat(const XMMATRIX& mat) { constMapTransform_->world = mat; }
	void SetCameraPos(const Vec3& pos) { constMapTransform_->cameraPos = pos; }

public:
	//�o�b�t�@���쐬���ēo�^
	static ConstBuffTransformDrawData CreateBuffAndAdd();

private:
	//�ۑ����Ă���΃f�[�^��Ԃ�
	static bool GetSaveData(ConstBuffTransformDrawData& data);
	//�V���Ƀo�b�t�@���쐬���ĕԂ�
	static ConstBuffTransformDrawData CreateBuff();
	static ConstBuffTransformAllData CreateBuffInternal();

public:
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetCBVGpuHandle() { return cbvGPUHandle_; }
	static const D3D12_DESCRIPTOR_RANGE& GetDescRange() { return sDescRange_; }
};