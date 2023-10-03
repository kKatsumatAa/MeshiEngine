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

};