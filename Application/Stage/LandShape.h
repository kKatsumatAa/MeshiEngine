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

	Vec3 endColor = { 0.6f,1.6f,2.85f };

	//分割数
	const float TESS_FACTOR_MAX_ = 61.0f;


public:
	static std::unique_ptr<LandShape> Create(std::unique_ptr<WorldMat> worldMat, IModel* model);

	bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model);

	void Update()override;

	void DrawImGui(std::function<void()>imguiF = NULL)override;

	void DrawShadow()override;
	void Draw()override;
};