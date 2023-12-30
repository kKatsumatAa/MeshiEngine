/*
* @file IObject.h
* @brief オブジェクトの基底クラス
*/

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

public:
	//4つの頂点
	static const int8_t S_VERTEX_4_ = 4;
	//描画先のレンダーターゲット数
	static const int8_t S_RENDER_TARGET_NUM_ = 3;
	static const int8_t S_EFFECT_DESC_RANGE_NUM_ = 4;
	static const int8_t S_TEX_SAMPLER_NUM_ = 2;
	static const int8_t S_SAMPLER_DESC_MAX_ANISOROPY_ = 1;
	static const int8_t S_SAMPLER_DESC_SHADER_REGISTER_ = 1;
	static const int8_t S_SAMPLE_DESC_COUNT_ = 1;

	const Vec4 INIT_COLOR_ = { 1.0f,1.0f,1.0f,1.0f };

private:
	//演出用デスクリプタレンジ
	static D3D12_DESCRIPTOR_RANGE effectDescRange_[S_EFFECT_DESC_RANGE_NUM_];

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
	//デストラクタ
	virtual ~IObject();
	//コンストラクタ
	IObject();

protected:
	//共通の初期化処理
	void Initialize(std::unique_ptr<WorldMat> worldMat = nullptr);

public:
	//静的初期化
	static void CommonInitialize();

public:
	//更新
	virtual void Update();
	//描画
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
	void SetRootPipe(const RootPipe& pipelineSet);

	//パイプラインの設定
	static void PipeLineSetting(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe,
		const std::string& vSName, const std::string& pSName,
		const std::string& gSName = "", const std::string& hSName = "", const std::string& dSName = "",
		D3D12_INPUT_ELEMENT_DESC* inputLayout = nullptr, uint32_t inputLCount = 0,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE priTopoType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		int32_t numRTarget = 3,
		bool isSprite = false);

	//ブレンドの設定
	static void Blend(D3D12_GRAPHICS_PIPELINE_STATE_DESC& blendDesc,
		const D3D12_BLEND_OP& blendMode,
		bool Inversion = 0, bool Translucent = 0);

public:
	//衝突時コールバック関数、警告避けるために参照だけ
	virtual void OnCollision(const CollisionInfo& info) { info; }

public:
	//インスタンスの種類を得る
	ObjectInstanceType GetObjInsType() { return objInsType_; }

public:
	//生きてるかセット
	void SetIsAlive(bool isAlive);
	//生きてるか取得
	bool GetIsAlive() { return isAlive_; }

	//描画、更新するかどうかセット
	virtual void SetIsValid(bool isValid);
	//更新するかどうかセット
	void SetIsValidUpdate(bool isvalid) { isValidUpdate_ = isvalid; }
	//描画するかどうかセット
	void SetIsValidDraw(bool isvalid) { isValidDraw_ = isvalid; }

	//更新するかどうか取得
	bool GetIsValidUpdate() { return isValidUpdate_; }
	//描画するかどうか取得
	bool GetIsValidDraw() { return isValidDraw_; }

	//スピード（当たり判定に使う）セット
	void SetVelocity(const Vec3& vec) { velocity_ = vec; }
	//スピード（当たり判定に使う）取得
	const Vec3& GetVelocity() { return velocity_; }

	//インスタンスの番号セット
	void SetInstanceNum(int32_t instanceNum) { instanceNum_ = instanceNum; }
	//インスタンスの番号取得
	int32_t GetInstanceNum() { return instanceNum_; }

public:
	//位置セット
	void SetTrans(const Vec3& pos) { worldMat_->trans_ = pos; }
	//X位置セット
	void SetTransX(float pos) { worldMat_->trans_.x = pos; }
	//Y位置セット
	void SetTransY(float pos) { worldMat_->trans_.y = pos; }
	//Z位置セット
	void SetTransZ(float pos) { worldMat_->trans_.z = pos; }
	//位置取得
	Vec3 GetTrans()const { return worldMat_->trans_; }
	//ローカル座標セット
	void SetLocalTrans(const Vec3& pos) { worldMat_->localTrans_ = pos; }
	//ローカル座標取得
	Vec3 GetLocalTrans()const { return worldMat_->localTrans_; }
	//スケールセット
	void SetScale(const Vec3& scale) { worldMat_->scale_ = scale; }
	//Xスケールセット
	void SetScaleX(float scale) { worldMat_->scale_.x = scale; }
	//Yスケールセット
	void SetScaleY(float scale) { worldMat_->scale_.y = scale; }
	//Zスケールセット
	void SetScaleZ(float scale) { worldMat_->scale_.z = scale; }
	//スケール取得
	const Vec3& GetScale()const { return worldMat_->scale_; }
	//角度セット
	void SetRot(const Vec3& rot) {
		worldMat_->rot_ = rot;
	}
	//X角度セット
	void SetRotX(float rot) { worldMat_->rot_.x = rot; }
	//Y角度セット
	void SetRotY(float rot) { worldMat_->rot_.y = rot; }
	//Z角度セット
	void SetRotZ(float rot) { worldMat_->rot_.z = rot; }
	//角度取得
	const Vec3& GetRot()const { return worldMat_->rot_; }

	//親と自分の行列を更新
	void SetLocalParentMat(const M4& mat) { worldMat_->SetLocalParentMat(mat); }
	//ワールド行列更新
	void CalcWorldMat() { worldMat_->CalcWorldMat(); }
	//回転行列更新
	void CalcRotMat() { worldMat_->CalcRotMat(); }
	//トランス行列更新
	void CalcTransMat() { worldMat_->CalcTransMat(); }
	//スケール行列更新
	void CalcScaleMat() { worldMat_->CalcScaleMat(); }
	//親のworldMatセット
	void SetParent(IObject* obj) { worldMat_->parent_ = obj->GetWorldMat(); }
	//親のworldMatセット（worldMatで）
	void SetParent(WorldMat* worldMat) { worldMat_->parent_ = worldMat; }
	//親のworldMat取得
	WorldMat* GetParent() { return worldMat_->parent_; }
	//ワールド行列用クラスのインスタンスを取得
	WorldMat* GetWorldMat() { return worldMat_.get(); }
	void SetWorldMatParam(const WorldMat& worldMat) { *(worldMat_.get()) = worldMat; }
	//親子関係を考慮した位置をゲット
	Vec3 GetWorldTrans()const { return worldMat_->GetWorldTrans(); }
	//親子関係を考慮したスケールをゲット
	Vec3 GetWorldScale()const { return worldMat_->GetWorldScale(); }
	//ワールド行列の取得
	const M4& GetMatWorld() { return worldMat_->matWorld_; }

	//クォータニオン取得
	Quaternion GetQuaternion() { return worldMat_->GetQuaternion(); }
	//クォータニオンの行列を使うかセット
	void SetIsUseQuaternionMatRot(bool is) { worldMat_->SetIsUseQuaMatRot(is); }
	//クォータニオンの行列を使うか取得
	void SetMatRot(const M4& m) { worldMat_->SetRotMat(m); }

public:
	//オブジェクト名前セット
	void SetObjName(std::string objName) { objName_ = objName; }
	//オブジェクト名前取得
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
//エラー処理
void Error(bool filed, ID3DBlob* errorBlob);