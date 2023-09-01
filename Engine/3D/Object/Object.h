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
enum objType
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

//���o�p�̃t���O
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
	DirectX::XMFLOAT4 rimColor = { 1.0f,1.0f,1.0f,0 };
	//�^���V���G�b�g
	uint32_t isSilhouette = false;
	//�f�B�]���u
	uint32_t isDissolve = false;
	//�f�B�]���u����
	float dissolveT = 0;
	//�X�y�L�����}�b�v
	uint32_t isSpecularMap = false;
	//�m�[�}���}�b�v
	uint32_t isNormalMap = false;
	//����
	uint32_t time = 0;
};

class Object
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

public:
	enum ROOTPARAM_NUM
	{
		COLOR,
		TEX,
		MATRIX,
		MESH_MAT,
		MATERIAL,
		LIGHT,
		EFFECT,
		SKIN,
		DISSOLVE,
		SPECULAR_MAP,
		NORM_MAP
	};

public:
	//�{�[���̍ő吔
	static const int32_t S_MAX_BONES_ = 90;

	//�萔�o�b�t�@�p�f�[�^�\���́i�X�L�j���O�j
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[S_MAX_BONES_];
	};

private:
	//���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC resDesc{};
	ConstBuffTransform cbt_;//�������ǂ��ɂ�����΁A�C���X�^���X��ł��F�X�`��

	//�}�`�̃N���X
	static Primitive primitive_;
	//
	static RootPipe objPipeLineSet_[3];
	//
	static RootPipe spritePipelineSet_;
	//al4_02_02
	static RootPipe pipelineSetM_;
	//FBX�p
	static RootPipe pipelineSetFBX_;
	//���[�g�p�����[�^�̐ݒ�
	static D3D12_ROOT_PARAMETER rootParams_[11];
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

	//�萔�o�b�t�@�i�X�L���j
	ComPtr<ID3D12Resource> constBuffSkin_ = nullptr;
	ConstBufferDataSkin* constMapSkin = nullptr;

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
	//�Đ��̃X�s�[�h�{��
	float animationSpeed_ = 1.0f;

	//�e�q�֌W�����ԃ��f���̃m�[�h
	const Node* parentNode_ = nullptr;
	ModelFBX* parentNodeModel_ = nullptr;

	//�����Ă�t���O
	bool isAlive_ = true;
	//
	std::unique_ptr<WorldMat> worldMat_ = std::make_unique<WorldMat>();
	//�����̐��ʃx�N�g��
	Vec3 frontVec_ = { 0,0,-1.0f };
	Vec3 frontVecTmp_ = { 0,0,-1.0f };

	//��
	Vec3 velocity_ = { 0,0,0 };

	//���f���̃|�C���^
	IModel* model_ = nullptr;

	//�f�B�]���u�̉摜�n���h��
	uint64_t dissolveTextureHandle_ = NULL;
	//�X�y�L�����}�b�v�̉摜
	uint64_t specularMapTextureHandle_ = NULL;
	//�m�[�}���}�b�v�̉摜
	uint64_t normalMapTextureHandle_ = NULL;

	//�`��A�X�V���邩�ǂ���
	bool isValid_ = true;

	//���b�V���̃I�t�Z�b�g
	Mesh::PolygonOffset meshOffsetData_;

	//��ʌ��ʗp
	ComPtr <ID3D12Resource> effectFlagsBuff_;
	EffectOConstBuffer* mapEffectFlagsBuff_;

	Object* parentObj_ = nullptr;

public://�ϐ�
	bool isWireFrame_ = 0;
	//��ʌ��ʗp
	EffectOConstBuffer effectFlags_;

protected://�p����܂Ō��J
	//�N���X��(�f�o�b�O�p)
	std::string objName_;
	//�R���C�_�[
	std::unique_ptr<BaseCollider> collider_ = nullptr;


private:
	//--------------------
	void Update(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, ConstBuffTransform* constBuffTransform,
		Camera* camera, IModel* model = nullptr, bool primitiveMode = true);

	//�s�񑗐M
	void SendingMat(int32_t indexNum, Camera* camera, IModel* model = nullptr);

	void SendingMat(int32_t indexNum, Camera* camera, const XMMATRIX* mat);

	//���[�g�V�O�l�`���n�̃R�}���h
	void SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature);
	//�}�e���A���A���C�g�A�e�N�X�`���n�̃R�}���h
	void SetMaterialLightMTexSkin(uint64_t textureHandle, uint64_t dissolveTex, uint64_t specularMapTex,
		uint64_t normalMapTex, bool setTex = true);
	void SetMaterialLightMTexSkinModel(uint64_t dissolveTexHandle, uint64_t specularMapTexhandle,
		uint64_t normalMapTexHandle);

	//�A�j���[�V�����J�n
	void PlayAnimationInternal(FbxTime& sTime, FbxTime& eTime,
		bool isLoop = false, bool isReverse = false);
	//�A�j���[�V�������Z�b�g
	void AnimationReset(FbxTime& sTime, FbxTime& eTime);

	//�{�[���̃A�j���[�V��������
	void CalcBoneDataInternal(ModelFBX* model);
	//�{�[���̍s����v�Z
	XMMATRIX GetCalcSkinMat(const std::vector<ModelFBX::Bone>& bones, int32_t index);

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
	Vec3 GetTrans() { return worldMat_->trans_; }
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
	void CalcWorldMat() { worldMat_->CalcWorldMat(); }
	void CalcRotMat() { worldMat_->CalcRotMat(); }
	void CalcTransMat() { worldMat_->CalcTransMat(); }
	void CalcScaleMat() { worldMat_->CalcScaleMat(); }
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

	//���f���̃|�C���^
	void SetModel(IModel* model) { model_ = model; }
	IModel* GetModel() { return model_; }

	//�����Ă邩
	void SetIsAlive(bool isAlive);
	bool GetIsAlive() { return isAlive_; }

	//�`��A�X�V���邩�ǂ���
	virtual void SetIsValid(bool isValid);
	bool GetIsValid() { return isValid_; }

	//���b�V���̃I�t�Z�b�g�p
	void SetMeshPolygonOffsetData(const Mesh::PolygonOffset& offsetData) { meshOffsetData_ = offsetData; }

	//���ʃx�N�g��
	//�I�u�W�F�N�g�̊p�x�ŉ�]���������ʃx�N�g�����Q�b�g
	const Vec3& GetFrontVec();
	void CalcFrontVec();
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

	//�f�B�]���u�摜�n���h��
	void SetDissolveTexHandle(uint64_t dissolveTextureHandle) { dissolveTextureHandle_ = dissolveTextureHandle; }
	//�f�B�]���u����
	void SetDissolveT(float t) { effectFlags_.dissolveT = t; }
	void SetisDissolve(bool isDissolve) { effectFlags_.isDissolve = isDissolve; }

	//�X�y�L�����}�b�v
	void SetIsSpecularMap(bool isSpecularMap) { effectFlags_.isSpecularMap = isSpecularMap; }
	void SetSpecularMapTexHandle(uint64_t specularMapTextureHandle) { specularMapTextureHandle_ = specularMapTextureHandle; }

	//�m�[�}���}�b�v
	void SetIsNormalMap(bool isNormalMap) { effectFlags_.isNormalMap = isNormalMap; }
	void SetNormalMapTexHandle(uint64_t normalMapTextureHandle) { normalMapTextureHandle_ = normalMapTextureHandle; }

	//--------------------------------------------------------
	//������
	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat = nullptr);

	virtual void Update();

	virtual void Draw();

	//���o�n�̃A�b�v�f�[�g
	void EffectUpdate();
	//�s����X�V�A����ɔ����R���C�_�[��
	void WorldMatColliderUpdate();

	static void StaticUpdate();
	//--------------------------------------------------------

	//���[���h�s��̎擾
	const M4& GetMatWorld() { return worldMat_->matWorld_; }

	//�R���C�_�[�̃Z�b�g
	void SetCollider(std::unique_ptr<BaseCollider> collider);

	//�����L���ɂ��邩
	void SetColliderIsValid(bool isValid);
	//�X�v���C�g���ǂ���
	void SetIs2D(bool is2D);

	//�Փˎ��R�[���o�b�N�֐�
	virtual void OnCollision(const CollisionInfo& info) {}

	//�A�j���[�V�����J�n
	void PlayAnimation(bool isLoop = false);
	void PlayReverseAnimation(bool isLoop = false);
	//�A�j���[�V�����t���O
	void SetIsPlayAnimation(bool isPlay) { isPlay_ = isPlay; }
	void SetIsLoopAnimation(bool isLoop) { isLoop_ = isLoop; }
	void SetIsReverseAnimation(bool isReverse) { isReverse_ = isReverse; }
	//�A�j���[�V�����X�s�[�h
	void SetAnimationSpeed(float speed) { animationSpeed_ = speed; }
	//���f���̕��ʂƐe�q�֌W����������
	void ParentFbxNode(Object* obj, IModel* model, const std::string& nodeName);
	//�{�[���𓾂�
	const XMMATRIX* GetModelBones()const { return constMapSkin->bones; }
	//�{�[���̃f�[�^�]��
	void MappingBoneData(ModelFBX* model);

	//�t�H�O�Ƃ��̃t���O
	void SetIsSilhouette(bool is) { effectFlags_.isSilhouette = is; }

	//-------------

	void DrawTriangle(Camera* camera = nullptr, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBox(Camera* camera = nullptr, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBoxSprite(const Vec2& pos, const Vec2& scale, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }
		, uint64_t textureHandle = NULL, const Vec2& ancorUV = { 0,0 }, bool isReverseX = false, bool isReverseY = false,
		float rotation = 0.0f, int32_t pipelineNum = 0);

	void DrawClippingBoxSprite(const Vec2& leftTop, const Vec2& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, bool isPosLeftTop = true,
		bool isReverseX = false, bool isReverseY = false, float rotation = 0.0f, int32_t pipelineNum = 0);

	void DrawCube3D(Camera* camera = nullptr,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawLine(Camera* camera = nullptr, const Vec4& color
		= { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL);

	void DrawCircle(Camera* camera = nullptr,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawSphere(Camera* camera = nullptr,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawModel(IModel* model, Camera* camera = nullptr,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, int32_t pipelineNum = 0);

	//�f�o�b�O�p�̕\��
	virtual void DrawImGui(std::function<void()>imguiF = NULL);

	//�F��Ԃ�
	const Vec4& GetColor() { return constMapMaterial_->color; }
	/// <summary>
	/// ���C�g�̃Z�b�g
	/// </summary>
	/// <param name="light"></param>
	static void SetLight(LightManager* lightManager) { Object::sLightManager_ = lightManager; }

	//
	//�D�悵�čŏ��̕��ɏ�����
	static void DrawInitialize();

};
void SetNormDigitalMat(DirectX::XMMATRIX& mat);

void Error(bool filed, ID3DBlob* errorBlob);