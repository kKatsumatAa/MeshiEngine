#pragma once
#include<DirectXMath.h>
#include <wrl.h>


/// <summary>
/// 照明(様々なライトを作るときはポリモーフィズムで継承するといい)
/// </summary>
class DirLight
{
private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using Comptr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;


private://メンバ変数
	//ライト光線方向
	XMVECTOR lightdir_ = { 1,0,0,0 };
	//ライト色
	XMFLOAT3 lightColor_ = { 1,1,1 };
	// 有効フラグ
	bool active_ = false;


public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMVECTOR lightv;     //ライトの方向を表すベクトル
		XMFLOAT3 lightColor; //ライトの色
		uint32_t active; //有効か
	};


public:
	/// <summary>
	/// ライト方向をセット
	/// </summary>
	/// <param name="lightdir_">ライト方向</param>
	inline void SetLightDir(const XMVECTOR& lightdir) { lightdir_ = DirectX::XMVector3Normalize(lightdir); }

	/// <summary>
	/// ライト方向を取得
	/// </summary>
	/// <returns>ライト方向</returns>
	inline const XMVECTOR& GetLightDir() { return lightdir_; }

	/// <summary>
	/// ライト色をセット
	/// </summary>
	/// <param name="lightcolor">ライト色</param>
	inline void SetLightColor(const XMFLOAT3& lightcolor) { lightColor_ = lightcolor; }

	/// <summary>
	/// ライト色を取得
	/// </summary>
	/// <returns>ライト色</returns>
	inline const XMFLOAT3& GetLightColor() { return lightColor_; }

	/// <summary>
	/// 有効フラグをセット
	/// </summary>
	/// <param name="active">有効フラグ</param>
	inline void SetActive(bool active) { active_ = active; }

	/// <summary>
	/// 有効チェック
	/// </summary>
	/// <returns>有効フラグ</returns>
	inline bool GetActive() { return active_; }

	//
	void DrawImGui(int32_t index);
};

