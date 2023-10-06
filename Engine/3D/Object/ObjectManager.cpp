#include "ObjectManager.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "MouseInput.h"
#include "ImGuiManager.h"


ObjectManager& ObjectManager::GetInstance()
{
	static ObjectManager sInst;
	return sInst;
}

void ObjectManager::Initialize()
{
	objsGroups_.clear();
}

void ObjectManager::Update()
{
	//�A�b�v�f�[�g
	for (std::map<std::string, std::vector<std::unique_ptr<IObject3D>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<IObject3D>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			//�L���ł����
			if (itG->get()->GetIsValid())
			{
				itG->get()->Update();
			}
		}
	}

	PostUpdate();
}

void ObjectManager::DebugUpdate()
{
#ifdef _DEBUG

	//���N���b�N���āA�f�o�b�O�J�����ɂȂ��Ă��āAimgui��ɃJ�[�\�����Ȃ����
	if (CameraManager::GetInstance().GetIsDebugCamera() && MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT)
		&& ImGui::GetIO().WantCaptureMouse == false)
	{
		Camera* camera = CameraManager::GetInstance().GetCamera();

		//�N���b�N�����ꏊ�Ƀ��C������
		Vec3 nearPos;
		Vec3 farPos;
		Vec2toNearFarPos(MouseInput::GetInstance().GetCurcorPos(), nearPos, farPos, camera->GetViewMat(), camera->GetProjMat());

		Ray ray;
		ray.start = { camera->GetEye().x,camera->GetEye().y,camera->GetEye().z };

		Vec3 dirV = (farPos - camera->GetEye()).GetNormalized();
		ray.dir = { dirV.x,dirV.y,dirV.z };

		RaycastHit info;
		//������������
		if (CollisionManager::GetInstance()->Raycast(ray, &info, 1000))
		{
			//�V���G�b�g�����őI�����ĂȂ��悤�Ɍ�����
			if (selectObj_)
			{
				selectObj_->SetIsSilhouette(false);
			}

			//�O��Ɠ��������������
			if (selectObj_ == info.object)
			{
				selectObj_ = nullptr;
			}
			else
			{
				selectObj_ = info.object;
				CameraManager::GetInstance().GetCamera()->SetTarget(selectObj_->GetWorldTrans());
			}
		}

	}
	else if (!CameraManager::GetInstance().GetIsDebugCamera())
	{
		selectObj_ = nullptr;
	}

	if (selectObj_)
	{
		selectObj_->SetIsSilhouette(true);

		selectObj_->EffectUpdate();
	}

#endif // _DEBUG
}


void ObjectManager::Draw()
{
	for (std::map<std::string, std::vector<std::unique_ptr<IObject3D>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<IObject3D>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			//�L���ł����
			if (itG->get()->GetIsValid())
			{
				itG->get()->Draw();
			}
		}
	}

}

void ObjectManager::DrawImGui()
{
	//�N���b�N�őI���������̂��o��
	if (selectObj_)
	{
		selectObj_->DrawImGui();
	}


	for (std::map<std::string, std::vector<std::unique_ptr<IObject3D>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		//�O���[�v���Ƃ�
		if (ImGui::TreeNode(itM->first.c_str()))
		{
			for (std::vector<std::unique_ptr<IObject3D>>::iterator itG = itM->second.begin();
				itG != itM->second.end(); itG++)
			{
				itG->get()->DrawImGui();
			}

			ImGui::TreePop();
		}
	}
}

void ObjectManager::PostUpdate()
{
	//�R���C�_�[�Ő����Ă�t���O�I�t�ɂȂ��������
	for (std::map<std::string, std::vector<std::unique_ptr<IObject3D>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<IObject3D>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			if (!itG->get()->GetIsAlive())
			{
				if (itG->get() == selectObj_)
				{
					selectObj_ = nullptr;
				}

				itM->second.erase(itG);
				if (itM->second.size())
				{
					itG = itM->second.begin();
				}
				else
				{
					break;
				}
			}
		}
	}
}

//-------------------------------------------------------------
bool ObjectManager::FindObjGroup(const std::string& name)
{
	//�O���[�v�T��
	auto itr = objsGroups_.find(name);

	//�Ȃ����
	if (itr == objsGroups_.end())
	{
		return false;
	}

	return true;
}

std::map<std::string, std::vector<std::unique_ptr<IObject3D>>>::iterator ObjectManager::GetObjGroup(const std::string& name)
{
	//�O���[�v�T��
	auto itr = objsGroups_.find(name);

	//�Ȃ����
	assert(itr != objsGroups_.end());

	return itr;
}

//---------------------------------------------------------------------------------------------
void ObjectManager::AddObject(const std::string& groupName, const std::string& name)
{
	std::unique_ptr<IObject3D>obj = std::make_unique<IObject3D>();

	obj->Initialize();
	obj->SetObjName(name);

	//�O���[�v�ɒǉ��A�O���[�v���܂��Ȃ���΃O���[�v�̔z��ɂ��ǉ�
	AddObjAndGroup(std::move(obj), groupName);
}

void ObjectManager::AddObject(const std::string& groupName, std::unique_ptr<IObject3D> inst)
{
	//�O���[�v�ɒǉ��A�O���[�v���܂��Ȃ���΃O���[�v�̔z��ɂ��ǉ�
	AddObjAndGroup(std::move(inst), groupName);
}

void ObjectManager::AddGroup(const std::string& groupName, std::unique_ptr<IObject3D> inst)
{
	std::vector<std::unique_ptr<IObject3D>> group;
	group.push_back(std::move(inst));

	objsGroups_.insert(std::make_pair(groupName, std::move(group)));
}

void ObjectManager::AddObjAndGroup(std::unique_ptr<IObject3D> obj, const std::string& groupName)
{
	//�O���[�v�����łɂ����
	if (FindObjGroup(groupName))
	{
		auto itr = GetObjGroup(groupName);

		itr->second.push_back(std::move(obj));
	}
	//�Ȃ���ΐV�������
	else
	{
		AddGroup(groupName, std::move(obj));
	}
}

//---------------------------------------------------------------------------------------
bool ObjectManager::GetSameAttribute(IObject3D* obj, uint16_t attribute)
{
	if (obj->GetCollider()->GetAttribute() & attribute)
	{
		return true;
	}

	return false;
}

bool ObjectManager::GetSameObjName(IObject3D* obj, const std::string& name)
{
	if (obj->GetObjName() == name)
	{
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------
std::vector<IObject3D*> ObjectManager::GetObjs(const std::string& groupName, const std::string& name)
{
	//�z��ŕԂ�(�I�u�W�F�N�g�̖��O�ŒT����)
	return GetObjectInternal(groupName, [=](IObject3D* obj) {return GetSameObjName(obj, name); });
}

std::vector<IObject3D*> ObjectManager::GetObjs(const std::string& groupName, uint16_t attribute)
{
	//�z��ŕԂ�(���葮���ŒT����)
	return GetObjectInternal(groupName, [=](IObject3D* obj) {return GetSameAttribute(obj, attribute); });
}

std::vector<IObject3D*> ObjectManager::GetObjs(const std::string& groupName)
{
	//�O���[�v�S�ĕԂ�
	return GetObjectInternal(groupName, [=](IObject3D* obj) {return true; });
}

std::vector<IObject3D*> ObjectManager::GetObjs()
{
	std::vector<IObject3D*>rObjs = {};

	for (std::map<std::string, std::vector<std::unique_ptr<IObject3D>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<IObject3D>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			rObjs.push_back(itG->get());
		}
	}

	return rObjs;
}

std::vector<IObject3D*> ObjectManager::GetObjectInternal(const std::string& groupName, const std::function<bool(IObject3D* obj)>& isF)
{
	std::vector<IObject3D*>rObjs = {};

	//�O���[�v��������Ȃ���Ή��������ĂȂ��z��Ԃ�
	if (FindObjGroup(groupName))
	{
		//�O���[�v�T��
		auto& objects = GetObjGroup(groupName)->second;

		for (std::vector<std::unique_ptr<IObject3D>>::iterator itr = objects.begin();
			itr != objects.end(); itr++)
		{
			//�����N���A�����
			bool is = isF(itr->get());
			if (is)
			{
				rObjs.push_back(itr->get());
			}
		}
	}

	return rObjs;
}

//---------------------------------------------------------------------------------------------
void ObjectManager::ClearAllObj()
{
	selectObj_ = nullptr;
	objsGroups_.clear();
}

//�O���[�v���Ƃɍ폜
void ObjectManager::ClearGroup(const std::string& groupName)
{
	if (FindObjGroup(groupName))
	{
		selectObj_ = nullptr;
		GetObjGroup(groupName)->second.clear();
	}
}