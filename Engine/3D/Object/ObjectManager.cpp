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
	//アップデート
	for (std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<Object>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			//有効であれば
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

	//左クリックして、デバッグカメラになっていて、imgui上にカーソルがなければ
	if (CameraManager::GetInstance().GetIsDebugCamera() && MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT)
		&& ImGui::GetIO().WantCaptureMouse == false)
	{
		Camera* camera = CameraManager::GetInstance().GetCamera();

		//クリックした場所にレイをだす
		Vec3 nearPos;
		Vec3 farPos;
		Vec2toNearFarPos(MouseInput::GetInstance().GetCurcorPos(), nearPos, farPos, camera->GetViewMat(), camera->GetProjMat());

		Ray ray;
		ray.start = { camera->GetEye().x,camera->GetEye().y,camera->GetEye().z };

		Vec3 dirV = (farPos - camera->GetEye()).GetNormalized();
		ray.dir = { dirV.x,dirV.y,dirV.z };

		RaycastHit info;
		//何かあったら
		if (CollisionManager::GetInstance()->Raycast(ray, &info, 1000))
		{
			if (selectObj_)
			{
				selectObj_->effectFlags_.isSilhouette = false;
			}

			//前回と同じだったら解除
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
		selectObj_->effectFlags_.isSilhouette = true;

		selectObj_->EffectUpdate();
	}

#endif // _DEBUG
}


void ObjectManager::Draw()
{
	for (std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<Object>>::iterator itG = itM->second.begin();
			itG != itM->second.end(); itG++)
		{
			//有効であれば
			if (itG->get()->GetIsValid())
			{
				itG->get()->Draw();
			}
		}
	}

}

void ObjectManager::DrawImGui()
{
	//クリックで選択したものを出す
	if (selectObj_)
	{
		selectObj_->DrawImGui();
	}


	for (std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		//グループごとに
		if (ImGui::TreeNode(itM->first.c_str()))
		{
			for (std::vector<std::unique_ptr<Object>>::iterator itG = itM->second.begin();
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
	//コライダーで生きてるフラグオフになったら消す
	for (std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator itM = objsGroups_.begin();
		itM != objsGroups_.end(); itM++)
	{
		for (std::vector<std::unique_ptr<Object>>::iterator itG = itM->second.begin();
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
	//グループ探す
	auto itr = objsGroups_.find(name);

	//なければ
	if (itr == objsGroups_.end())
	{
		return false;
	}

	return true;
}

std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator ObjectManager::GetObjGroup(const std::string& name)
{
	//グループ探す
	auto itr = objsGroups_.find(name);

	//なければ
	assert(itr != objsGroups_.end());

	return itr;
}

//---------------------------------------------------------------------------------------------
void ObjectManager::AddObject(const std::string& groupName, const std::string& name)
{
	std::unique_ptr<Object>obj = std::make_unique<Object>();

	obj->Initialize();
	obj->SetObjName(name);

	//グループに追加、グループがまだなければグループの配列にも追加
	AddObjAndGroup(std::move(obj), groupName);
}

void ObjectManager::AddObject(const std::string& groupName, std::unique_ptr<Object> inst)
{
	//グループに追加、グループがまだなければグループの配列にも追加
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
	//グループがすでにあれば
	if (FindObjGroup(groupName))
	{
		auto itr = GetObjGroup(groupName);

		itr->second.push_back(std::move(obj));
	}
	//なければ新しく作る
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
	//配列で返す(オブジェクトの名前で探して)
	return GetObjectInternal(groupName, [=](Object* obj) {return GetSameObjName(obj, name); });
}

std::vector<Object*> ObjectManager::GetObjs(const std::string& groupName, uint16_t attribute)
{
	//配列で返す(判定属性で探して)
	return GetObjectInternal(groupName, [=](Object* obj) {return GetSameAttribute(obj, attribute); });
}

std::vector<Object*> ObjectManager::GetObjs(const std::string& groupName)
{
	//グループ全て返す
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

	//グループが見つからなければ何も入ってない配列返す
	if (FindObjGroup(groupName))
	{
		//グループ探す
		auto& objects = GetObjGroup(groupName)->second;

		for (std::vector<std::unique_ptr<Object>>::iterator itr = objects.begin();
			itr != objects.end(); itr++)
		{
			//条件クリアすれば
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

//グループごとに削除
void ObjectManager::ClearGroup(const std::string& groupName)
{
	if (FindObjGroup(groupName))
	{
		selectObj_ = nullptr;
		GetObjGroup(groupName)->second.clear();
	}
}