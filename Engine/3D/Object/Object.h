#pragma once
#include "IObject3D.h"
#include "Primitive.h"


//�I�u�W�F�N�g�N���X
class Object : public IObject3D
{
private:
	// �I�u�W�F�N�g�̃^�C�v
	enum ObjType
	{
		TRIANGLE,
		BOX,
		CUBE,
		LINE,
		CIRCLE,
		SPHERE,
		SPRITE,
		OBJ,
		FBX
	};

private:
	// ���_���C�A�E�g�̐ݒ�
// ���_���C�A�E�g
	static D3D12_INPUT_ELEMENT_DESC sInputLayout_[5];

private:
	//�}�`�̃N���X
	static Primitive primitive_;
	//�p�C�v���C���Ȃǂ̐ݒ�
	static RootPipe primitivePipeLineSet_[3];
	//.obj��
	static RootPipe pipelineSetM_[2];

private:
	//�`�悷��`����w�肳�ꂽ��Ăԕ`��p�֐�
	void DrawUpdate(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, ConstBuffTransform* constBuffTransform,
		Camera* camera, IModel* model = nullptr, bool isWireFrame = false);

public:
	//�R���X�g���N�^
	Object();

	//�f�X�g���N�^
	virtual ~Object();

public:
	//�D�悵�čŏ��̕��ɏ�����
	static void CommonInitialize();

	virtual void Update()override;

	void DrawModelInternal(int32_t pipelineNum)override;

	virtual void Draw()override;

public:
	void DrawTriangle(Camera* camera = nullptr,
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBox(Camera* camera = nullptr,

		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawCube3D(Camera* camera = nullptr,
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawLine(Camera* camera = nullptr,
		uint64_t textureHandle = NULL);

	void DrawCircle(Camera* camera = nullptr,
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawSphere(Camera* camera = nullptr,
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawModel(IModel* model, Camera* camera = nullptr,
		int32_t pipelineNum = 0);
};
