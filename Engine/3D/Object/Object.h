#pragma once

#include "Util.h"
#include "Primitive.h"
#include "LightManager.h"
#include"CollisionInfo.h"
#include "PostPera.h"
#include <FbxLoader.h>
#include "Camera.h"
#include "RootPipe.h"

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
	XMFLOAT4 rimColor = { 1.0f,1.0f,1.0f,0 };
	//�^���V���G�b�g
	uint32_t isSilhouette = false;
	//����
	uint32_t time = 0;
};

class Object
{
private:
	//���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC resDesc{};
	ConstBuffTransform cbt_;//�������ǂ��ɂ�����΁A�C���X�^���X��ł��F�X�`��

	//�}�`�̃N���X
	static Primitive primitive_;
	//
	static RootPipe objPipeLineSet_[3];
	//
	static RootPipe pipelineSet_;
	//al4_02_02
	static RootPipe pipelineSetM_;
	//FBX�p
	static RootPipe pipelineSetFBX_;
	//���[�g�p�����[�^�̐ݒ�
	static D3D12_ROOT_PARAMETER rootParams_[7];
	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_;

	// 2.�`���̕ύX
		// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
	static D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

	//�萔�o�b�t�@�̐���
	ComPtr < ID3D12Resource> constBuffMaterial_ = nullptr;
	//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;

	//�X�v���C�g
	std::unique_ptr<Sprite> sprite_;

	//���C�g
	static LightManager* sLightManager_;

	//�{�[���̍ő吔
	static const int32_t S_MAX_BONES_ = 32;
	//�萔�o�b�t�@�i�X�L���j
	ComPtr<ID3D12Resource> constBuffSkin_ = nullptr;

	//1�t���[���̎���
	FbxTime frameTime_;
	//�A�j���[�V�����J�n����
	FbxTime startTime_;
	//�A�j���[�V�����I������
	FbxTime endTime_;
	//�A�j���[�V�������ݎ���
	FbxTime currentTime_;
	//�A�j���[�V�����t���O
	bool isPlay_ = false;
	//���[�v
	bool isLoop_ = false;
	//�t�Đ�
	bool isReverse_ = false;

	//
	static float sRimColorF3_[3];

	//�����Ă�t���O
	bool isAlive_ = true;
	//
	std::unique_ptr<WorldMat> worldMat_ = std::make_unique<WorldMat>();
	//�����̐��ʃx�N�g��
	Vec3 frontVec_ = { 0,0,-1.0f };
	Vec3 frontVecTmp_ = { 0,0,-1.0f };

	//��
	Vec3 velocity_ = { 0,0,0 };

public://�ϐ�
	bool isWireFrame_ = 0;
	//��ʌ��ʗp
	EffectOConstBuffer sEffectFlags_;
	//��ʌ��ʗp
	ComPtr <ID3D12Resource> sEffectFlagsBuff_;
	EffectOConstBuffer* sMapEffectFlagsBuff_;

public:
	//�萔�o�b�t�@�p�f�[�^�\���́i�X�L�j���O�j
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[S_MAX_BONES_];
	};

protected://�p����܂Ō��J
	//�N���X��(�f�o�b�O�p)
	std::string objName_;
	//�R���C�_�[
	std::unique_ptr<BaseCollider> collider_ = nullptr;


private:
	//--------------------
	void Update(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, const ConstBuffTransform& constBuffTransform,
		Camera* camera, Model* model = nullptr, ModelFBX* fbx = nullptr, bool primitiveMode = true);

	//�s�񑗐M
	void SendingMat(int32_t indexNum, Camera* camera);

	//�{�[���̃f�[�^�]��
	void SendingBoneData(ModelFBX* model);

	//���[�g�V�O�l�`���n�̃R�}���h
	void SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature);
	//�}�e���A���A���C�g�A�e�N�X�`���n�̃R�}���h
	void SetMaterialLightMTexSkin(uint64_t textureHandle, ConstBuffTransform cbt);
	void SetMaterialLightMTexSkinModel(uint64_t textureHandle, ConstBuffTransform cbt, Material* material);

	//
	static void PipeLineState(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe, int32_t indexNum = NULL);

	static void Blend(const D3D12_BLEND_OP& blendMode,
		bool Inversion = 0, bool Translucent = 0);

public:
	//�R���X�g���N�^
	Object();

	//�f�X�g���N�^
	virtual ~Object();

	//�ʒu
	void SetTrans(const Vec3& pos) { worldMat_->trans_ = pos; }
	void SetTransX(float pos) { worldMat_->trans_.x_ = pos; }
	void SetTransY(float pos) { worldMat_->trans_.y_ = pos; }
	void SetTransZ(float pos) { worldMat_->trans_.z_ = pos; }
	const Vec3& GetTrans() { return worldMat_->trans_; }
	//�X�P�[��
	void SetScale(const Vec3& scale) { worldMat_->scale_ = scale; }
	void SetScaleX(float scale) { worldMat_->scale_.x_ = scale; }
	void SetScaleY(float scale) { worldMat_->scale_.y_ = scale; }
	void SetScaleZ(float scale) { worldMat_->scale_.z_ = scale; }
	const Vec3& GetScale() { return worldMat_->scale_; }
	//��]
	void SetRot(const Vec3& rot) {
		worldMat_->rot_ = rot;
	}
	void SetRotX(float rot) { worldMat_->rot_.x_ = rot; }
	void SetRotY(float rot) { worldMat_->rot_.y_ = rot; }
	void SetRotZ(float rot) { worldMat_->rot_.z_ = rot; }
	const Vec3& GetRot() { return worldMat_->rot_; }
	//�s����X�V
	void CulcWorldMat() { worldMat_->CulcWorldMat(); }
	void CulcRotMat() { worldMat_->CulcRotMat(); }
	void CulcTransMat() { worldMat_->CulcTransMat(); }
	void CulcScaleMat() { worldMat_->CulcScaleMat(); }
	//���[���h�s��̒��g�R�s�[
	void SetWorldMat(std::unique_ptr<WorldMat> worldMat) { worldMat_ = std::move(worldMat); }
	//�e
	void SetParent(Object* obj) { worldMat_->parent_ = obj->GetWorldMat(); }
	void SetParent(WorldMat* worldMat) { worldMat_->parent_ = worldMat; }
	WorldMat* GetParent() { return worldMat_->parent_; }
	//
	WorldMat* GetWorldMat() { return worldMat_.get(); }
	//�e�q�֌W���l�������ʒu���Q�b�g
	Vec3 GetWorldTrans() { return worldMat_->GetWorldTrans(); }

	//�I�u�W�F�N�g���O
	void SetObjName(std::string objName) { objName_ = objName; }
	const std::string& GetObjName() { return objName_; }
	//�R���C�_�[
	BaseCollider* GetCollider() { return collider_.get(); }

	//�����Ă邩
	inline void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
	inline bool GetIsAlive() { return isAlive_; }

	//���ʃx�N�g��
	//�I�u�W�F�N�g�̊p�x�ŉ�]���������ʃx�N�g�����Q�b�g
	const Vec3& GetFrontVec();
	void CulcFrontVec();
	const Vec3& GetFrontVecTmp() { return frontVecTmp_; }
	inline void SetFrontVecTmp(const Vec3& vec) { frontVecTmp_ = vec; }
	inline void SetFrontVec(const Vec3& vec) { frontVec_ = vec; }

	//�X�s�[�h�i�����蔻��Ɏg���j
	inline void SetVelocity(const Vec3& vec) { velocity_ = vec; }
	inline const Vec3& GetVelocity() { return velocity_; }

	//�N�H�[�^�j�I���n
	Quaternion GetQuaternion() { return worldMat_->GetQuaternion(); }
	void SetIsUseQuaternionMatRot(bool is) { worldMat_->SetIsUseQuaMatRot(is); }
	void SetMatRot(const M4& m) { worldMat_->SetRotMat(m); }

	//������
	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat = nullptr);

	virtual void Update();

	static void StaticUpdate();

	virtual void Draw();

	//���[���h�s��̎擾
	const M4& GetMatWorld() { return worldMat_->matWorld_; }

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

	//�t�H�O�Ƃ��̃t���O
	void SetIsSilhouette(bool is) { sEffectFlags_.isSilhouette = is; }

	//-------------

	void DrawTriangle(/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,*/
		Camera* camera = nullptr, const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBox(Camera* camera = nullptr, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBoxSprite(const Vec2& pos, float scale, const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }
		, uint64_t textureHandle = NULL, const Vec2& ancorUV = { 0,0 }, bool isReverseX = false, bool isReverseY = false,
		float rotation = 0.0f, int32_t pipelineNum = 0);

	void DrawClippingBoxSprite(const Vec2& leftTop, float scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, bool isPosLeftTop = true,
		bool isReverseX = false, bool isReverseY = false, float rotation = 0.0f, int32_t pipelineNum = 0);

	void DrawCube3D(Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawLine(/*const Vec3& pos1, const Vec3& pos2, */Camera* camera = nullptr, const XMFLOAT4& color
		= { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL);

	void DrawCircle(Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawSphere(Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawModel(Model* model, Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, int32_t pipelineNum = 0);

	void DrawFBX(ModelFBX* modelFbx, Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, int32_t pipelineNum = 0);

	//�F��Ԃ�
	const XMFLOAT4& GetColor() { return constMapMaterial_->color; }
	/// <summary>
	/// ���C�g�̃Z�b�g
	/// </summary>
	/// <param name="light"></param>
	static void SetLight(LightManager* lightManager) { Object::sLightManager_ = lightManager; }

	//
	//�D�悵�čŏ��̕��ɏ�����
	static void DrawInitialize();

private:
	void constBuffTransfer(const XMFLOAT4& plusRGBA);
};
void SetNormDigitalMat(XMMATRIX& mat);

void Error(bool filed, ID3DBlob* errorBlob);