#pragma once
#include "Util.h"
#include"CollisionInfo.h"
#include "Camera.h"
#include "RootPipe.h"
#include "IModel.h"
#include <string>
#include <vector>
#include "ImguiManager.h"


//繧ｳ繝ｩ繧､繝繝ｼ縺ｮ隕ｪ繧ｯ繝ｩ繧ｹ蜑肴婿螳｣險
class BaseCollider;

struct ConstBufferDataMaterial
{
	Vec4 color = { 1.0f,1.0f,1.0f,1.0f };//濶ｲ(RGBA)
};

//繧ｪ繝悶ず繧ｧ繧ｯ繝郁ｦｪ繧ｯ繝ｩ繧ｹ
class IObject
{
protected://繧ｨ繧､繝ｪ繧｢繧ｹ
	//Microsoft::WRL::繧堤怐逡･
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::繧堤怐逡･
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//繝ｫ繝ｼ繝医ヱ繝ｩ繝｡繝ｼ繧ｿ縺ｮ蠑墓焚
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
		//隕∫ｴ謨ｰ
		count = 11
	};

	//繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ縺ｮ繧ｿ繧､繝・
	enum ObjectInstanceType
	{
		UNKNOWN = -1,
		SPRITE,
		OBJ,
		FBX,
		THREE_D
	};

private:
	// 繧ｰ繝ｩ繝輔ぅ繝・け繧ｹ繝代う繝励Λ繧､繝ｳ險ｭ螳・
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_;

private:
	//貍泌・逕ｨ繝・せ繧ｯ繝ｪ繝励ち繝ｬ繝ｳ繧ｸ
	static D3D12_DESCRIPTOR_RANGE effectDescRange_[3];

protected://邯呎価蜈医∪縺ｧ蜈ｬ髢・
	//螳壽焚繝舌ャ繝輔ぃ縺ｮ逕滓・
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
	//螳壽焚繝舌ャ繝輔ぃ逕ｨ繝・・繧ｿ讒矩菴難ｼ医・繝・Μ繧｢繝ｫ・・
	//螳壽焚繝舌ャ繝輔ぃ縺ｮ繝槭ャ繝斐Φ繧ｰ
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;

private:
	//繝ｫ繝ｼ繝医ヱ繝ｩ繝｡繝ｼ繧ｿ縺ｮ險ｭ螳・
	static D3D12_ROOT_PARAMETER rootParams_[RootParamNum::count];

protected:
	//繝ｯ繝ｼ繝ｫ繝芽｡悟・逕ｨ縺ｮ繝舌ャ繝輔ぃ縺ｪ縺ｩ
	ConstBuffTransform cbt_;//縺薙％繧偵←縺・↓縺九☆繧後・縲√う繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ荳縺､縺ｧ繧り牡縲・緒逕ｻ
	//繝ｯ繝ｼ繝ｫ繝芽｡悟・逕ｨ繧ｯ繝ｩ繧ｹ
	std::unique_ptr<WorldMat> worldMat_;
	//隕ｪ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ
	IObject* parentObj_ = nullptr;

protected:
	//繧ｯ繝ｩ繧ｹ蜷・繝・ヰ繝・げ逕ｨ)
	std::string objName_;
	//逕溘″縺ｦ繧九ヵ繝ｩ繧ｰ
	bool isAlive_ = true;
	//謠冗判縲∵峩譁ｰ縺吶ｋ縺九←縺・°
	bool isValid_ = true;
	//騾溷ｺｦ
	Vec3 velocity_ = { 0,0,0 };
	//繧ｳ繝ｩ繧､繝繝ｼ
	std::unique_ptr<BaseCollider> collider_ = nullptr;
	//繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ縺ｮ遞ｮ鬘橸ｼ亥ｭ舌け繝ｩ繧ｹ縺御ｽ輔°・・
	ObjectInstanceType objInsType_ = ObjectInstanceType::UNKNOWN;


	//髢｢謨ｰ------------------------------------------------------------------------------
public:
	virtual ~IObject();
	IObject();

protected:
	//蜈ｱ騾壹・蛻晄悄蛹門・逅・
	void Initialize(std::unique_ptr<WorldMat> worldMat = nullptr);

public:
	//髱咏噪蛻晄悄蛹・
	static void CommonInitialize();

public:
	virtual void Update();

	virtual void Draw() = 0;

public:
	//陦悟・繧呈峩譁ｰ縲√◎繧後↓莨ｴ縺・さ繝ｩ繧､繝繝ｼ繧・
	void WorldMatColliderUpdate();

	//繝・ヰ繝・げ逕ｨ縺ｮ陦ｨ遉ｺ
	virtual void DrawImGui(std::function<void()>imguiF = NULL);

public:
	//繝ｫ繝ｼ繝医す繧ｰ繝阪メ繝｣邉ｻ縺ｮ繧ｳ繝槭Φ繝・
	void SetRootPipe(RootPipe* pipelineSet, int32_t pipelineNum, ID3D12RootSignature* rootSignature);

	//繝代う繝励Λ繧､繝ｳ縺ｮ險ｭ螳・
	static void PipeLineSetting(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe,
		const std::string& vSName, const std::string& pSName,
		D3D12_INPUT_ELEMENT_DESC* inputLayout, uint32_t inputLCount,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE priTopoType, bool isSprite);

	static void Blend(const D3D12_BLEND_OP& blendMode,
		bool Inversion = 0, bool Translucent = 0);

public:
	//陦晉ｪ∵凾繧ｳ繝ｼ繝ｫ繝舌ャ繧ｯ髢｢謨ｰ縲∬ｭｦ蜻企∩縺代ｋ縺溘ａ縺ｫ蜿ら・縺縺・
	virtual void OnCollision(const CollisionInfo& info) { info; }

public:
	//繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ縺ｮ遞ｮ鬘槭ｒ蠕励ｋ
	ObjectInstanceType GetObjInsType() { return objInsType_; }
	//2D縺・D縺・

public:
	//逕溘″縺ｦ繧九°
	void SetIsAlive(bool isAlive);
	bool GetIsAlive() { return isAlive_; }

	//謠冗判縲∵峩譁ｰ縺吶ｋ縺九←縺・°
	virtual void SetIsValid(bool isValid);
	bool GetIsValid() { return isValid_; }

	//繧ｹ繝斐・繝会ｼ亥ｽ薙◆繧雁愛螳壹↓菴ｿ縺・ｼ・
	void SetVelocity(const Vec3& vec) { velocity_ = vec; }
	const Vec3& GetVelocity() { return velocity_; }


public:
	//菴咲ｽｮ
	void SetTrans(const Vec3& pos) { worldMat_->trans_ = pos; }
	void SetTransX(float pos) { worldMat_->trans_.x = pos; }
	void SetTransY(float pos) { worldMat_->trans_.y = pos; }
	void SetTransZ(float pos) { worldMat_->trans_.z = pos; }
	Vec3 GetTrans() { return worldMat_->trans_; }
	//繧ｹ繧ｱ繝ｼ繝ｫ
	void SetScale(const Vec3& scale) { worldMat_->scale_ = scale; }
	void SetScaleX(float scale) { worldMat_->scale_.x = scale; }
	void SetScaleY(float scale) { worldMat_->scale_.y = scale; }
	void SetScaleZ(float scale) { worldMat_->scale_.z = scale; }
	const Vec3& GetScale() { return worldMat_->scale_; }
	//蝗櫁ｻ｢
	void SetRot(const Vec3& rot) {
		worldMat_->rot_ = rot;
	}
	void SetRotX(float rot) { worldMat_->rot_.x = rot; }
	void SetRotY(float rot) { worldMat_->rot_.y = rot; }
	void SetRotZ(float rot) { worldMat_->rot_.z = rot; }
	const Vec3& GetRot() { return worldMat_->rot_; }
	//陦悟・繧偵そ繝・ヨ
	void SetMatWorld(const M4& mat) { worldMat_->SetMatWorld(mat); }
	//陦悟・繧呈峩譁ｰ
	void CalcWorldMat() { worldMat_->CalcWorldMat(); }
	void CalcRotMat() { worldMat_->CalcRotMat(); }
	void CalcTransMat() { worldMat_->CalcTransMat(); }
	void CalcScaleMat() { worldMat_->CalcScaleMat(); }
	//隕ｪ
	void SetParent(IObject* obj) { worldMat_->parent_ = obj->GetWorldMat(); }
	void SetParent(WorldMat* worldMat) { worldMat_->parent_ = worldMat; }
	WorldMat* GetParent() { return worldMat_->parent_; }
	//繝ｯ繝ｼ繝ｫ繝芽｡悟・逕ｨ繧ｯ繝ｩ繧ｹ縺ｮ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ繧貞叙蠕・
	WorldMat* GetWorldMat() { return worldMat_.get(); }
	//隕ｪ蟄宣未菫ゅｒ閠・・縺励◆菴咲ｽｮ繧偵ご繝・ヨ
	Vec3 GetWorldTrans() { return worldMat_->GetWorldTrans(); }
	//繝ｯ繝ｼ繝ｫ繝芽｡悟・縺ｮ蜿門ｾ・
	const M4& GetMatWorld() { return worldMat_->matWorld_; }

	//繧ｯ繧ｩ繝ｼ繧ｿ繝九が繝ｳ邉ｻ
	Quaternion GetQuaternion() { return worldMat_->GetQuaternion(); }
	void SetIsUseQuaternionMatRot(bool is) { worldMat_->SetIsUseQuaMatRot(is); }
	void SetMatRot(const M4& m) { worldMat_->SetRotMat(m); }

public:
	//繧ｪ繝悶ず繧ｧ繧ｯ繝亥錐蜑・
	void SetObjName(std::string objName) { objName_ = objName; }
	const std::string& GetObjName() { return objName_; }

	//繧ｳ繝ｩ繧､繝繝ｼ
	BaseCollider* GetCollider() { return collider_.get(); }
	//繧ｳ繝ｩ繧､繝繝ｼ縺ｮ繧ｻ繝・ヨ
	void SetCollider(std::unique_ptr<BaseCollider> collider);
	//蛻､螳壹ｒ譛牙柑縺ｫ縺吶ｋ縺・
	void SetColliderIsValid(bool isValid);

public:
	//濶ｲ繧偵そ繝・ヨ
	void SetColor(const Vec4& color) { constMapMaterial_->color = color; }
	//濶ｲ繧定ｿ斐☆
	const Vec4& GetColor() { return constMapMaterial_->color; }

};

void Error(bool filed, ID3DBlob* errorBlob);
