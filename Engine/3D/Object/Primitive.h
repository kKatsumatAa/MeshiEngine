#pragma once
#include"Model.h"


/// <summary>
/// �}�`�p�̏����������N���X�idraw�Ŏg���̂ň�̂ݐ����j
/// </summary>
class Primitive
{
private:

public:
	//���̃C���f�b�N�X�p
	static const int32_t S_SPHERE_INDEX_NUM_ = 66 * 3 + 6;

	//���_�o�b�t�@�̐���
	ComPtr < ID3D12Resource> vertBuffTriangle_ = nullptr;
	ComPtr < ID3D12Resource> vertBuffBox_ = nullptr;
	ComPtr < ID3D12Resource> vertBuffCircle_ = nullptr;
	ComPtr < ID3D12Resource> vertBuffCube_ = nullptr;
	ComPtr < ID3D12Resource> vertBuffLine_ = nullptr;
	ComPtr<ID3D12Resource> vertBuffSphere_ = nullptr;

	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbTriangleView_{};
	D3D12_VERTEX_BUFFER_VIEW vbBoxView_{};
	D3D12_VERTEX_BUFFER_VIEW vbCircleView_{};
	D3D12_VERTEX_BUFFER_VIEW vbCubeView_{};
	D3D12_VERTEX_BUFFER_VIEW vbLineView_{};
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbViewSphere_{};

	//04_01
		//���_�f�[�^�\����

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp_{};

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	D3D12_INDEX_BUFFER_VIEW ibViewTriangle_{};
	D3D12_INDEX_BUFFER_VIEW ibViewBox_{};
	D3D12_INDEX_BUFFER_VIEW ibViewCircle_{};
	D3D12_INDEX_BUFFER_VIEW ibViewLine_{};
	D3D12_INDEX_BUFFER_VIEW ibViewCube_{};
	D3D12_INDEX_BUFFER_VIEW ibViewSphere_{};

	//�����Ȑ}�`�p
	Vertex verticesTriangle_[3] = {
		//��O
		{{-1.0f,-1.0f,0.0f},{},{0.0f,1.0f}},//����
		{{-1.0f,1.0f, 0.0f},{},{0.0f,0.0f}},//����
		{{1.0f,-1.0f, 0.0f},{},{1.0f,1.0f}},//�E��
	};
	Vertex verticesBox_[4] = {
		//��O
		{{-1.0f,-1.0f,0.0f},{},{0.0f,1.0f}},//����
		{{-1.0f,1.0f, 0.0f},{},{0.0f,0.0f}},//����
		{{1.0f,-1.0f, 0.0f},{},{1.0f,1.0f}},//�E��
		{{1.0f,1.0f,  0.0f},{},{1.0f,0.0f}},//�E��
	};
	Vertex verticesCircle_[24] = {};
	Vertex verticesCube_[24] = {};
	Vertex verticesLine_[2] = {};
	//����
	Vertex verticesSphere_[2 + 34 * 36];

public:
	//�������n
	void Initialize();

	void InitializeTriangle();
	void InitializeBox();
	void InitializeCircle();
	void InitializeCube();
	void InitializeLine();
	void InitializeSphere();

	//Draw�ɕK�v�ȃR�}���h
	void DrawCommandPrimitive(ID3D12Resource* pVertBuff, int32_t vertexCount, Vertex* pVertex, const D3D12_PRIMITIVE_TOPOLOGY& primitiveTopology,
		const D3D12_VERTEX_BUFFER_VIEW& buffView, const D3D12_INDEX_BUFFER_VIEW& ibView, int32_t indicesCount,
		const std::function<void()>& setRootParam,const std::function<void()>& setMaterialLightTex);

	void TriangleDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void BoxDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void CircleDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void CubeDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void LineDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
	void SphereDraw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);
};
