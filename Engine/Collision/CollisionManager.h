/*
* @file CollisionManager.h
* @brief コライダーを管理（追加等）
*/

#pragma once
#include<forward_list>
#include"CollisionPrimitive.h"
#include"RaycastHit.h"
#include"QueryCallback.h"

#include<d3d12.h>

class BaseCollider;

//Objectが持っているコライダーをこのクラスの変数に登録して使う
class CollisionManager
{
private:
	//コライダーのリスト(3D)
	std::forward_list<BaseCollider*> colliders3D_;
	//コライダーのリスト(2D)
	std::forward_list<BaseCollider*> colliders2D_;


public://静的メンバ関数
	//コピーコンストラクタ禁止
	CollisionManager(const CollisionManager&) = delete;
	//コピーコンストラクタ禁止
	CollisionManager& operator=(const CollisionManager&) = delete;

	//インスタンス取得
	static CollisionManager* GetInstance();

private:
	//コンストラクタ
	CollisionManager() = default;
	//デストラクタ
	~CollisionManager() = default;

private:
	//3d全てのコライダー同士の判定調べる
	void CheckAllCollision3D();
	//2d全てのコライダー同士の判定調べる
	void CheckAllCollision2D();

public://関数
	//初期化
	void Initialize();

	//全てのコライダー同士の判定を調べる
	void CheckAllCollisions();

public:
	/// <summary>
	/// コライダーの追加
	/// </summary>
	/// <param name="collider"></param>
	void AddCollider(BaseCollider* collider);

	/// <summary>
	/// コライダーの削除
	/// </summary>
	/// <param name="collider"></param>
	inline void RemoveCollider(BaseCollider* collider)
	{
		colliders3D_.remove(collider);
	}

	/// <summary>
	/// レイキャスト
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="hitInfo">衝突情報</param>
	/// <param name="maxDistance">最大距離</param>
	/// <returns>レイが任意のコライダーと交わる場合はtrue</returns>
	bool Raycast(const Ray& ray, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	/// レイキャスト判定属性指定版
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="attribute">判定属性</param>
	/// <param name="hitInfo">衝突情報</param>
	/// <param name="maxDistance">最大距離</param>
	/// <returns></returns>
	bool Raycast(const Ray& ray, uint16_t attribute, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	///　レイキャストの便利版
	/// </summary>
	/// <param name="pos">終点</param>
	/// <param name="oldPos">始点</param>
	/// <param name="attribute">判定属性</param>
	/// <param name="info"></param>
	/// <returns></returns>
	bool RaycastUtil(const Vec3& endPos, const Vec3& startPos, uint16_t attribute, RaycastHit* info);

	/// <summary>
	/// 球による衝突全検索
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="callBack"></param>
	/// <param name="attribute"></param>
	void QuerySphere(const Sphere& sphere, QueryCallback* callBack, uint16_t attribute = (uint16_t)0xffff);
};

