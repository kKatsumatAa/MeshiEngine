#pragma once

#include<forward_list>
#include"CollisionPrimitive.h"
#include"RaycastHit.h"
#include"QueryCallback.h"

#include<d3d12.h>

class BaseCollider;

//Object縺梧戟縺｣縺ｦ縺・ｋ繧ｳ繝ｩ繧､繝繝ｼ繧偵％縺ｮ繧ｯ繝ｩ繧ｹ縺ｮ螟画焚縺ｫ逋ｻ骭ｲ縺励※菴ｿ縺・
class CollisionManager
{
private:
	CollisionManager() = default;
	~CollisionManager() = default;

private:
	//繧ｳ繝ｩ繧､繝繝ｼ縺ｮ繝ｪ繧ｹ繝・3D)
	std::forward_list<BaseCollider*> colliders3D_;
	//繧ｳ繝ｩ繧､繝繝ｼ縺ｮ繝ｪ繧ｹ繝・2D)
	std::forward_list<BaseCollider*> colliders2D_;



public://髱咏噪繝｡繝ｳ繝宣未謨ｰ
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;

	static CollisionManager* GetInstance();

private:
	void CheckAllCollision3D();
	void CheckAllCollision2D();

public://髢｢謨ｰ
	/// <summary>
	/// 繧ｳ繝ｩ繧､繝繝ｼ縺ｮ霑ｽ蜉
	/// </summary>
	/// <param name="collider"></param>
	void AddCollider(BaseCollider* collider);

	/// <summary>
	/// 繧ｳ繝ｩ繧､繝繝ｼ縺ｮ蜑企勁
	/// </summary>
	/// <param name="collider"></param>
	inline void RemoveCollider(BaseCollider* collider)
	{
		colliders3D_.remove(collider);
	}

	/// <summary>
	/// 繝ｬ繧､繧ｭ繝｣繧ｹ繝・
	/// </summary>
	/// <param name="ray">繝ｬ繧､</param>
	/// <param name="hitInfo">陦晉ｪ∵ュ蝣ｱ</param>
	/// <param name="maxDistance">譛螟ｧ霍晞屬</param>
	/// <returns>繝ｬ繧､縺御ｻｻ諢上・繧ｳ繝ｩ繧､繝繝ｼ縺ｨ莠､繧上ｋ蝣ｴ蜷医・true</returns>
	bool Raycast(const Ray& ray, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	/// 繝ｬ繧､繧ｭ繝｣繧ｹ繝亥愛螳壼ｱ樊ｧ謖・ｮ夂沿
	/// </summary>
	/// <param name="ray">繝ｬ繧､</param>
	/// <param name="attribute">蛻､螳壼ｱ樊ｧ</param>
	/// <param name="hitInfo">陦晉ｪ∵ュ蝣ｱ</param>
	/// <param name="maxDistance">譛螟ｧ霍晞屬</param>
	/// <returns></returns>
	bool Raycast(const Ray& ray, uint16_t attribute, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	///縲繝ｬ繧､繧ｭ繝｣繧ｹ繝医・萓ｿ蛻ｩ迚・
	/// </summary>
	/// <param name="pos">邨らせ</param>
	/// <param name="oldPos">蟋狗せ</param>
	/// <param name="attribute">蛻､螳壼ｱ樊ｧ</param>
	/// <param name="info"></param>
	/// <returns></returns>
	bool RaycastUtil(const Vec3& endPos, const Vec3& startPos, uint16_t attribute, RaycastHit* info);

	/// <summary>
	/// 逅・↓繧医ｋ陦晉ｪ∝・讀懃ｴ｢
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="callBack"></param>
	/// <param name="attribute"></param>
	void QuerySphere(const Sphere& sphere, QueryCallback* callBack, uint16_t attribute = (uint16_t)0xffff);

	void CheckAllCollisions();

	void Initialize();
};

