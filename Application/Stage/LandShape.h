/*
* @file LandShape.h
* @brief 地形オブジェクト
*/

#pragma once
#include"TouchableObject.h"
#include"LandShapePapierMache.h"


class LandShape : public TouchableObject
{
private:
	std::unique_ptr<LandShapePapierMache>landShapePapierMache_ = nullptr;

	//分割数
	const float TESS_FACTOR_MAX_ = 71.0f;


public:
	static std::unique_ptr<LandShape> Create(std::unique_ptr<WorldMat> worldMat, IModel* model);

	bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model);

	void Update()override;

	void DrawImGui(std::function<void()>imguiF = NULL)override;

	void DrawShadow()override;
	void Draw()override;
};