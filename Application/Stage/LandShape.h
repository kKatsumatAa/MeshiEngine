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
	//インスタンス生成
	static std::unique_ptr<LandShape> Create(std::unique_ptr<WorldMat> worldMat, IModel* model);

	//初期化
	bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model);
	//更新
	void Update()override;
	//ImGuiの描画
	void DrawImGui(std::function<void()>imguiF = NULL)override;
	//影用の深度描画
	void DrawShadow()override;
	//描画
	void Draw()override;
};