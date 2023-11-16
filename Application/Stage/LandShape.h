#pragma once
#include"TouchableObject.h"
#include"LandShapePapierMache.h"


class LandShape : public TouchableObject
{
private:
	std::unique_ptr<LandShapePapierMache>landShapePapierMache_ = nullptr;

	float timer_ = 0;
	const float TIMER_MAX_ = 40.0f;
	float changeVel_ = 0;

	//分割数
	const float TESS_FACTOR_MAX_ = 61.0f;


public:
	static std::unique_ptr<LandShape> Create(std::unique_ptr<WorldMat> worldMat, IModel* model);

	bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model);

	void Update()override;

	void DrawShadow()override;
	void Draw()override;
};