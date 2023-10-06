#pragma once
#include"TextureManager.h"
#include<memory>
#include"Vec3.h"

//�A�t�B���ϊ��s��p
class ConstBuffTransform
{
protected://�G�C���A�X
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

private://�`��p
	//�o�b�t�@
	ComPtr<ID3D12Resource> constBuffTransform_;
	ConstBufferDataTransform* constMapTransform_ = nullptr;//�萔�o�b�t�@�̃}�b�s���O�p�|�C���^

public:
	ConstBuffTransform() { ; }
	~ConstBuffTransform() { ; }

	void Initialize();
	void DrawCommand(int32_t index);

public:
	void SetViewProjMat(const XMMATRIX& mat) { constMapTransform_->viewproj = mat; }
	void SetWorldMat(const XMMATRIX& mat) { constMapTransform_->world = mat; }
	void SetCameraPos(const Vec3& pos) { constMapTransform_->cameraPos = pos; }

	const XMMATRIX& GetWorldMat() const{ return constMapTransform_->world; }
	const XMMATRIX& GetViewProjMat()const { return constMapTransform_->viewproj; }
	const Vec3& GetCameraPos()const { return constMapTransform_->cameraPos; }
};