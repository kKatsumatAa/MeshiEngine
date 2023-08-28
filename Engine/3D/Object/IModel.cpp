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

void IModel::AddMaterial(std::unique_ptr<Material> material)
{
	// コンテナに登録
	materials_.insert(std::make_pair(material->name_, std::move(material)));
}


void IModel::Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex
	, const ConstBuffTransform& cbt)
{
	//パイプラインなどセット
	setRootParam();
	//ライトなどをセット
	setMaterialLightTex();

	// 全メッシュを描画
	for (auto& mesh : meshes_) {
		mesh->Draw(materialExtend_, cbt, setRootParam, setMaterialLightTex);
	}
}

void IModel::DrawImGui()
{
	if (ImGui::TreeNode("model"))
	{
		ImGui::SliderFloat("scaleExtend", &scaleExtend_, 0, 10.0f);
		ImGui::SliderFloat3("materialExtend", &materialExtend_.x_, 0, 5.0f);

		ImGui::TreePop();
	}
}

void IModel::SetPolygonOffsetData(const Mesh::PolygonOffset& polygonOffsetData)
{
	//仮で全部のメッシュに適用
	for (auto& mesh : meshes_)
	{
		mesh->SetPolygonOffsetData(polygonOffsetData);
	}
}
