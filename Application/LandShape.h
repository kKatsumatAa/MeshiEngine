#pragma once
#include"TouchableObject.h"


class LandShape : public TouchableObject
{
private:
	//オブジェクト生成時のタイマー
	float generateTimer_ = 0;
	const float GENERATE_TIMER_MAX_ = 45.0f;

public:



private:


public:
	static std::unique_ptr<LandShape> Create(std::unique_ptr<WorldMat> worldMat, Model* model);

	bool Initialize(std::unique_ptr<WorldMat> worldMat, Model* model);

	void Update()override;

	void Draw()override;

};