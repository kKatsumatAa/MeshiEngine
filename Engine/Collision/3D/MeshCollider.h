#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"


/// <summary>
/// 繝｡繝・す繝･陦晉ｪ∝愛螳壹が繝悶ず繧ｧ繧ｯ繝・
/// </summary>
class MeshCollider :
	public BaseCollider
{
public:
	MeshCollider()
	{
		// 繝｡繝・す繝･蠖｢迥ｶ繧偵そ繝・ヨ
		shapeType_ = COLLISIONSHAPE_MESH;
	}

	/// <summary>
	/// 荳芽ｧ貞ｽ｢縺ｮ驟榊・繧呈ｧ狗ｯ峨☆繧・
	/// </summary>
	void ConstructTriangles(IModel* model);

	/// <summary>
	/// 譖ｴ譁ｰ
	/// </summary>
	void Update() override;

	/// <summary>
	/// 逅・→縺ｮ蠖薙◆繧雁愛螳・
	/// </summary>
	/// <param name="sphere">逅*/param>
	/// <param name="inter">莠､轤ｹ・亥・蜉帷畑・*/param>
	/// <returns>莠､蟾ｮ縺励※縺・ｋ縺句凄縺*/returns>
	bool CheckCollisionSphere(const Sphere& sphere, DirectX::XMVECTOR* inter = nullptr,
		DirectX::XMVECTOR* reject = nullptr);

	/// <summary>
	/// 繝ｬ繧､縺ｨ縺ｮ蠖薙◆繧雁愛螳・
	/// </summary>
	/// <param name="sphere">繝ｬ繧､</param>
	/// <param name="distance">霍晞屬・亥・蜉帷畑・*/param>
	/// <param name="inter">莠､轤ｹ・亥・蜉帷畑・*/param>
	/// <returns>莠､蟾ｮ縺励※縺・ｋ縺句凄縺*/returns>
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

private:
	std::vector<Triangle> triangles_;
	// 繝ｯ繝ｼ繝ｫ繝芽｡悟・縺ｮ騾・｡悟・
	DirectX::XMMATRIX invMatWorld_;
};

