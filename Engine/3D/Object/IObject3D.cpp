#include"IObject3D.h"
#include"ObjectFBX.h"
#include"BaseCollider.h"

using namespace DirectX;

//--------------------------------------------------------
//���C�g
LightManager* IObject3D::sLightManager_ = nullptr;
//�C���v�b�g���C�A�E�g
D3D12_INPUT_ELEMENT_DESC IObject3D::sInputLayoutM_[7] = {
	{//xyz���W
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�@���x�N�g��
	 "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//uv���W
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�e�����󂯂�{�[���ԍ�
	 "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�{�[���̃X�L���E�F�C�g�i4�j
	 "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�ڐ�
	 "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�]�@��
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

	//�C���X�^���X�^�C�v
	objInsType_ = ObjectInstanceType::THREE_D;

	//���o�p
	{
		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
		//���\�[�X�ݒ�
		ResourceProperties(cbResourceDesc,
			((uint32_t)sizeof(EffectOConstBuffer) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/);
		//�萔�o�b�t�@�̐���
		BuffProperties(cbHeapProp, cbResourceDesc, &effectFlagsBuff_);
		//�萔�o�b�t�@�̃}�b�s���O
		result = effectFlagsBuff_->Map(0, nullptr, (void**)&mapEffectFlagsBuff_);//�}�b�s���O
		assert(SUCCEEDED(result));
	}

	//�g�����X�t�H�[���s��
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
void IObject3D::EffectUpdate()
{
	effectFlags_.time++;

	//��ʌ��ʗp
	{
		*mapEffectFlagsBuff_ = effectFlags_;
	}
}

void IObject3D::Update()
{
	//���o�X�V
	EffectUpdate();
	//�e�N���X��
	IObject::Update();
}

//------------------------------
void IObject3D::DrawModel(Camera* camera, bool isWireFrame)
{
	Camera* lCamera = camera;
	//�J�������Z�b�g����ĂȂ���Ύg�p����Ă�J�������g��
	if (camera == nullptr)
	{
		lCamera = CameraManager::GetInstance().usingCamera_;
	}

	//�s��}�b�s���O
	MatMap(lCamera, model_);

	//�`��R�}���h�Ȃ�(false��������[0]�Œʏ�`��)
	DrawModelInternal((int32_t)isWireFrame);
}

void IObject3D::Draw()
{
	//���f����`��
	DrawModel();
}

//--------------------------------------------------------------------------------
void IObject3D::SetMaterialLightMTex(uint64_t textureHandle, uint64_t dissolveTex, uint64_t specularMapTex,
	uint64_t normalMapTex, bool setTex)
{
	//���b�V���̍\��
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//SRV�q�[�v�̐ݒ�R�}���h
	DirectXWrapper::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetDescHeapPP());

	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	//�e�N�X�`����ݒ肵�Ă��Ȃ�������
	uint64_t texHL = textureHandle;
	TextureManager::CheckTexHandle(texHL);
	srvGpuHandle.ptr = texHL;

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(COLOR, constBuffMaterial_->GetGPUVirtualAddress());

	//���f���̓��b�V�����Ƃɍs�񓙃Z�b�g����̂�
	//�e�N�X�`��
	if (setTex)
	{
		//(�C���X�^���X�œǂݍ��񂾃e�N�X�`���p��SRV���w��)
		DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(TEX, srvGpuHandle);
	}
	//�s��
	cbt_.DrawCommand(MATRIX);

	//���C�g
	sLightManager_->Draw(LIGHT);

	//�f�B�]���u�e�N�X�`��
	srvGpuHandle.ptr = dissolveTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(DISSOLVE, srvGpuHandle);

	//�X�y�L�����}�b�v�e�N�X�`��
	srvGpuHandle.ptr = specularMapTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(SPECULAR_MAP, srvGpuHandle);

	//�m�[�}���}�b�v�e�N�X�`��
	srvGpuHandle.ptr = normalMapTex;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(NORM_MAP, srvGpuHandle);

	//���o�t���O
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(EFFECT, effectFlagsBuff_->GetGPUVirtualAddress());
}

//--------------------------------------------------------------------------------
void IObject3D::MatMap(Camera* camera, IModel* model)
{
	//�傫�������肷��̂�h���p��
	Vec3 scale = worldMat_->scale_;

	if (model)
	{
		worldMat_->scale_ = worldMat_->scale_ * model->GetScaleExtend();
	}

	XMMATRIX matW;

	//�ϊ��s���GPU�ɑ��M
	if (parentNode_ && worldMat_->parent_ &&
		parentObj_->GetObjInsType() == ObjectInstanceType::FBX && parentNodeModel_->GetIsFbx())
	{
		//�I�u�W�F�N�g�̃��[�J��
		worldMat_->CalcWorldMat();
		//�e�s��
		M4 pM = worldMat_->GetOnlyParentALLTreeMat();
		//�{�[��(�{�[���̃O���[�o���g�����X�t�H�[���A)
		M4 bM;
		{
			//�e�m�[�h�Ɠ������O�̃{�[���̃C���f�b�N�X�𓾂�
			uint64_t bIndex = parentNodeModel_->GetBoneIndex(parentNode_->name);
			//�e�m�[�h�����郂�f���̃{�[�����C���f�b�N�X�Ŏw�肵�čs��̌v�Z
			XMMATRIX bXM;
			//�e�̃��[���h�s��
			pM.MatIntoXMMATRIX(bXM);
			//�e�̃m�[�h�s��Ƀ��[���h�s����|����
			auto modelL = dynamic_cast<ObjectFBX*>(parentObj_);
			bXM = modelL->GetNodes()[bIndex].globalTransform;
			bM.PutInXMMATRIX(bXM);
			bM.m_[3][0] *= parentNodeModel_->GetScaleExtend();
			bM.m_[3][1] *= parentNodeModel_->GetScaleExtend();
			bM.m_[3][2] *= parentNodeModel_->GetScaleExtend();
		}
		(worldMat_->matWorld_ * (bM * pM)).MatIntoXMMATRIX(matW);
		worldMat_->matWorld_.PutInXMMATRIX(matW);
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
		//���ʃx�N�g��
		if (ImGui::TreeNode("FrontVec")) {

			ImGui::Text("FrontVec: %.2f %.2f %.2f", frontVec_.x, frontVec_.y, frontVec_.z);
			ImGui::Text("FrontVecTmp: %.2f %.2f %.2f", frontVecTmp_.x, frontVecTmp_.y, frontVecTmp_.z);

			ImGui::TreePop();
		}

		//���f��
		if (ImGui::TreeNode("Model")) {

			std::string modelPState = "MODEL_NULL";
			if (model_ != nullptr)
			{
				modelPState = "MODEL_SET";
			}
			ImGui::Text(modelPState.c_str());
			if (model_ != nullptr)
			{
				ImGui::Text("modelIsFbx: %d", model_->GetIsFbx());
			}
			if (model_)
			{
				model_->DrawImGui();
			}
			ImGui::TreePop();

			//�q�N���X�̏���
			if (imguiF)
			{
				imguiF();
			}
		}

		//���o
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

	//�e�N���X��
	IObject::DrawImGui(f);
}
