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

public:


private:
	ModelManager() { ; }
	~ModelManager();

private:

	ModelObj* LoadModelObj(const std::string& fileName, bool smoothing = false, bool sameNameOtherModel = false);
	ModelFBX* LoadModelFBX(const std::string& fileName, bool smoothing = false, bool sameNameOtherModel = false);

public:
	ModelManager& operator=(const ModelManager& inst) = delete;
	ModelManager(const ModelManager& inst) = delete;

	static ModelManager& GetInstance();

	//後始末
	void Finalize();

	/// <summary>
	///fbxでもobjでも読み込み（fbxの場合は "xxx.fbx"とする）
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="smoothing">スムージング</param>
	/// <param name="otherSameNameModel">同じ名前の別のインスタンス</param>
	/// <returns>モデルのポインタ</returns>
	IModel* LoadModel(const std::string& fileName, bool smoothing = false, bool sameNameOtherModel = false);
};