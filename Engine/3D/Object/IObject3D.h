#pragma once
#include"IObject.h"
#include "LightManager.h"
#include "Camera.h"


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

//3D�I�u�W�F�N�g�̐e�N���X--------------------------------------------
class IObject3D : public IObject
{
private:
	ConstBuffTransform cbt_;//�������ǂ��ɂ�����΁A�C���X�^���X��ł��F�X�`��

private:
	//���o�p�o�b�t�@
	ComPtr <ID3D12Resource> effectFlagsBuff_;
	EffectOConstBuffer* mapEffectFlagsBuff_;

private:
	//���f���̃|�C���^
	IModel* model_ = nullptr;

	//�f�B�]���u�̉摜�n���h��
	uint64_t dissolveTextureHandle_ = NULL;
	//�X�y�L�����}�b�v�̉摜
	uint64_t specularMapTextureHandle_ = NULL;
	//�m�[�}���}�b�v�̉摜
	uint64_t normalMapTextureHandle_ = NULL;

private:
	//���b�V���̃I�t�Z�b�g
	Mesh::PolygonOffset meshOffsetData_;

private:
	//�����̐��ʃx�N�g��
	Vec3 frontVecTmp_ = { 0,0,-1.0f };
	//���ʃx�N�g��
	Vec3 frontVec_ = frontVecTmp_;


public://�֐�---------------------------------------------------------
	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat = nullptr, IModel* model = nullptr);

public:
	//���o�n�̃A�b�v�f�[�g
	void EffectUpdate();

public:
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

public:
	//���f���̃|�C���^
	void SetModel(IModel* model);
	IModel* GetModel() { return model_; }

	//���ʃx�N�g��
	//�I�u�W�F�N�g�̊p�x�ŉ�]���������ʃx�N�g�����Q�b�g
	const Vec3& GetFrontVec();
	void CalcFrontVec();
	const Vec3& GetFrontVecTmp() { return frontVecTmp_; }
	inline void SetFrontVecTmp(const Vec3& vec) { frontVecTmp_ = vec; }
	inline void SetFrontVec(const Vec3& vec) { frontVec_ = vec; }

public:
	//���b�V���̃I�t�Z�b�g�p
	void SetMeshPolygonOffsetData(const Mesh::PolygonOffset& offsetData) { meshOffsetData_ = offsetData; }

public:
	/// <summary>
/// ���C�g�̃Z�b�g
/// </summary>
/// <param name="light"></param>
	static void SetLight(LightManager* lightManager) { Object::sLightManager_ = lightManager; }

};