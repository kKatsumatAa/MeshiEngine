#pragma once

#include<forward_list>

class BaseCollider;

//Objectが持っているコライダーをこのクラスの変数に登録して使う
class CollisionManager 
{
private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;
	//コライダーのリスト
	std::forward_list<BaseCollider*> colliders;



public://静的メンバ関数
	static CollisionManager* GetInstance();

public://関数
	/// <summary>
	/// コライダーの追加
	/// </summary>
	/// <param name="collider"></param>
	inline void AddCollider(BaseCollider* collider)
	{
		colliders.push_front(collider);
	}

	/// <summary>
	/// コライダーの削除
	/// </summary>
	/// <param name="collider"></param>
	inline void RemoveCollider(BaseCollider* collider)
	{
		colliders.remove(collider);
	}


	void CheckAllCollisions();

	void Initialize();
};

