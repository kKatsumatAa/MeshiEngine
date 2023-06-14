#pragma once
#include"TextureManager.h"
#include "DirectXWrapper.h"
#include "GlassFilter.h"
#include <functional>


//��ʌ��ʗp�̃t���O
struct EffectConstBuffer
{
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
	//����
	uint32_t time = 0;
};

struct PeraVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

class PostPera
{
private:
	PeraVertex pv[4] = {
		{{-1.0f, -1.0f,0},{0,1.0f}},//����
		{{-1.0f, +1.0f,0},{0,0}},//����
		{{+1.0f, -1.0f,0},{1.0f,1.0f}},//�E��
		{{+1.0f, +1.0f,0},{1.0f,0}}//�E��
	};

	ComPtr<ID3D12Resource> _peraVB;
	D3D12_VERTEX_BUFFER_VIEW _peraVBV;

	ComPtr<ID3DBlob> vs;
	ComPtr<ID3DBlob> ps;
	ComPtr<ID3DBlob> errBlob;

	ComPtr<ID3D12RootSignature> _peraRS;
	ComPtr<ID3D12PipelineState> _peraPipeline = NULL;
	ComPtr<ID3D12PipelineState> _peraPipeline2 = NULL;

	//��ʌ��ʗp
	ComPtr <ID3D12Resource> effectFlagsBuff;
	//�}�b�s���O�p
	EffectConstBuffer* mapEffectFlagsBuff;

	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[1] = {};

	//�|�X�g�G�t�F�N�g�p
	D3D12_CPU_DESCRIPTOR_HANDLE peraHandle;
	std::array<ComPtr<ID3D12Resource>, 2> _peraResource;
	ComPtr<ID3D12Resource> _peraResource2;
	ComPtr<ID3D12DescriptorHeap> _peraRTVHeap;//�����_�[�^�[�Q�b�g�p
	ComPtr<ID3D12DescriptorHeap> _peraSRVHeap;//�e�N�X�`���p
	bool isPeraClear = false;

	GausianBuffer gausianBuff;

	//�K���X
	ComPtr<ID3D12Resource>_effectTexBuffer;

	//�u���[���o�b�t�@�[
	std::array<ComPtr<ID3D12Resource>, 2>_bloomBuffer;
	//��ʑS�̂ڂ����p�p�C�v���C��(�u���[��)
	ComPtr<ID3D12PipelineState> _blurPipeline;

public:
	//��ʌ��ʗp
	EffectConstBuffer effectFlags;



private://�֐�
	void InitializeBuffRTV(const wchar_t* nomalImageFileName);
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

public:
	void Initialize(const wchar_t* nomalImageFileName);

	void Update();

	//�k���o�b�t�@��������
	void DrawShrinkTextureForBlur();

	//�G�t�F�N�g�����I����Ď��ۂɕ`�悷��
	void Draw2();

	//�ꖇ�ڂƓ񖇖ڂ�`�悷��(�����̕`��֐����ꖇ�ڂɕ`��)
	void DrawToPostpera(const std::function<void()>& f);
};

