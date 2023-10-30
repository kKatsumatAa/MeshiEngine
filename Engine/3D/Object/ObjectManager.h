#pragma once
#include"IObject3D.h"


//今は3Dのみの管理
class ObjectManager final
{
private:
	//選択中のオブジェクト
	IObject3D* selectObj_ = nullptr;
	//オブジェクトの配列をグループごとに分けた
	std::map<std::string, std::vector<std::unique_ptr<IObject3D>>> objsGroups_;


private:
	ObjectManager() { ; }
	~ObjectManager() { ; }

public:
	ObjectManager& operator=(const ObjectManager& inst) = delete;
	ObjectManager(const ObjectManager& inst) = delete;

	static ObjectManager& GetInstance();

public:
	void Initialize();
	void Update();
	void DrawShadow();
	void Draw();
	void DrawImGui();
	void PostUpdate();

public:
	//デバッグ用アップデート(コライダーがないとクリックしても反応しない)
	void DebugUpdate();

private:
	//グループがあるか調べる
	bool FindObjGroup(const std::string& name);

	std::map<std::string, std::vector<std::unique_ptr<IObject3D>>>::iterator
		GetObjGroup(const std::string& name);

	//グループを追加する
	void AddGroup(const std::string& groupName, std::unique_ptr<IObject3D> inst);
	//グループにオブジェクト追加、グループがまだなければグループも配列に追加
	void AddObjAndGroup(std::unique_ptr<IObject3D> obj, const std::string& groupName);

	//判定属性と同じなら
	bool GetSameAttribute(IObject3D* obj, uint16_t attribute);
	//オブジェクトの名前とあっていたら
	bool GetSameObjName(IObject3D* obj, const std::string& name);

	//名前を付けてオブジェを追加、登録
	std::vector<IObject3D*> GetObjectInternal(const std::string& groupName,
		const std::function<bool(IObject3D* obj)>& isF);

	//同じ名前のオブジェクトの数を取得
	int32_t GetSameNameCount(const std::string& groupName, const std::string& name);

public:
	//名前を付けてオブジェを追加、登録
	void AddObject(const std::string& groupName, const std::string& name);
	//そのままインスタンスを登録()
	void AddObject(const std::string& groupName, std::unique_ptr<IObject3D>inst);
	//名前を指定してオブジェのポイントを配列で返す
	std::vector<IObject3D*> GetObjs(const std::string& groupName, const std::string& name);
	//判定属性を指定してオブジェのポイントを配列で返す
	std::vector<IObject3D*> GetObjs(const std::string& groupName, uint16_t attribute);
	//グループ全部
	std::vector<IObject3D*>GetObjs(const std::string& groupName);
	//全部(条件なく)
	std::vector<IObject3D*>GetObjs();

	//objクリア
	void ClearAllObj();
	//グループクリア
	void ClearGroup(const std::string& groupName);
};