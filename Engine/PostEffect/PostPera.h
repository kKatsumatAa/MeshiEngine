#pragma once
#include"TextureManager.h"
#include "DirectXWrapper.h"
#include "GlassFilter.h"
#include <functional>


//��ʌ��ʗp�̃t���O
struct EffectConstBuffer
{
	//�ڂ���
	unsigned int isGaussian = false;
	//�K�E�V�A���ڂ���
	unsigned int isGaussian2 = false;
	//�G���{�X
	unsigned int isEmboss = false;
	//�V���[�v�l�X
	unsigned int isSharpness = false;
	//�~��
	unsigned int isGradation = false;
	//�A�E�g���C��
	unsigned int isOutLine = false;
	//�r�l�b�g
	unsigned int isVignette = false;
	//�M��p��
	unsigned int isBarrelCurve = false;
	//������
	unsigned int isScanningLine = false;
	//�O���[�X�P�[��
	unsigned int isGrayScale = false;
	//�K���X�t�B���^�[
	unsigned int isGlassFilter = false;
	//���U�C�N
	unsigned int isMosaic = false;
	//�l�K�|�W
	unsigned int isNega = false;
	//�l�K�|�W
	unsigned int isRGBShift = false;
	//����
	unsigned int time = 0;
};

struct PeraVertex
{
	XMFLOAT3 pos;
	float pad1 = 1.0f;
	XMFLOAT2 uv;
};

class PostPera
{
private:
	PeraVertex pv[4] = {
		{{-1, -1,0.1f},1, {0,1}},//����
		{{-1, 1,0.1f},1,{0,0}},//����
		{{1, -1,0.1f},1,{1,1}},//�E��
		{{1, 1,0.1f},1,{1,0}}//�E��
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

	D3D12_RESOURCE_BARRIER barrierDesc{};

public:
	//��ʌ��ʗp
	EffectConstBuffer effectFlags;



private://�֐�
	void InitializeBuffRTV();
	void GenerateRSPL();
	void GlassFilterBuffGenerate(const wchar_t* fileName);
	bool CreateEffectBufferAndView(const wchar_t* fileName);

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

	//�G�t�F�N�g�����I����Ď��ۂɕ`�悷��
	void Draw2();

	//�ꖇ�ڂƓ񖇖ڂ�`�悷��(�����̕`��֐����ꖇ�ڂɕ`��)
	void DrawToPostpera(std::function<void()> f);
};

