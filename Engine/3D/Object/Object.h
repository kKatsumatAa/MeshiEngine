#pragma once
#include "IObject3D.h"
#include "Primitive.h"


/// <summary>
/// �I�u�W�F�N�g�̃^�C�v
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

//�I�u�W�F�N�g�N���X
class Object : public IObject3D
{


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



private:
	//�}�`�̃N���X
	static Primitive primitive_;
	//
	static RootPipe objPipeLineSet_[3];
	//al4_02_02
	static RootPipe pipelineSetM_;

	//���[�g�p�����[�^�̐ݒ�
	static D3D12_ROOT_PARAMETER rootParams_[11];

	//�萔�o�b�t�@�̐���
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
	//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;

	//�X�v���C�g
	std::unique_ptr<Sprite> sprite_ = nullptr;

	//���C�g
	static LightManager* sLightManager_;

private:
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
	void PlayAnimationInternal(int32_t animeIndex,
		bool isLoop = false, bool isReverse = false);
	//�A�j���[�V�������Z�b�g
	void AnimationReset(int32_t animeIndex);

	//�A�j���[�V�����ŕω������m�[�h��{�[���̏���
	void CalcNodeMatBoneMatInternal(ModelFBX* model);
	//�{�[���̍s����v�Z
	XMMATRIX GetCalcSkinMat(IModel* model, int32_t index);

	static void Blend(const D3D12_BLEND_OP& blendMode,
		bool Inversion = 0, bool Translucent = 0);

public:
	//�R���X�g���N�^
	Object();

	//�f�X�g���N�^
	virtual ~Object();

	//���f���̃|�C���^
	void SetModel(IModel* model);
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
	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat = nullptr, IModel* model = nullptr);

	virtual void Update();

	virtual void Draw();

	//���o�n�̃A�b�v�f�[�g
	void EffectUpdate();
	//�s����X�V�A����ɔ����R���C�_�[��
	void WorldMatColliderUpdate();
	//�m�[�h�̍s����A�j���[�V�����ɍ��킹�ĕύX
	void BlendAnimationUpdate();
	//fbx�̃m�[�h�̍s��X�V
	void UpdateFBXNodeMat();
	//�A�j���[�V�����̃A�b�v�f�[�g
	void AnimationUpdate();

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



	//�t�H�O�Ƃ��̃t���O
	void SetIsSilhouette(bool is) { effectFlags_.isSilhouette = is; }

	//-------------

	void DrawTriangle(Camera* camera = nullptr, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBox(Camera* camera = nullptr, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBoxSprite(Camera2D* camera, uint64_t textureHandle = NULL, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }
		, const Vec2& ancorUV = { 0,0 }, bool isReverseX = false, bool isReverseY = false,
		int32_t pipelineNum = 0);

	void DrawClippingBoxSprite(Camera2D* camera, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		uint64_t textureHandle = NULL, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, bool isPosLeftTop = true,
		bool isReverseX = false, bool isReverseY = false, int32_t pipelineNum = 0);

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