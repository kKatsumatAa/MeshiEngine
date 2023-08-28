#include "IModel.h"
#include "ImGuiManager.h"


/// <summary>
/// �ÓI�����o�ϐ��̎���
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
	// ���b�V���̃}�e���A���`�F�b�N
	for (auto& m : meshes_) {
		// �}�e���A���̊��蓖�Ă��Ȃ�
		if (m && m->GetMaterial() == nullptr) {
			if (defaultMaterial_ == nullptr) {
				// �f�t�H���g�}�e���A���𐶐�
				defaultMaterial_ = Material::Create();
				defaultMaterial_->name_ = "no material";

				// �f�t�H���g�}�e���A�����Z�b�g(���b�V���ɓn���̂̓|�C���^�̂݁i���L���͓n���Ȃ��j)
				m->SetMaterial(defaultMaterial_.get());

				//�z��ɒǉ��i���L���n���j
				materials_.insert(std::make_pair(defaultMaterial_->name_, std::move(defaultMaterial_)));
			}
			else
			{
				// �f�t�H���g�}�e���A�����Z�b�g(���b�V���ɓn���̂̓|�C���^�̂݁i���L���͓n���Ȃ��j)
				m->SetMaterial(defaultMaterial_.get());
			}
		}
	}

	// ���b�V���̃o�b�t�@����
	for (auto& m : meshes_) {
		if (m) {
			m->CreateBuffers();
		}
	}
	// �}�e���A���̐��l��萔�o�b�t�@�ɔ��f
	for (auto& m : materials_) {
		m.second->Update();
	}

	// �e�N�X�`���̓ǂݍ���
	LoadTextures();
}


void IModel::LoadTexturesInternal(const std::string& directoryPath)
{
	for (auto& m : materials_) {
		Material* material = m.second.get();

		// �}�e���A���Ƀe�N�X�`���ǂݍ���
		material->LoadTexture(directoryPath);
	}
}

void IModel::AddMaterial(std::unique_ptr<Material> material)
{
	// �R���e�i�ɓo�^
	materials_.insert(std::make_pair(material->name_, std::move(material)));
}


void IModel::Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex
	, const ConstBuffTransform& cbt)
{
	//�p�C�v���C���ȂǃZ�b�g
	setRootParam();
	//���C�g�Ȃǂ��Z�b�g
	setMaterialLightTex();

	// �S���b�V����`��
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
	//���őS���̃��b�V���ɓK�p
	for (auto& mesh : meshes_)
	{
		mesh->SetPolygonOffsetData(polygonOffsetData);
	}
}
