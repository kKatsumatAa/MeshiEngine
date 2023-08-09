#include "LevelManager.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "Player.h"
#include "Enemy.h"
#include "BulletManager.h"
#include "TouchableObject.h"
#include "MeshCollider.h"
#include "LandShape.h"


LevelManager::~LevelManager()
{
	objAndModels_.clear();
}

LevelManager& LevelManager::GetInstance()
{
	static LevelManager sInst;
	return sInst;
}

void LevelManager::StaticInitialize()
{
}

void LevelManager::LoadLevelData(int32_t fileIndex)
{
	//json読み込み
	JsonLevelLoader::Getinstance().LoadJsonFile(FILE_NAME_ + std::to_string(fileIndex));

	//セットで保存してるのをクリア
	objAndModels_.clear();
	CameraManager::GetInstance().Initialize();

	//レベルデータからカメラを取得
	for (auto& cameraData : JsonLevelLoader::Getinstance().levelData_->cameras)
	{
		//カメラ名からカメラを作成
		CameraManager::GetInstance().AddCamera(cameraData->fileName);
		//cameraのパラメータセット
		Camera* camera = CameraManager::GetInstance().GetCamera(cameraData->fileName);
		camera->SetEye(cameraData->camera->GetEye());
		camera->SetTarget(cameraData->camera->GetTarget());
		camera->SetUp(cameraData->camera->GetUp());

		//とりあえずカメラをセット
		CameraManager::GetInstance().SetUsingCamera(cameraData->fileName);

		//cameraManagerに登録されてるのでこっちでは登録しない
	}

	//レベルデータからオブジェクトを生成、配置
	for (auto& objData : JsonLevelLoader::Getinstance().levelData_->objects)
	{
		//
		LoadObj(*objData.get());
	}
}

Weapon* LevelManager::GetChildWeapon(LevelData::ObjectData& objData)
{
	if (objData.childData == nullptr || (objData.childData->fileName != "gun" &&
		objData.childData->fileName != "sword"))
	{
		return nullptr;
	}

	//ファイル名から登録済みモデルを検索
	IModel* model = ModelManager::GetInstance().LoadModel(objData.childData->fileName);

	//モデルを指定して3Dオブジェクトを生成
	std::unique_ptr <Weapon> newObj = {};

	//子が銃だった時
	if (objData.childData && objData.childData->fileName == "gun")
	{
		//インスタンス
		newObj = Gun::Create(std::move(objData.childData->worldMat));
	}
	//違う武器
	else
	{

	}

	Weapon* ans = newObj.get();

	//名前
	newObj->SetObjName(objData.childData->fileName);

	//判定系
	SetCollider(newObj.get(), objData, true);

	//正面ベクトル(objの角度によって回転,回転後のベクトルを基礎正面とする)
	newObj->CulcFrontVec();
	newObj->SetFrontVecTmp(newObj->GetFrontVec());

	//モデルセット
	newObj->SetModel(model);

	//セットで登録
	objAndModels_.insert(std::make_pair(std::move(newObj), model));

	if (ans != nullptr)
	{
		return ans;
	}

	return nullptr;
}

void LevelManager::CheckLandShapeObject(const LevelData::ObjectData& objData, bool& isLandShape)
{
	//地形オブジェクト
	if (objData.colliderData.attribute & COLLISION_ATTR_LANDSHAPE)
	{
		isLandShape = true;
	}
	else
	{
		isLandShape = false;
	}
}

void LevelManager::SetCollider(Object* obj, const LevelData::ObjectData& objData, bool isSettingCollider)
{
	//タイプがなければコライダー無しなので
	if (objData.colliderData.colliderType != CollisionShapeType::SHAPE_UNKNOWN)
	{
		CollisionShapeType type = objData.colliderData.colliderType;
		uint16_t attribute = objData.colliderData.attribute;

		if (isSettingCollider)
		{
			//球コライダー
			if (type == COLLISIONSHAPE_SPHERE)
			{
				obj->SetCollider(std::make_unique<SphereCollider>());
			}
			//メッシュコライダー
			else if (type == COLLISIONSHAPE_MESH)
			{
				obj->SetCollider(std::make_unique<MeshCollider>());
			}
		}
		//判定属性セット
		obj->GetCollider()->SetAttribute(attribute);
	}
}

void LevelManager::CreateObjectOrTouchableObject(std::unique_ptr<Object>& obj, LevelData::ObjectData& objData, bool isLandShape, IModel* model)
{
	//地形オブジェクトとして使うのなら
	if (isLandShape_)
	{
		obj = TouchableObject::Create(std::move(objData.worldMat), model);
	}
	//それ以外は普通のObject
	else
	{
		obj = std::make_unique<Object>();
		obj->SetWorldMat(std::move(objData.worldMat));
	}
}

bool LevelManager::GetGameOver()
{
	Player* p = nullptr;

	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetObjName() == "player")
		{
			return false;
		}
	}
	return true;
}

bool LevelManager::GetGameClear()
{
	int count = 0;

	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetObjName() == "enemy" && obj->GetIsAlive())
		{
			return false;
		}
	}
	return true;
}

void LevelManager::SetObjectIsDissolve(bool isDissolve, uint16_t attribute)
{
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetCollider()->GetAttribute() == attribute)
		{
			obj->SetisDissolve(isDissolve);
		}
	}
}

void LevelManager::SetIsDissolveT(float dissolveT, uint16_t attribute)
{
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetCollider() && obj->GetCollider()->GetAttribute() == attribute)
		{
			obj->SetDissolveT(dissolveT);
		}
	}
}

//-------------------------------------------------------------------------------------------
void LevelManager::LoadObj(LevelData::ObjectData& objData)
{
	//親がいたらスキップ(登録済みならworldMatはmove()されて無いので)
	if (/*objData.fileName == "gun" &&*/ !objData.worldMat)
	{
		return;
	}

	//ファイル名から登録済みモデルを検索
	IModel* model = ModelManager::GetInstance().LoadModel(objData.fileName);
	//モデルを指定して3Dオブジェクトを生成
	std::unique_ptr <Object> newObj = {};

	//地形オブジェクトとして使うか
	CheckLandShapeObject(objData, isLandShape_);

	//プレイヤーの場合
	if (objData.fileName == "player")
	{
		//playerはObjectクラスを継承してるのでポリモーフィズム
		newObj = Player::Create(std::move(objData.worldMat), GetChildWeapon(objData));
	}
	//敵の場合
	else if (objData.fileName == "enemy")
	{
		//enemyもObjectクラスを継承してるのでポリモーフィズム
		newObj = Enemy::Create(std::move(objData.worldMat), GetChildWeapon(objData));
	}
	//銃の場合(親がいる場合は既に登録してあるので通らない)
	else if (objData.fileName == "gun")
	{
		newObj = Gun::Create(std::move(objData.worldMat));
	}
	//地形オブジェクトなら
	else if (isLandShape_)
	{
		newObj = LandShape::Create(std::move(objData.worldMat), model);
	}
	//ただのオブジェクト(仮)
	else
	{
		newObj = std::make_unique<Object>();
	}

	//名前
	newObj->SetObjName(objData.fileName);

	//判定系
	SetCollider(newObj.get(), objData, !isLandShape_);

	//仮で正面ベクトル(objの角度によって回転,回転後のベクトルを基礎正面とする)
	if (objData.fileName == "player")
	{
		newObj->CulcFrontVec();
		newObj->SetFrontVecTmp(newObj->GetFrontVec());
	}

	//モデルセット
	newObj->SetModel(model);

	newObj->PlayAnimation(true);

	//セットで登録
	objAndModels_.insert(std::make_pair(std::move(newObj), model));
}

void LevelManager::Update()
{
	//.Objを更新
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->SetIsSilhouette(false);
	}

	//.Objを更新
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->Update();
	}

	//コライダーで生きてるフラグオフになったら消す
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (!obj->GetIsAlive())
		{
			objAndModels_.erase(it);
			it = objAndModels_.begin();
		}
	}
}

void LevelManager::Draw(Camera* camera)
{
	//描画
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->Draw();
	}
}

void LevelManager::DrawImGui()
{
	for (std::map<std::unique_ptr<Object>, IModel*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->DrawImGui();
	}
}
