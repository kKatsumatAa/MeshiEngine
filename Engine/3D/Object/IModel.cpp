#include "IModel.h"
#include "ImGuiManager.h"


/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string IModel::S_BASE_DIRECTORY_ = "Resources/model/";

//--------------------------------------------------------------------------


IModel::~IModel()
{
	meshes_.clear();
	materials_.clear();
}

void IModel::StaticInitialize()
{
	Mesh::StaticInitialize(DirectXWrapper::GetInstance().GetDevice());
}

void IModel::Initialize()
{
	// メッシュのマテリアルチェック
	for (auto& m : meshes_) {
		// マテリアルの割り当てがない
		if (m && m->GetMaterial() == nullptr) {
			if (defaultMaterial_ == nullptr) {
				// デフォルトマテリアルを生成
				defaultMaterial_ = Material::Create();
				defaultMaterial_->name_ = "no material";

				// デフォルトマテリアルをセット(メッシュに渡すのはポインタのみ（所有権は渡さない）)
				m->SetMaterial(defaultMaterial_.get());

				//配列に追加（所有権渡す）
				materials_.insert(std::make_pair(defaultMaterial_->name_, std::move(defaultMaterial_)));
			}
			else
			{
				// デフォルトマテリアルをセット(メッシュに渡すのはポインタのみ（所有権は渡さない）)
				m->SetMaterial(defaultMaterial_.get());
			}
		}
	}

	// メッシュのバッファ生成
	for (auto& m : meshes_) {
		if (m) {
			m->CreateBuffers();
		}
	}
	// マテリアルの数値を定数バッファに反映
	for (auto& m : materials_) {
		m.second->Update();
	}

	// テクスチャの読み込み
	LoadTextures();
}


void IModel::LoadTexturesInternal(const std::string& directoryPath)
{
	for (auto& m : materials_) {
		Material* material = m.second.get();

		// マテリアルにテクスチャ読み込み
		material->LoadTexture(directoryPath);
	}
}

void IModel::Draw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex
	, const ConstBuffTransform& cbt, bool isShadow)
{
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(topology);

	//パイプラインなどセット
	setRootParam();
	//ライトなどをセット
	setMaterialLightTex();

	// 全メッシュを描画
	for (auto& mesh : meshes_) {
		mesh->Draw(materialExtend_, cbt, isShadow);
	}
}

void IModel::DrawImGui()
{
	if (ImGui::TreeNode("model"))
	{
		ImGui::SliderFloat("scaleExtend", &scaleExtend_, IMGUI_SCALE_EXTEND_MIN_, IMGUI_SCALE_EXTEND_MAX_);
		ImGui::DragFloat3("materialExtend", &materialExtend_.x, IMGUI_MATERIAL_EXTEND_DRAG_SPEED_,
			IMGUI_MATERIAL_EXTEND_MIN_, IMGUI_MATERIAL_EXTEND_MAX_);

		ImGui::TreePop();
	}
}

Mesh* IModel::GetMesh(const std::string& name)
{
	for (auto& mesh : meshes_)
	{
		if (mesh->GetName() == name)
		{
			return mesh.get();
		}
	}

	return nullptr;
}

std::unique_ptr<Mesh> IModel::MoveUniqueMesh(const std::string& name)
{
	for (auto itr = meshes_.begin(); itr != meshes_.end(); itr++)
	{
		if (itr->get()->GetName() == name)
		{
			//名前が一致したら譲渡、配列から削除
			std::unique_ptr<Mesh> retMesh = std::move(*itr);
			meshes_.erase(itr);
			return std::move(retMesh);
		}
	}

	return nullptr;
}

void IModel::DeleteMesh(const std::string& name)
{
	for (auto itr = meshes_.begin(); itr != meshes_.end(); itr++)
	{
		if (itr->get()->GetName() == name)
		{
			//名前が一致したら削除
			meshes_.erase(itr);
			break;
		}
	}
}

void IModel::AddMesh(std::unique_ptr<Mesh> mesh)
{
	meshes_.push_back(std::move(mesh));
}

void IModel::SetPolygonOffsetData(const Mesh::PolygonOffset& polygonOffsetData)
{
	//仮で全部のメッシュに適用
	for (auto& mesh : meshes_)
	{
		mesh->SetPolygonOffsetData(polygonOffsetData);
	}
}

void IModel::CopyUnUniqueParams(const IModel& model)
{
	isFbx_ = model.GetIsFbx();
	materialExtend_ = model.GetMaterialExtend();
}

void IModel::MoveMeshToCenter(const std::string& meshName)
{
	for (auto& mesh : meshes_)
	{
		//名前が一致していたらメッシュを中心に
		if (mesh->GetName() == meshName)
		{
			mesh->MoveVerticesToCenter();
		}
	}
}

Vec3 IModel::GetMeshCentroid(const std::string& meshName)
{
	for (auto& mesh : meshes_)
	{
		//名前が一致していたら重心を得る
		if (mesh->GetName() == meshName)
		{
			return mesh->GetVerticesCentroid();
		}
	}

	return Vec3(0,0,0);
}

//----------------------------------------------------------
void IModel::AddMaterial(std::unique_ptr<Material> material)
{
	// コンテナに登録
	materials_.insert(std::make_pair(material->name_, std::move(material)));
}

std::unique_ptr<Material> IModel::MoveUniqueMaterial(Mesh* mesh)
{
	for (auto itr = materials_.begin(); itr != materials_.end(); itr++)
	{
		if (itr->second.get() == mesh->GetMaterial())
		{
			//マテリアルのポインタが一致したら譲渡、配列から削除
			std::unique_ptr<Material> retMaterial = std::move(itr->second);
			materials_.erase(itr);
			return std::move(retMaterial);
		}
	}

	return nullptr;
}
