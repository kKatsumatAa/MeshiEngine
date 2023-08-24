#pragma once
#include<DirectXMath.h>
#include <wrl.h>


/// <summary>
/// �Ɩ�(�l�X�ȃ��C�g�����Ƃ��̓|�����[�t�B�Y���Ōp������Ƃ���)
/// </summary>
class DirLight
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using Comptr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;


private://�����o�ϐ�
	//���C�g��������
	XMVECTOR lightdir_ = { 1,0,0,0 };
	//���C�g�F
	XMFLOAT3 lightColor_ = { 1,1,1 };
	// �L���t���O
	bool active_ = false;


public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		XMVECTOR lightv;     //���C�g�̕�����\���x�N�g��
		XMFLOAT3 lightColor; //���C�g�̐F
		uint32_t active; //�L����
	};


public:
	/// <summary>
	/// ���C�g�������Z�b�g
	/// </summary>
	/// <param name="lightdir_">���C�g����</param>
	inline void SetLightDir(const XMVECTOR& lightdir) { lightdir_ = DirectX::XMVector3Normalize(lightdir); }

	/// <summary>
	/// ���C�g�������擾
	/// </summary>
	/// <returns>���C�g����</returns>
	inline const XMVECTOR& GetLightDir() { return lightdir_; }

	/// <summary>
	/// ���C�g�F���Z�b�g
	/// </summary>
	/// <param name="lightcolor">���C�g�F</param>
	inline void SetLightColor(const XMFLOAT3& lightcolor) { lightColor_ = lightcolor; }

	/// <summary>
	/// ���C�g�F���擾
	/// </summary>
	/// <returns>���C�g�F</returns>
	inline const XMFLOAT3& GetLightColor() { return lightColor_; }

	/// <summary>
	/// �L���t���O���Z�b�g
	/// </summary>
	/// <param name="active">�L���t���O</param>
	inline void SetActive(bool active) { active_ = active; }

	/// <summary>
	/// �L���`�F�b�N
	/// </summary>
	/// <returns>�L���t���O</returns>
	inline bool GetActive() { return active_; }

	//
	void DrawImGui(int32_t index);
};

