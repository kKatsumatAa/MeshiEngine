#include "LevelManager.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "Player.h"
#include "Enemy.h"

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

void LevelManager::LoadLevelData()
{
	//initializeの度,毎回やっちゃうとおかしくなる
	objAndModels_.clear();

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
		LoadCharacter(*objData.get());
	}
}

void LevelManager::LoadCharacter(const LevelData::ObjectData& objData)
{
	//ファイル名から登録済みモデルを検索
	Model* model = ModelManager::GetInstance().LoadModel(objData.fileName);
	//モデルを指定して3Dオブジェクトを生成
	std::unique_ptr <Object> newObj = {};

	//プレイヤーの場合
	if (objData.fileName == "player")
	{
		//playerはObjectクラスを継承してるのでポリモーフィズム
		newObj = Player::Create();
	}
	//敵の場合
	else if (objData.fileName == "enemy")
	{
		//enemyもObjectクラスを継承してるのでポリモーフィズム
		newObj = Enemy::Create();
	}
	//特に当てはまらないとき
	else
	{
		newObj = std::make_unique<Object>();
	}



	//worldmat
	newObj->SetWorldMat_(*objData.worldMat.get());

	//正面ベクトル(objの角度によって回転,回転後のベクトルを基礎正面とする)
	newObj->CulcFrontVec();
	newObj->SetFrontVecTmp(newObj->GetFrontVec());

	//セットで登録
	objAndModels_.insert(std::make_pair(std::move(newObj), model));
}

void LevelManager::Update()
{
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

	//.Objを更新
	for (std::map<std::unique_ptr<Object>, Model*>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();

		obj->Update();
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
	}
}
