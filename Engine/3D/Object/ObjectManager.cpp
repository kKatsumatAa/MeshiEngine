#include "ObjectManager.h"
#include "CameraManager.h"
#include "CollisionManager.h"


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
	for (std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<Object>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			itG->get()->Update();
		}
	}

	//�R���C�_�[�Ő����Ă�t���O�I�t�ɂȂ��������
	for (std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<Object>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			if (!itG->get()->GetIsAlive())
			{
				itG->reset();
				*itG = nullptr;
				itM->second.erase(itG);
				itG = itM->second.begin();
			}
		}
	}


#ifdef _DEBUG

	DebugUpdate();

#endif // _DEBUG
}

void ObjectManager::DebugUpdate()
{
}


void ObjectManager::Draw()
{
	for (std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<Object>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			itG->get()->Draw();
		}
	}

}

void ObjectManager::DrawImGui()
{
	//��(�N���b�N�őI���������̂̏o������)
	if (objsGroups_.size())
	{
		objsGroups_.begin()->second.begin()->get()->DrawImGui();
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

std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator ObjectManager::GetObjGroup(const std::string& name)
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
	std::unique_ptr<Object>obj = std::make_unique<Object>();

	obj->Initialize();
	obj->SetObjName(name);

	//�O���[�v�ɒǉ��A�O���[�v���܂��Ȃ���΃O���[�v�̔z��ɂ��ǉ�
	AddObjAndGroup(std::move(obj), groupName);
}

void ObjectManager::AddObject(const std::string& groupName, std::unique_ptr<Object> inst)
{
	//�O���[�v�ɒǉ��A�O���[�v���܂��Ȃ���΃O���[�v�̔z��ɂ��ǉ�
	AddObjAndGroup(std::move(inst), groupName);
}

void ObjectManager::AddGroup(const std::string& groupName, std::unique_ptr<Object> inst)
{
	std::vector<std::unique_ptr<Object>> group;
	group.push_back(std::move(inst));

	objsGroups_.insert(std::make_pair(groupName, std::move(group)));
}

void ObjectManager::AddObjAndGroup(std::unique_ptr<Object> obj, const std::string& groupName)
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
bool ObjectManager::GetSameAttribute(Object* obj, uint16_t attribute)
{
	if (obj->GetCollider()->GetAttribute() & attribute)
	{
		return true;
	}

	return false;
}

bool ObjectManager::GetSameObjName(Object* obj, const std::string& name)
{
	if (obj->GetObjName() == name)
	{
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------
std::vector<Object*> ObjectManager::GetObjs(const std::string& groupName, const std::string& name)
{
	//�z��ŕԂ�(�I�u�W�F�N�g�̖��O�ŒT����)
	return GetObjectInternal(groupName, [=](Object* obj) {return GetSameObjName(obj, name); });
}

std::vector<Object*> ObjectManager::GetObjs(const std::string& groupName, uint16_t attribute)
{
	//�z��ŕԂ�(���葮���ŒT����)
	return GetObjectInternal(groupName, [=](Object* obj) {return GetSameAttribute(obj, attribute); });
}

std::vector<Object*> ObjectManager::GetObjs(const std::string& groupName)
{
	//�O���[�v�S�ĕԂ�
	return GetObjectInternal(groupName, [=](Object* obj) {return true; });
}

std::vector<Object*> ObjectManager::GetObjs()
{
	std::vector<Object*>rObjs = {};

	for (std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<Object>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			rObjs.push_back(itG->get());
		}
	}

	return rObjs;
}

std::vector<Object*> ObjectManager::GetObjectInternal(const std::string& groupName, const std::function<bool(Object* obj)>& isF)
{
	std::vector<Object*>rObjs = {};

	//�O���[�v��������Ȃ���Ή��������ĂȂ��z��Ԃ�
	if (FindObjGroup(groupName))
	{
		//�O���[�v�T��
		auto& objects = GetObjGroup(groupName)->second;

		for (std::vector<std::unique_ptr<Object>>::iterator itr = objects.begin();
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
void ObjectManager::ClearGroup(const std::string& groupName)
{
	if (FindObjGroup(groupName))
	{
		GetObjGroup(groupName)->second.clear();
	}
}