/*
* @file ModelManager.h
* @brief モデルのデータ（fbx、obj）を管理
*/

#pragma once
#include"ModelObj.h"
#include"ModelFBX.h"

class ModelManager final
{
private:
	//モデル名とobj
	std::map<std::string, std::unique_ptr<ModelObj>>nameAndModels_;
	//モデル名とfbx
	std::map<std::string, std::unique_ptr<ModelFBX>>nameAndModelFBXs_;
	//同じ名前の別のインスタンス
	std::vector<std::unique_ptr<IModel>>sameNameOtherModels_;


private:
	//コンストラクタ
	ModelManager() { ; }
	//デストラクタ
	~ModelManager();

private:
	//.objのモデル読み込み
	ModelObj* LoadModelObj(const std::string& fileName, bool smoothing = false, bool sameNameOtherModel = false);
	//fbxのモデル読み込み
	ModelFBX* LoadModelFBX(const std::string& fileName, bool smoothing = false, bool sameNameOtherModel = false);

public:
	//コピーコンストラクタ禁止
	ModelManager& operator=(const ModelManager& inst) = delete;
	//コピーコンストラクタ禁止
	ModelManager(const ModelManager& inst) = delete;

	//インスタンス取得
	static ModelManager& GetInstance();

public:
	//モデルデータ後始末
	void ClearModels();

	//fbxモデルデータ後始末
	void ClearFbxModels();

	/// <summary>
	///fbxでもobjでも読み込み（fbxの場合は "xxx.fbx"とする）
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="smoothing">スムージング</param>
	/// <param name="otherSameNameModel">同じ名前の別のインスタンス</param>
	/// <returns>モデルのポインタ</returns>
	IModel* LoadModel(const std::string& fileName, bool smoothing = false, bool sameNameOtherModel = false);

public:
	//objモデルの追加
	void AddModelObj(std::unique_ptr<ModelObj> model, const std::string& name);
};
