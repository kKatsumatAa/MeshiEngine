#pragma once
#include "Util.h"
#include"CollisionInfo.h"
#include "Camera.h"
#include "RootPipe.h"
#include "IModel.h"


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

private:
	// グラフィックスパイプライン設定
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_;

private:
	//親インスタンス
	IObject* parentObj_ = nullptr;

protected://継承先まで公開
	//クラス名(デバッグ用)
	std::string objName_;
	//コライダー
	std::unique_ptr<BaseCollider> collider_ = nullptr;


private:
	//パイプラインの設定
	static void PipeLineState(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe);

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
	//
	WorldMat* GetWorldMat() { return worldMat_.get(); }
	//親子関係を考慮した位置をゲット
	Vec3 GetWorldTrans() { return worldMat_->GetWorldTrans(); }

	//オブジェクト名前
	void SetObjName(std::string objName) { objName_ = objName; }
	const std::string& GetObjName() { return objName_; }
	//コライダー
	BaseCollider* GetCollider() { return collider_.get(); }

public:

public:

};