#pragma once
#include"TouchableObject.h"


class LandShape : public TouchableObject
{
public:
	static std::unique_ptr<LandShape> Create(std::unique_ptr<WorldMat> worldMat, IModel* model);

	bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model);

	void Update()override;

	void DrawShadow()override;
	void Draw()override;

	void OnCollision(const CollisionInfo& info) override;
};