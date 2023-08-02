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
	//���W�A���u���[
	uint32_t isRadialBlur = false;
	//���W�A������
	float radialPow = 1.0f;
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
	float barrelCurvePow = 1.0f;
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
	//�u���[���̋���
	float bloomPow = 9.0f;
	//�m�C�Y
	uint32_t isNoise = false;
	//��ʊE�[�x
	uint32_t isDepthField = false;
	//����
	uint32_t time = 0;
};

struct PeraVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

class PostPera
{
private:
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//�y���̊g�嗦
	float pera1Extend_ = 1.0f;
	float peraGExtend_ = 1.0f;
	float pera2Extend_ = 1.0f;
	//�ꖇ�ڂ̒��_
	PeraVertex p1Vertices_[4] = {
			{{-1.0f, -1.0f,0},{0,1.0f}},//����
			{{-1.0f, +1.0f,0},{0,0}},//����
			{{+1.0f, -1.0f,0},{1.0f,1.0f}},//�E��
			{{+1.0f, +1.0f,0},{1.0f,0}}//�E��
	};
	//�K�E�V�A���p�̒��_
	PeraVertex pGVertices_[4] = {
			{{-1.0f, -1.0f,0},{0,1.0f}},//����
			{{-1.0f, +1.0f,0},{0,0}},//����
			{{+1.0f, -1.0f,0},{1.0f,1.0f}},//�E��
			{{+1.0f, +1.0f,0},{1.0f,0}}//�E��
	};
	//�񖇖ڂ̒��_
	PeraVertex p2Vertices_[4] = {
			{{-1.0f, -1.0f,0},{0,1.0f}},//����
			{{-1.0f, +1.0f,0},{0,0}},//����
			{{+1.0f, -1.0f,0},{1.0f,1.0f}},//�E��
			{{+1.0f, +1.0f,0},{1.0f,0}}//�E��
	};

	ComPtr<ID3D12Resource> pera1VB_;
	D3D12_VERTEX_BUFFER_VIEW pera1VBV_;
	ComPtr<ID3D12Resource> peraGVB_;
	D3D12_VERTEX_BUFFER_VIEW peraGVBV_;
	ComPtr<ID3D12Resource> pera2VB_;
	D3D12_VERTEX_BUFFER_VIEW pera2VBV_;



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
	const float BLOOM_MAX_POW_ = 15.0f;
	//���W�A���̍ő勭��
	const float RADIAL_MAX_POW_ = 1.0f;

	//
	float peraExtend_ = 1.0f;

public:
	//��ʌ��ʗp
	EffectConstBuffer effectFlags_;



private://�֐�
	//���_�o�b�t�@�쐬
	void CreateVertexBuff(PeraVertex* vertex, int32_t vertexCount, ID3D12Resource** vB, D3D12_VERTEX_BUFFER_VIEW* vBV);
	//���_�o�b�t�@�}�b�s���O
	void MappingVertexBuff(PeraVertex* vertex, int32_t vertexCount, float peraExtend, ID3D12Resource* vB);

	void InitializeBuffRTV(const wchar_t* normalImageFileName);
	void GenerateRSPL();
	void GlassFilterBuffGenerate(const wchar_t* fileName);
	//�K���X�p�̃o�b�t�@�ƃr���[�쐬
	bool CreateEffectBufferAndView(const wchar_t* fileName);
	//�u���[���p�̃o�b�t�@�쐬
	bool CreateBloomBuffer();
	//�u���[���p�̃o�b�t�@�쐬
	bool CreateBloomBuffView();
	//�[�x�l�e�N�X�`���pSRV
	bool CreateDepthBuffAndSRV();

	//�ꖇ�ڂɕ`��
	void PreDraw(bool isFirstPostPera);
	void PostDraw();

	//�񖇖ڂɕ`��
	void PreDraw2();
	void PostDraw2();
	//���ڂ̃G�t�F�N�g������
	void Draw();

	//�q�[�v�ƃq�[�v��ɍ�����o�b�t�@�r���[�����Z�b�g
	void SetHeapAllBuffView(bool isPost2 = false, bool isPost1 = false);

public:
	void Initialize(const wchar_t* normalImageFileName);

	void Update();
	void ImGuiUpdate();

	//���ڂ̃G�t�F�N�g������p�̕`��
	void DrawToBlurAll();

	//�k���o�b�t�@��������
	void DrawShrinkTextureForBlur();

	//�G�t�F�N�g�����I����Ď��ۂɕ`�悷��
	void Draw2();

	//�ꖇ�ڂƓ񖇖ڂ�`�悷��(�����̕`��֐����ꖇ�ڂɕ`��)
	void DrawToPostpera(const std::function<void()>& f, bool isFirstPostPera = false);

public:
	void SetPera1Extend(float extend) { pera1Extend_ = extend; }
	void SetPeraGExtend(float extend) { peraGExtend_ = extend; }
	void SetPera2Extend(float extend) { pera2Extend_ = extend; }
};

