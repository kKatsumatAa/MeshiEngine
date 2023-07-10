#include "LevelManager.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "Player.h"
#include "Enemy.h"
#include "BulletManager.h"
#include "TouchableObject.h"


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

void LevelManager::LoadLevelData(std::string fileName)
{
	//json読み込み
	JsonLevelLoader::Getinstance().LoadJsonFile(fileName);

	//initializeの度,毎回やっちゃうとおかしくなる
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

Weapon* LevelManager::GetChildWeapon(const LevelData::ObjectData& objData)
{
	if (objData.childData == nullptr || (objData.childData->fileName != "gun" &&
		objData.childData->fileName != "sword"))
	{
		return nullptr;
	}

	//ファイル名から登録済みモデルを検索
	Model* model = ModelManager::GetInstance().LoadModel(objData.childData->fileName);

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

	//正面ベクトル(objの角度によって回転,回転後のベクトルを基礎正面とする)
	newObj->CulcFrontVec();
	newObj->SetFrontVecTmp(newObj->GetFrontVec());

	//セットで登録
	objAndModels_.insert(std::make_pair(std::move(newObj), model));

	if (ans != nullptr)
	{
		return ans;
	}

	return nullptr;
}

bool LevelManager::GetGameOver()
{
	Player* p = nullptr;

	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
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

	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		if (obj->GetObjName() == "enemy" && obj->GetIsAlive())
		{
			return false;
		}
	}
	return true;
}

void LevelManager::LoadObj(LevelData::ObjectData& objData)
{
	//武器で親がいたらスキップ(登録済みならworldMatはmove()されてないので)
	if (objData.fileName == "gun" && !objData.worldMat)
	{
		return;
	}


	//ファイル名から登録済みモデルを検索
	Model* model = ModelManager::GetInstance().LoadModel(objData.fileName);
	//モデルを指定して3Dオブジェクトを生成
	std::unique_ptr <Object> newObj = {};


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
		newObj->SetObjName("enemy");
	}
	//銃の場合(親がいる場合は既に登録してあるので飛ばされる)
	else if (objData.fileName == "gun")
	{
		newObj = std::make_unique<Object>();
		newObj->SetObjName("gun");
		newObj->SetWorldMat(std::move(objData.worldMat));
	}
	//地面だったら
	else if (objData.fileName == "stage")
	{
		newObj = TouchableObject::Create(std::move(objData.worldMat), model);
		newObj->SetObjName("stage");
		newObj->GetCollider()->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	}
	//特に当てはまらないとき
	else
	{
		newObj = std::make_unique<Object>();
		newObj->SetWorldMat(std::move(objData.worldMat));
	}

	//正面ベクトル(objの角度によって回転,回転後のベクトルを基礎正面とする)
	newObj->CulcFrontVec();
	newObj->SetFrontVecTmp(newObj->GetFrontVec());

	//セットで登録
	objAndModels_.insert(std::make_pair(std::move(newObj), model));
}

void LevelManager::Update()
{
	//.Objを更新
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->SetIsSilhouette(false);
	}

	//.Objを更新
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->Update();
	}

	//コライダーで生きてるフラグオフになったら消す
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
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
	//.Objを表示
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();
		Model* model = it->second;

		obj->DrawModel(model, camera);
		obj->Draw();
	}
}
