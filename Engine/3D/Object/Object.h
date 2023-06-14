#pragma once

#include "Util.h"
#include "Primitive.h"
#include "LightManager.h"
#include"CollisionInfo.h"
#include "PostPera.h"
#include <FbxLoader.h>

class BaseCollider;


/// <summary>
/// ���_�C���f�b�N�X�p
/// </summary>
enum indices
{
	TRIANGLE,
	BOX,
	CUBE,
	LINE,
	CIRCLE,
	SPHERE,
	SPRITE,
	MODEL,
	FBX
};

//��ʌ��ʗp�̃t���O
struct EffectOConstBuffer
{
	//�t�H�O
	uint32_t isFog = false;
	//�g�D�[��
	uint32_t isToon = false;
	//�������C�g
	uint32_t isRimLight = false;
	float pad1;
	//�����̐F
	XMFLOAT3 rimColor = { 1.0f,1.0f,1.0f };
	float pad2;
	//����
	uint32_t time = 0;
};

class Object
{
private:
	//���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC resDesc{};
	ConstBuffTransform cbt;//�������ǂ��ɂ�����΁A�C���X�^���X��ł��F�X�`��


	//�萔�o�b�t�@�̐���
	ComPtr < ID3D12Resource> constBuffMaterial = nullptr;
	//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	//�X�v���C�g
	std::unique_ptr<Sprite> sprite_;

	//���C�g
	static LightManager* lightManager;

	//�{�[���̍ő吔
	static const int32_t MAX_BONES = 32;
	//�萔�o�b�t�@�i�X�L���j
	ComPtr<ID3D12Resource> constBuffSkin = nullptr;

	//1�t���[���̎���
	FbxTime frameTime;
	//�A�j���[�V�����J�n����
	FbxTime startTime;
	//�A�j���[�V�����I������
	FbxTime endTime;
	//�A�j���[�V�������ݎ���
	FbxTime currentTime;
	//�A�j���[�V�����t���O
	bool isPlay = false;
	//���[�v
	bool isLoop = false;
	//�t�Đ�
	bool isReverse = false;

	//
	static float rimColorF3[3];

	std::unique_ptr<WorldMat> worldMat_ = std::make_unique<WorldMat>();

public://�ϐ�
	ViewMat* view;
	ProjectionMat* projection;
	bool isWireFrame = 0;
	//��ʌ��ʗp
	static EffectOConstBuffer effectFlags;
	//��ʌ��ʗp
	static ComPtr <ID3D12Resource> effectFlagsBuff;
	static EffectOConstBuffer* mapEffectFlagsBuff;

public:
	//�萔�o�b�t�@�p�f�[�^�\���́i�X�L�j���O�j
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

protected://�p����܂Ō��J
	//�N���X��(�f�o�b�O�p)
	const char* name = nullptr;
	//�R���C�_�[
	std::unique_ptr<BaseCollider> collider_ = nullptr;


private:
	//--------------------
	void Update(const int32_t& indexNum, const int32_t& pipelineNum, const uint64_t textureHandle, const ConstBuffTransform& constBuffTransform,
		Model* model = nullptr, ModelFBX* fbx = nullptr, const bool& primitiveMode = true);

	//�s�񑗐M
	void SendingMat(int32_t indexNum);

	//�{�[���̃f�[�^�]��
	void SendingBoneData(ModelFBX* model);

	//���[�g�V�O�l�`���n�̃R�}���h
	void SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature);
	//�}�e���A���A���C�g�A�e�N�X�`���n�̃R�}���h
	void SetMaterialLightMTexSkin(uint64_t textureHandle_, ConstBuffTransform cbt);
	void SetMaterialLightMTexSkinModel(uint64_t textureHandle_, ConstBuffTransform cbt, Material* material);


public:
	//�R���X�g���N�^
	Object();

	//�f�X�g���N�^
	virtual ~Object();

	//�ʒu
	void SetTrans(const Vec3& pos) { worldMat_->trans = pos; }
	void SetTransX(float pos) { worldMat_->trans.x = pos; }
	void SetTransY(float pos) { worldMat_->trans.y = pos; }
	void SetTransZ(float pos) { worldMat_->trans.z = pos; }
	const Vec3& GetTrans() { return worldMat_->trans; }
	//�X�P�[��
	void SetScale(const Vec3& scale) { worldMat_->scale = scale; }
	void SetScaleX(float scale) { worldMat_->scale.x = scale; }
	void SetScaleY(float scale) { worldMat_->scale.y = scale; }
	void SetScaleZ(float scale) { worldMat_->scale.z = scale; }
	const Vec3& GetScale() { return worldMat_->scale; }
	//��]
	void SetRot(const Vec3& rot) { worldMat_->rot = rot; }
	void SetRotX(float rot) { worldMat_->rot.x = rot; }
	void SetRotY(float rot) { worldMat_->rot.y = rot; }
	void SetRotZ(float rot) { worldMat_->rot.z = rot; }
	const Vec3& GetRot() { return worldMat_->rot; }
	//�s����X�V
	void CulcWorldMat() { worldMat_->SetWorld(); }
	//���[���h�s��̒��g�R�s�[
	void SetWorldMat_(WorldMat worldMat) { *worldMat_ = worldMat; }

	//������
	virtual bool Initialize();

	virtual void Update();

	static void StaticUpdate();

	virtual void Draw();

	//���[���h�s��̎擾
	const M4& GetMatWorld() { return worldMat_->matWorld; }

	//�R���C�_�[�̃Z�b�g
	void SetCollider(std::unique_ptr<BaseCollider> collider);

	//�����L���ɂ��邩
	void SetIsValid(bool isValid);
	//�X�v���C�g���ǂ���
	void SetIs2D(bool is2D);

	//�Փˎ��R�[���o�b�N�֐�
	virtual void OnCollision(const CollisionInfo& info) {}

	//�A�j���[�V�����J�n
	void PlayAnimationInternal(ModelFBX* model, FbxTime& sTime, FbxTime& eTime,
		bool isLoop = false, bool isReverse = false);
	void PlayAnimation(ModelFBX* model, bool isLoop = false);
	void PlayReverseAnimation(ModelFBX* model, bool isLoop = false);

	//-------------

	void DrawTriangle(/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,*/
		ViewMat* view, ProjectionMat* projection, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f },
		const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawBox(ViewMat* view, ProjectionMat* projection, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f },
		const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawBoxSprite(const Vec3& pos, const float& scale, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }
		, const uint64_t textureHandle = NULL, const Vec2& ancorUV = { 0,0 }, const bool& isReverseX = false, const bool& isReverseY = false,
		float rotation = 0.0f, const int32_t& pipelineNum = 0);

	void DrawClippingBoxSprite(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL, bool isPosLeftTop = true,
		const bool& isReverseX = false, const bool& isReverseY = false, float rotation = 0.0f, const int32_t& pipelineNum = 0);

	void DrawCube3D(ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawLine(/*const Vec3& pos1, const Vec3& pos2, */ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color
		= { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL);

	void DrawCircle(ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawSphere(ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawModel(ViewMat* view, ProjectionMat* projection,
		Model* model, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const int32_t& pipelineNum = 0);

	void DrawFBX(ViewMat* view, ProjectionMat* projection,
		ModelFBX* modelFbx, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const int32_t& pipelineNum = 0);

	//�F��Ԃ�
	XMFLOAT4 GetColor() { return constMapMaterial->color; }
	/// <summary>
	/// ���C�g�̃Z�b�g
	/// </summary>
	/// <param name="light"></param>
	static void SetLight(LightManager* lightManager) { Object::lightManager = lightManager; }

private:
	void constBuffTransfer(const XMFLOAT4& plusRGBA);
};

//�D�悵�čŏ��̕��ɏ�����
void DrawInitialize();

//
void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, const int32_t& indexNum = NULL);

void Blend(const D3D12_BLEND_OP& blendMode,
	const bool& Inversion = 0, const bool& Translucent = 0);

void SetNormDigitalMat(XMMATRIX& mat);

void Error(const bool& filed);