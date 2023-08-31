#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <fbxsdk.h>
#include "Material.h"
#include "Sprite.h"
#include "Mesh.h"
#include <unordered_map>


class IModel
{
protected:// �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImg = DirectX::ScratchImage;
	using string = std::string;
	template <class T> using vector = std::vector<T>;

public://�萔
	//�Q�Ɛ�̃p�X
	static const std::string S_BASE_DIRECTORY_;


protected:
	// ���O
	std::string name_;
	// ���b�V���R���e�i
	std::vector<std::unique_ptr<Mesh>> meshes_;
	// �}�e���A���R���e�i
	std::unordered_map<std::string, std::unique_ptr<Material>> materials_;
	// �f�t�H���g�}�e���A��
	std::unique_ptr<Material> defaultMaterial_;

	//fbx��
	bool isFbx_ = false;

	//�X�P�[���̔{��(����Ȃǂɂ͉e�����Ȃ�)
	float scaleExtend_ = 1.0f;

	Vec3 materialExtend_ = { 1.0f,1.0f,1.0f };


public:
	IModel() { ; }
	virtual ~IModel();

protected:
	virtual void LoadTexturesInternal(const std::string& directoryPath);

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

public:
	/// <summary>
	/// �}�e���A���o�^
	/// </summary>
	void AddMaterial(std::unique_ptr<Material> material);

	virtual void LoadTextures() = 0;

public: // �����o�֐�
	static void StaticInitialize();

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList">���ߔ��s��R�}���h���X�g</param>
	virtual void Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex
		, const ConstBuffTransform& cbt);

	virtual void DrawImGui();

public:
	/// <summary>
	/// ���b�V���R���e�i���擾
	/// </summary>
	/// <returns></returns>
	inline const std::vector<std::unique_ptr<Mesh>>& GetMeshes() { return meshes_; }

	//fbx��
	bool GetIsFbx() { return isFbx_; }

	//�X�P�[���̔{��
	float GetScaleExtend()const { return scaleExtend_; }
	Vec3 GetMaterialExtend() { return materialExtend_; }
	void SetMaterialExtend(const Vec3& materialExtend) { materialExtend_ = materialExtend; }
	//�|���S���̍��W�ɉ��Z������W���v�Z���邽�߂̏����Z�b�g
	void SetPolygonOffsetData(const Mesh::PolygonOffset& polygonOffsetData);
};