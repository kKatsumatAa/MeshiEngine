#pragma once
#include"Object.h"


class ObjectManager final
{
private:
	//選択中のオブジェクト
	Object* selectObj_ = nullptr;
	//オブジェクトの配列をグループごとに分けた
	std::map<std::string, std::vector<std::unique_ptr<Object>>> objsGroups_;


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
	void Draw();
	void DrawImGui();
	void PostUpdate();

public:
	//デバッグ用アップデート(コライダーがないとクリックしても反応しない)
	void DebugUpdate();

private:
	//グループがあるか調べる
	bool FindObjGroup(const std::string& name);

	std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator
		GetObjGroup(const std::string& name);

	//グループを追加する
	void AddGroup(const std::string& groupName, std::unique_ptr<Object> inst);
	//グループにオブジェクト追加、グループがまだなければグループも配列に追加
	void AddObjAndGroup(std::unique_ptr<Object> obj, const std::string& groupName);

	//判定属性と同じなら
	bool GetSameAttribute(Object* obj, uint16_t attribute);
	//オブジェクトの名前とあっていたら
	bool GetSameObjName(Object* obj, const std::string& name);



	//名前を付けてオブジェを追加、登録
	std::vector<Object*> GetObjectInternal(const std::string& groupName,
		const std::function<bool(Object* obj)>& isF);

public:
	//名前を付けてオブジェを追加、登録
	void AddObject(const std::string& groupName, const std::string& name);
	//そのままインスタンスを登録()
	void AddObject(const std::string& groupName, std::unique_ptr<Object>inst);
	//名前を指定してオブジェのポイントを配列で返す
	std::vector<Object*> GetObjs(const std::string& groupName, const std::string& name);
	//判定属性を指定してオブジェのポイントを配列で返す
	std::vector<Object*> GetObjs(const std::string& groupName, uint16_t attribute);
	//グループ全部
	std::vector<Object*>GetObjs(const std::string& groupName);
	//全部(条件なく)
	std::vector<Object*>GetObjs();

	//objクリア
	void ClearAllObj();
	//グループクリア
	void ClearGroup(const std::string& groupName);
};