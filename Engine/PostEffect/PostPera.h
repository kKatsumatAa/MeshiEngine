#pragma once
#include"TextureManager.h"
#include "DirectXWrapper.h"
#include <functional>
#include "Vec4.h"
#include "Vec2.h"




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

public:
	static const float S_BLOOM_POW_;

	//�u���[���̍ő勭��
	static const float S_BLOOM_MAX_POW_;
	//���W�A���̍ő勭��
	static const float S_RADIAL_MAX_POW_;


private:
	//��ʌ��ʗp�̃t���O
	struct EffectConstBuffer
	{
		//�F
		Vec4 color = { 1.0f,1.0f,1.0f,1.0f };
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
		float grayScalePow = 1.0f;
		//�K���X�t�B���^�[
		uint32_t isGlassFilter = false;
		//���U�C�N
		uint32_t isMosaic = false;
		//�l�K�|�W
		uint32_t isNega = false;
		//RGB���炵
		uint32_t isRGBShift = false;
		float RGBShiftPow = 0.05f;
		//�u���[��
		uint32_t isBloom = false;
		//�N���X�t�B���^
		uint32_t isCrossFilter = false;
		//�u���[���̋���
		float bloomPow = PostPera::S_BLOOM_POW_;
		//�m�C�Y
		uint32_t isNoise = false;
		//��ʊE�[�x
		uint32_t isDepthField = false;
		//�t�H�[�J�X���킹��[�x�l
		float focusDepth = 0.05f;
		//�t�H�[�J�X�̃X���[�X�X�e�b�v�̕��̉���
		float nFocusWidth = 0.005f;
		//�t�H�[�J�X�̃X���[�X�X�e�b�v�̕��̏��
		float focusDiffPow = 0.01f;
		//����
		uint32_t time = 0;
	};

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

	//��ʊE�[�x�o�b�t�@
	ComPtr<ID3D12Resource>dofBuffer_;

	//�e�N�X�`���̑傫��
	float peraExtend_ = 1.0f;

public:
	//��ʌ��ʗp
	EffectConstBuffer effectFlags_;


private://�֐�
	//���_�o�b�t�@�쐬
	void CreateVertexBuff(PeraVertex* vertex, int32_t vertexCount, ID3D12Resource** vB, D3D12_VERTEX_BUFFER_VIEW* vBV);
	//���_�o�b�t�@�}�b�s���O
	void MappingVertexBuff(PeraVertex* vertex, int32_t vertexCount, float peraExtend, ID3D12Resource* vB);

	void InitializeBuffRTV(const char* normalImageFileName);
	void GenerateRSPL();
	void GlassFilterBuffGenerate(const char* fileName);
	//�K���X�p�̃o�b�t�@�ƃr���[�쐬
	bool CreateEffectBufferAndView(const char* fileName);
	//�u���[���p�̃o�b�t�@�쐬
	bool CreateBloomBuffer(const D3D12_RESOURCE_DESC& resDesc,
		const D3D12_HEAP_PROPERTIES& heapProp, D3D12_CLEAR_VALUE clearValue);
	bool CreateBloomBuffView();
	//��ʊE�[�x�p�o�b�t�@�쐬
	bool CreateDofBuffer(const D3D12_RESOURCE_DESC& resDesc,
		const D3D12_HEAP_PROPERTIES& heapProp, D3D12_CLEAR_VALUE clearValue);
	//��ʊE�[�x�pSRV
	bool CreateDofSRV();

	//�[�x�l�e�N�X�`���pSRV
	bool CreateDepthTexBuffAndSRV();

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
	void Initialize(const char* normalImageFileName);

	void Update();
	void ImGuiUpdate(int32_t num);

	//���ڂ̃G�t�F�N�g������p�̕`��
	void DrawToBlurAll();

	//�k���o�b�t�@��������
	void DrawShrinkTextureForBlur();

	//�G�t�F�N�g�����I����Ď��ۂɕ`�悷��
	void Draw2();

	//�ꖇ�ڂƓ񖇖ڂ�`�悷��(�����̕`��֐����ꖇ�ڂɕ`��)
	void DrawToPostpera(const std::function<void()>& f, bool isFirstPostPera = false, const std::function<void()>& drawSpriteF = nullptr);

public:
	void SetPera1Extend(float extend) { pera1Extend_ = extend; }
	void SetPeraGExtend(float extend) { peraGExtend_ = extend; }
	void SetPera2Extend(float extend) { pera2Extend_ = extend; }
};