#include"IObject3D.h"
#include"ObjectFBX.h"
#include"BaseCollider.h"

using namespace DirectX;

//--------------------------------------------------------
//ライト
LightManager* IObject3D::sLightManager_ = nullptr;
//インプットレイアウト
D3D12_INPUT_ELEMENT_DESC IObject3D::sInputLayoutM_[7] = {
	{//xyz座標
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//法線ベクトル
	 "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//uv座標
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//影響を受けるボーン番号
	 "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//ボーンのスキンウェイト（4つ）
	 "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//接線
	 "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//従法線
	 "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

};


//-------------------------------------------------------
IObject3D::~IObject3D()
{
	effectFlagsBuff_.Reset();
}

IObject3D::IObject3D()
{
	HRESULT result = S_OK;

	//インスタンスタイプ
	objInsType_ = ObjectInstanceType::THREE_D;

	//演出用
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		ResourceProperties(cbResourceDesc,
			((uint32_t)sizeof(EffectOConstBuffer) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &effectFlagsBuff_);
		//定数バッファのマッピング
		result = effectFlagsBuff_->Map(0, nullptr, (void**)&mapEffectFlagsBuff_);//マッピング
		assert(SUCCEEDED(result));
	}

	//トランスフォーム行列
	cbt_.Initialize();
}

//--------------------------------------------------------
void IObject3D::CommonInitialize()
{

}

bool IObject3D::Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	IObject::Initialize(std::move(worldMat));

	SetModel(model);

	return true;
}

//--------------------------------------------------------
void IObject3D::SetModel(IModel* model)
{
	isValidModel_ = false;
	if (model)
	{
		isValidModel_ = true;
	}

	model_ = model;
}

const Vec3& IObject3D::GetFrontVec()
{
	return frontVec_;
}

void IObject3D::CalcFrontVec()
{
	frontVec_ = GetTurnVec3UseQuaternionAndRot(frontVecTmp_, GetRot());
}

//--------------------------------------------------------
void IObject3D::EffectUpdate(IObject3D* effectCopyObj)
{
	effectFlags_.time++;

	//コピー
	if (effectCopyObj)
	{
		effectFlags_ = effectCopyObj->effectFlags_;
	}

	//画面効果用
	{
		*mapEffectFlagsBuff_ = effectFlags_;
	}
}

void IObject3D::Update()
{
	//演出更新
	EffectUpdate();
	//行列計算
	worldMat_->CalcAllTreeMat();
	//親クラスの
	IObject::Update();
}

//------------------------------
void IObject3D::DrawModel(Camera* camera, bool isWireFrame)
{
	if (!isValidModel_)
	{
		return;
	}

	Camera* lCamera = camera;
	//カメラがセットされてなければ使用されてるカメラを使う
	if (camera == nullptr)
	{
		lCamera = CameraManager::GetInstance().usingCamera_;
	}

	//行列マッピング
	MatMap(lCamera, model_);

	//描画コマンドなど(falseだったら[0]で通常描画)
	DrawModelInternal((int32_t)isWireFrame);
}

void IObject3D::Draw()
{
	//モデルを描画
	DrawModel();
}

//--------------------------------------------------------------------------------
void IObject3D::SetMaterialLightMTex(uint64_t textureHandle, uint64_t dissolveTex, uint64_t specularMapTex,
	uint64_t normalMapTex, bool setTex)
{
	//メッシュの構成
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//SRVヒープの設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetDescHeapPP());

	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	//テクスチャを設定していなかったら
	uint64_t texHL = textureHandle;
	TextureManager::CheckTexHandle(texHL);
	srvGpuHandle.ptr = texHL;

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(COLOR, constBuffMaterial_->GetGPUVirtualAddress());

	//モデルはメッシュごとに行列等セットするので
	//テクスチャ
	if (setTex)
	{
		//(インスタンスで読み込んだテクスチャ用のSRVを指定)
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(TEX, srvGpuHandle);
	}
	//行列
	cbt_.DrawCommand(MATRIX);

	//ライト
	sLightManager_->Draw(LIGHT);

	//ディゾルブテクスチャ
	srvGpuHandle.ptr = dissolveTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(DISSOLVE, srvGpuHandle);

	//スペキュラマップテクスチャ
	srvGpuHandle.ptr = specularMapTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(SPECULAR_MAP, srvGpuHandle);

	//ノーマルマップテクスチャ
	srvGpuHandle.ptr = normalMapTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(NORM_MAP, srvGpuHandle);

	//演出フラグ
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(EFFECT, effectFlagsBuff_->GetGPUVirtualAddress());
}

//--------------------------------------------------------------------------------
void IObject3D::MatMap(Camera* camera, IModel* model)
{
	//大きすぎたりするのを防ぐ用に
	Vec3 scale = worldMat_->scale_;

	if (model)
	{
		worldMat_->scale_ = worldMat_->scale_ * model->GetScaleExtend();
	}

	XMMATRIX matW;

	//変換行列をGPUに送信
	if (parentNode_ && worldMat_->parent_ &&
		parentObj_->GetObjInsType() == ObjectInstanceType::FBX && parentNodeModel_->GetIsFbx())
	{
		//親のスケールは反映しない
		Vec3 pScale = worldMat_->parent_->scale_;
		worldMat_->parent_->scale_ = { 1.0f,1.0f,1.0f };

		//オブジェクトのローカル
		worldMat_->CalcWorldMat();
		//親行列
		M4 pM = worldMat_->GetOnlyParentALLTreeMat();
		//ボーン(ボーンのグローバルトランスフォーム、)
		M4 bM;
		{
			//親ノードと同じ名前のボーンのインデックスを得る
			uint64_t bIndex = parentNodeModel_->GetBoneIndex(parentNode_->name);
			//親ノードがあるモデルのボーンをインデックスで指定して行列の計算
			XMMATRIX bXM;
			//親のワールド行列
			pM.MatIntoXMMATRIX(bXM);
			//親のノード行列にワールド行列を掛ける
			auto modelL = dynamic_cast<ObjectFBX*>(parentObj_);
			bXM = modelL->GetNodes()[bIndex].globalTransform;
			bM.PutInXMMATRIX(bXM);
			bM.m_[3][0] *= parentNodeModel_->GetScaleExtend() * pScale.x;
			bM.m_[3][1] *= parentNodeModel_->GetScaleExtend() * pScale.y;
			bM.m_[3][2] *= parentNodeModel_->GetScaleExtend() * pScale.z;
		}
		(worldMat_->matWorld_ * (bM * pM)).MatIntoXMMATRIX(matW);

		//親のスケール戻す
		worldMat_->parent_->scale_ = pScale;
	}
	else
	{
		worldMat_->CalcAllTreeMat();
		worldMat_->matWorld_.MatIntoXMMATRIX(matW);
	}

	worldMat_->scale_ = scale;

	cbt_.SetWorldMat(matW);
	cbt_.SetViewProjMat(camera->GetViewMat() * camera->GetProjMat());
	cbt_.SetCameraPos(camera->GetEye());
}

//-------------------------------------------------------------------------------
void IObject3D::ParentFbxNode(IObject* obj, IModel* model, const std::string& nodeName)
{
	ModelFBX* modelL = dynamic_cast<ModelFBX*>(model);

	if (model->GetIsFbx() && modelL->GetNode(nodeName))
	{
		parentNode_ = modelL->GetNode(nodeName);
		parentNodeModel_ = modelL;
		parentObj_ = obj;
		return;
	}

	assert(false);
}

void IObject3D::ResetParentFbxNode()
{
	parentNode_ = nullptr;
	parentNodeModel_ = nullptr;
}

//--------------------------------------------------------
void IObject3D::DrawImGui(std::function<void()> imguiF)
{
	auto f = [=]() {
		//正面ベクトル
		if (ImGui::TreeNode("FrontVec")) {

			ImGui::Text("FrontVec: %.2f %.2f %.2f", frontVec_.x, frontVec_.y, frontVec_.z);
			ImGui::Text("FrontVecTmp: %.2f %.2f %.2f", frontVecTmp_.x, frontVecTmp_.y, frontVecTmp_.z);

			ImGui::TreePop();
		}

		//モデル
		if (ImGui::TreeNode("Model")) {

			//モデルが有効か（描画するか）
			ImGui::Checkbox("isValidModel", &isValidModel_);
			if (model_ != nullptr)
			{
				ImGui::Text("modelIsFbx: %d", model_->GetIsFbx());
			}
			if (model_)
			{
				model_->DrawImGui();
			}
			ImGui::TreePop();

			//子クラスの処理
			if (imguiF)
			{
				imguiF();
			}
		}

		//演出
		if (ImGui::TreeNode("Effect")) {

			ImGui::Checkbox("isDissolve", (bool*)&effectFlags_.isDissolve);
			ImGui::SliderFloat("dissolveT", &effectFlags_.dissolveT, 0, 1.0f);
			ImGui::Checkbox("isFog", (bool*)&effectFlags_.isFog);
			ImGui::Checkbox("isNormalMap", (bool*)&effectFlags_.isNormalMap);
			ImGui::Checkbox("isRimLight", (bool*)&effectFlags_.isRimLight);
			ImGui::ColorEdit3("rimLightColor", &effectFlags_.rimColor.x);
			ImGui::Checkbox("isSilhouette", (bool*)&effectFlags_.isSilhouette);
			ImGui::ColorEdit3("silhouetteColor", &effectFlags_.silhouetteColor.x);
			ImGui::Checkbox("isSpecularMap", (bool*)&effectFlags_.isSpecularMap);

			ImGui::TreePop();
		}
		};

	//親クラスの
	IObject::DrawImGui(f);
}
