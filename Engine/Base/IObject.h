#pragma once
#include "Util.h"
#include"CollisionInfo.h"
#include "Camera.h"
#include "RootPipe.h"
#include "IModel.h"
#include <string>
#include <vector>
#include "ImguiManager.h"


//コライダーの親クラス前方宣言
class BaseCollider;

struct ConstBufferDataMaterial
{
	Vec4 color = { 1.0f,1.0f,1.0f,1.0f };//色(RGBA)
};

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
	//ルートパラメータの引数
	enum RootParamNum
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
		NORM_MAP,
		SHADOW_TEX,
		TESS_WAVE,
		//要素数
		count
	};

	//インスタンスのタイプ
	enum ObjectInstanceType
	{
		UNKNOWN = -1,
		SPRITE,
		OBJ,
		FBX,
		THREE_D
	};

private:
	//演出用デスクリプタレンジ
	static D3D12_DESCRIPTOR_RANGE effectDescRange_[4];

protected://継承先まで公開
	//定数バッファの生成
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
	//定数バッファ用データ構造体（マテリアル）
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;

protected:
	//ルートパラメータの設定
	static D3D12_ROOT_PARAMETER rootParams_[RootParamNum::count];

protected:
	//ワールド行列用のバッファなど
	ConstBuffTransform cbt_;//ここをどうにかすれば、インスタンス一つでも色々描画
	//ワールド行列用クラス
	std::unique_ptr<WorldMat> worldMat_;
	//親インスタンス
	IObject* parentObj_ = nullptr;

protected:
	//クラス名(デバッグ用)
	std::string objName_;
	//生きてるフラグ
	bool isAlive_ = true;
	//描画、更新するかどうか
	bool isValidUpdate_ = true;
	bool isValidDraw_ = true;
	//速度
	Vec3 velocity_ = { 0,0,0 };
	//コライダー
	std::unique_ptr<BaseCollider> collider_ = nullptr;
	//インスタンスの種類（子クラスが何か）
	ObjectInstanceType objInsType_ = ObjectInstanceType::UNKNOWN;
	//インスタンスの番号
	int32_t instanceNum_ = 0;


	//関数------------------------------------------------------------------------------
public:
	virtual ~IObject();
	IObject();

protected:
	//共通の初期化処理
	void Initialize(std::unique_ptr<WorldMat> worldMat = nullptr);

public:
	//静的初期化
	static void CommonInitialize();

public:
	virtual void Update();

	virtual void Draw() = 0;

public:
	//行列を更新、それに伴いコライダーも
	void WorldMatColliderUpdate();
	//コライダーのみ更新
	void ColliderUpdate();

	//デバッグ用の表示
	virtual void DrawImGui(std::function<void()>imguiF = NULL);

public:
	//ルートシグネチャ系のコマンド(パイプラインステートとルートシグネチャ別のも組み合わせられる)
	void SetRootPipe(RootPipe* pipelineSet, int32_t pipelineNum, ID3D12RootSignature* rootSignature);

	//パイプラインの設定
	static void PipeLineSetting(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe,
		const std::string& vSName, const std::string& pSName,
		const std::string& gSName = "", const std::string& hSName = "", const std::string& dSName = "",
		D3D12_INPUT_ELEMENT_DESC* inputLayout = nullptr, uint32_t inputLCount = 0,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE priTopoType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		int32_t numRTarget = 3,
		bool isSprite = false);

	static void Blend(D3D12_GRAPHICS_PIPELINE_STATE_DESC& blendDesc,
		const D3D12_BLEND_OP& blendMode,
		bool Inversion = 0, bool Translucent = 0);

public:
	//衝突時コールバック関数、警告避けるために参照だけ
	virtual void OnCollision(const CollisionInfo& info) { info; }

public:
	//インスタンスの種類を得る
	ObjectInstanceType GetObjInsType() { return objInsType_; }
	//2Dか3Dか

public:
	//生きてるか
	void SetIsAlive(bool isAlive);
	bool GetIsAlive() { return isAlive_; }

	//描画、更新するかどうか
	virtual void SetIsValid(bool isValid);
	void SetIsValidUpdate(bool isvalid) { isValidUpdate_ = isvalid; }
	void SetIsValidDraw(bool isvalid) { isValidDraw_ = isvalid; }

	bool GetIsValidUpdate() { return isValidUpdate_; }
	bool GetIsValidDraw() { return isValidDraw_; }

	//スピード（当たり判定に使う）
	void SetVelocity(const Vec3& vec) { velocity_ = vec; }
	const Vec3& GetVelocity() { return velocity_; }

	//インスタンスの番号
	void SetInstanceNum(int32_t instanceNum) { instanceNum_ = instanceNum; }
	int32_t GetInstanceNum() { return instanceNum_; }


public:
	//位置
	void SetTrans(const Vec3& pos) { worldMat_->trans_ = pos; }
	void SetTransX(float pos) { worldMat_->trans_.x = pos; }
	void SetTransY(float pos) { worldMat_->trans_.y = pos; }
	void SetTransZ(float pos) { worldMat_->trans_.z = pos; }
	Vec3 GetTrans()const { return worldMat_->trans_; }
	void SetLocalTrans(const Vec3& pos) { worldMat_->localTrans_ = pos; }
	Vec3 GetLocalTrans()const { return worldMat_->localTrans_; }
	//スケール
	void SetScale(const Vec3& scale) { worldMat_->scale_ = scale; }
	void SetScaleX(float scale) { worldMat_->scale_.x = scale; }
	void SetScaleY(float scale) { worldMat_->scale_.y = scale; }
	void SetScaleZ(float scale) { worldMat_->scale_.z = scale; }
	const Vec3& GetScale()const { return worldMat_->scale_; }
	//回転
	void SetRot(const Vec3& rot) {
		worldMat_->rot_ = rot;
	}
	void SetRotX(float rot) { worldMat_->rot_.x = rot; }
	void SetRotY(float rot) { worldMat_->rot_.y = rot; }
	void SetRotZ(float rot) { worldMat_->rot_.z = rot; }
	const Vec3& GetRot()const { return worldMat_->rot_; }
	//行列を更新
	void SetLocalParentMat(const M4& mat) { worldMat_->SetLocalParentMat(mat); }
	void CalcWorldMat() { worldMat_->CalcWorldMat(); }
	void CalcRotMat() { worldMat_->CalcRotMat(); }
	void CalcTransMat() { worldMat_->CalcTransMat(); }
	void CalcScaleMat() { worldMat_->CalcScaleMat(); }
	//親
	void SetParent(IObject* obj) { worldMat_->parent_ = obj->GetWorldMat(); }
	void SetParent(WorldMat* worldMat) { worldMat_->parent_ = worldMat; }
	WorldMat* GetParent() { return worldMat_->parent_; }
	//ワールド行列用クラスのインスタンスを取得
	WorldMat* GetWorldMat() { return worldMat_.get(); }
	//親子関係を考慮した位置をゲット
	Vec3 GetWorldTrans()const { return worldMat_->GetWorldTrans(); }
	//親子関係を考慮したスケールをゲット
	Vec3 GetWorldScale()const { return worldMat_->GetWorldScale(); }
	//ワールド行列の取得
	const M4& GetMatWorld() { return worldMat_->matWorld_; }

	//クォータニオン系
	Quaternion GetQuaternion() { return worldMat_->GetQuaternion(); }
	void SetIsUseQuaternionMatRot(bool is) { worldMat_->SetIsUseQuaMatRot(is); }
	void SetMatRot(const M4& m) { worldMat_->SetRotMat(m); }

public:
	//オブジェクト名前
	void SetObjName(std::string objName) { objName_ = objName; }
	const std::string& GetObjName()const { return objName_; }

	//コライダー
	BaseCollider* GetCollider() { return collider_.get(); }
	//コライダーのセット
	void SetCollider(std::unique_ptr<BaseCollider> collider);
	//判定を有効にするか
	void SetColliderIsValid(bool isValid);

public:
	//色をセット
	void SetColor(const Vec4& color) { constMapMaterial_->color = color; }
	//色を返す
	const Vec4& GetColor() { return constMapMaterial_->color; }

};

void Error(bool filed, ID3DBlob* errorBlob);