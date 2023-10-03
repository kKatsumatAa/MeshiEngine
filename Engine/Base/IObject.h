#pragma once
#include "Util.h"
#include"CollisionInfo.h"
#include "Camera.h"
#include "RootPipe.h"
#include "IModel.h"
#include "LightManager.h"


//コライダーの親クラス前方宣言
class BaseCollider;

//オブジェクト親クラス
class IObject
{
protected://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//要修正
	enum ROOTPARAM_NUM
	{
		COLOR,
		TEX,
		MATRIX,
		MESH_MAT,
		MATERIAL,
		LIGHT,
		EFFECT,
		SKIN,
		DISSOLVE,
		SPECULAR_MAP,
		NORM_MAP
	};

protected://継承先まで公開
	// グラフィックスパイプライン設定
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_;
	//定数バッファの生成
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
	//定数バッファ用データ構造体（マテリアル）
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;
	//ライト
	static LightManager* sLightManager_;

protected:
	//ワールド行列用クラス
	std::unique_ptr<WorldMat> worldMat_ = std::make_unique<WorldMat>();
	//親インスタンス
	IObject* parentObj_ = nullptr;

protected:
	//クラス名(デバッグ用)
	std::string objName_;
	//生きてるフラグ
	bool isAlive_ = true;
	//描画、更新するかどうか
	bool isValid_ = true;
	//速度
	Vec3 velocity_ = { 0,0,0 };
	//コライダー
	std::unique_ptr<BaseCollider> collider_ = nullptr;


//関数------------------------------------------------------------------------------
protected:
	//共通の初期化処理
	void InitializeCommon(std::unique_ptr<WorldMat> worldMat = nullptr);

public:
	virtual void Update();

	virtual void Draw();

public:
	//行列を更新、それに伴いコライダーも
	void WorldMatColliderUpdate();

	//デバッグ用の表示
	virtual void DrawImGui(std::function<void()>imguiF = NULL);

	//パイプラインの設定
	static void PipeLineState(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe);

public:
	//衝突時コールバック関数
	virtual void OnCollision(const CollisionInfo& info) {}

public:
	//生きてるか
	void SetIsAlive(bool isAlive);
	bool GetIsAlive() { return isAlive_; }

	//描画、更新するかどうか
	virtual void SetIsValid(bool isValid);
	bool GetIsValid() { return isValid_; }

	//スピード（当たり判定に使う）
	inline void SetVelocity(const Vec3& vec) { velocity_ = vec; }
	inline const Vec3& GetVelocity() { return velocity_; }


public:
	//位置
	void SetTrans(const Vec3& pos) { worldMat_->trans_ = pos; }
	void SetTransX(float pos) { worldMat_->trans_.x = pos; }
	void SetTransY(float pos) { worldMat_->trans_.y = pos; }
	void SetTransZ(float pos) { worldMat_->trans_.z = pos; }
	Vec3 GetTrans() { return worldMat_->trans_; }
	//スケール
	void SetScale(const Vec3& scale) { worldMat_->scale_ = scale; }
	void SetScaleX(float scale) { worldMat_->scale_.x = scale; }
	void SetScaleY(float scale) { worldMat_->scale_.y = scale; }
	void SetScaleZ(float scale) { worldMat_->scale_.z = scale; }
	const Vec3& GetScale() { return worldMat_->scale_; }
	//回転
	void SetRot(const Vec3& rot) {
		worldMat_->rot_ = rot;
	}
	void SetRotX(float rot) { worldMat_->rot_.x = rot; }
	void SetRotY(float rot) { worldMat_->rot_.y = rot; }
	void SetRotZ(float rot) { worldMat_->rot_.z = rot; }
	const Vec3& GetRot() { return worldMat_->rot_; }
	//行列を更新
	void CalcWorldMat() { worldMat_->CalcWorldMat(); }
	void CalcRotMat() { worldMat_->CalcRotMat(); }
	void CalcTransMat() { worldMat_->CalcTransMat(); }
	void CalcScaleMat() { worldMat_->CalcScaleMat(); }
	//ワールド行列の中身コピー
	void SetWorldMat(std::unique_ptr<WorldMat> worldMat) { worldMat_ = std::move(worldMat); }
	//親
	void SetParent(Object* obj) { worldMat_->parent_ = obj->GetWorldMat(); }
	void SetParent(WorldMat* worldMat) { worldMat_->parent_ = worldMat; }
	WorldMat* GetParent() { return worldMat_->parent_; }
	//ワールド行列用クラスのインスタンスを取得
	WorldMat* GetWorldMat() { return worldMat_.get(); }
	//親子関係を考慮した位置をゲット
	Vec3 GetWorldTrans() { return worldMat_->GetWorldTrans(); }
	//ワールド行列の取得
	const M4& GetMatWorld() { return worldMat_->matWorld_; }

	//クォータニオン系
	Quaternion GetQuaternion() { return worldMat_->GetQuaternion(); }
	void SetIsUseQuaternionMatRot(bool is) { worldMat_->SetIsUseQuaMatRot(is); }
	void SetMatRot(const M4& m) { worldMat_->SetRotMat(m); }

public:
	//オブジェクト名前
	void SetObjName(std::string objName) { objName_ = objName; }
	const std::string& GetObjName() { return objName_; }

	//コライダー
	BaseCollider* GetCollider() { return collider_.get(); }
	//コライダーのセット
	void SetCollider(std::unique_ptr<BaseCollider> collider);
	//判定を有効にするか
	void SetColliderIsValid(bool isValid);

public:
	//色を返す
	const Vec4& GetColor() { return constMapMaterial_->color; }

};

void SetNormDigitalMat(DirectX::XMMATRIX& mat);

void Error(bool filed, ID3DBlob* errorBlob);