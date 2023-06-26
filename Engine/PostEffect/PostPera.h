#pragma once
#include"TextureManager.h"
#include "DirectXWrapper.h"
#include "GlassFilter.h"
#include <functional>


//��ʌ��ʗp�̃t���O
struct EffectConstBuffer
{
	//�ۑ�p�}���`�e�N�X�`���g�ݍ��킹
	uint32_t isMultiTex = false;
	//�ڂ���
	uint32_t isGaussian = false;
	//�K�E�V�A���ڂ���
	uint32_t isGaussian2 = false;
	//�G���{�X
	uint32_t isEmboss = false;
	//�V���[�v�l�X
	uint32_t isSharpness = false;
	//�~��
	uint32_t isGradation = false;
	//�A�E�g���C��
	uint32_t isOutLine = false;
	//�r�l�b�g
	uint32_t isVignette = false;
	//�M��p��
	uint32_t isBarrelCurve = false;
	//������
	uint32_t isScanningLine = false;
	//�O���[�X�P�[��
	uint32_t isGrayScale = false;
	//�K���X�t�B���^�[
	uint32_t isGlassFilter = false;
	//���U�C�N
	uint32_t isMosaic = false;
	//�l�K�|�W
	uint32_t isNega = false;
	//�l�K�|�W
	uint32_t isRGBShift = false;
	//�u���[��
	uint32_t isBloom = false;
	//�N���X�t�B���^
	uint32_t isCrossFilter = false;
	//����
	uint32_t time = 0;
	//�u���[���̋���
	float bloomPow = 3.0f;
};

struct PeraVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

class PostPera
{
private:
	PeraVertex pVertices_[4] = {
		{{-1.0f, -1.0f,0},{0,1.0f}},//����
		{{-1.0f, +1.0f,0},{0,0}},//����
		{{+1.0f, -1.0f,0},{1.0f,1.0f}},//�E��
		{{+1.0f, +1.0f,0},{1.0f,0}}//�E��
	};

	ComPtr<ID3D12Resource> peraVB_;
	D3D12_VERTEX_BUFFER_VIEW peraVBV_;

	ComPtr<ID3DBlob> vs_;
	ComPtr<ID3DBlob> ps_;
	ComPtr<ID3DBlob> errBlob_;

	ComPtr<ID3D12RootSignature> peraRS_;
	ComPtr<ID3D12PipelineState> peraPipeline_ = NULL;
	ComPtr<ID3D12PipelineState> peraPipeline2_ = NULL;

	//��ʌ��ʗp
	ComPtr <ID3D12Resource> effectFlagsBuff_;
	//�}�b�s���O�p
	EffectConstBuffer* mapEffectFlagsBuff_;

	//�|�X�g�G�t�F�N�g�p
	D3D12_CPU_DESCRIPTOR_HANDLE peraSRHandle_;
	ComPtr<ID3D12Resource> peraResources_[2];
	ComPtr<ID3D12Resource> peraResource2_;
	ComPtr<ID3D12DescriptorHeap> peraRTVHeap_;//�����_�[�^�[�Q�b�g�p
	ComPtr<ID3D12DescriptorHeap> peraSRVHeap_;//�e�N�X�`���p
	bool isPeraClear_ = false;

	GausianBuffer gaussianBuff_;

	//�@���}�b�v�p
	ComPtr<ID3D12Resource>effectTexBuffer_;

	//�u���[���o�b�t�@�[
	ComPtr<ID3D12Resource>bloomBuffers_[3];
	//��ʑS�̂ڂ����p�p�C�v���C��(�u���[��)
	ComPtr<ID3D12PipelineState> blurPipeline_;

	//�u���[���̍ő勭��
	const float BLOOM_MAX_POW_ = 10.0f;

public:
	//��ʌ��ʗp
	EffectConstBuffer effectFlags_;



private://�֐�
	void InitializeBuffRTV(const wchar_t* normalImageFileName);
	void GenerateRSPL();
	void GlassFilterBuffGenerate(const wchar_t* fileName);
	//�K���X�p�̃o�b�t�@�ƃr���[�쐬
	bool CreateEffectBufferAndView(const wchar_t* fileName);
	//�u���[���p�̃o�b�t�@�쐬
	bool CreateBloomBuffer();
	//�u���[���p�̃o�b�t�@�쐬
	bool CreateBloomBuffView();

	//�ꖇ�ڂɕ`��
	void PreDraw();
	void PostDraw();

	//�񖇖ڂɕ`��
	void PreDraw2();
	void PostDraw2();
	//���ڂ̃G�t�F�N�g������
	void Draw();

	//���ڂ̃G�t�F�N�g������p�̕`��
	void Draw2All();

	//�S�Ẵr���[�����Z�b�g
	void SetHeapAllBuffView(bool isPost2 = false);

public:
	void Initialize(const wchar_t* normalImageFileName, float vertexExtend = 1.0f);

	void Update();

	//�k���o�b�t�@��������
	void DrawShrinkTextureForBlur();

	//�G�t�F�N�g�����I����Ď��ۂɕ`�悷��
	void Draw2();

	//�ꖇ�ڂƓ񖇖ڂ�`�悷��(�����̕`��֐����ꖇ�ڂɕ`��)
	void DrawToPostpera(const std::function<void()>& f);
};

