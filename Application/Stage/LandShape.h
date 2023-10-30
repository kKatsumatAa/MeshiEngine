#pragma once
#include"TouchableObject.h"


class LandShape : public TouchableObject
{
private:

public:



private:


public:
	static std::unique_ptr<LandShape> Create(std::unique_ptr<WorldMat> worldMat, IModel* model);

	bool Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model);

	void Update()override;

	void DrawShadow()override;
	void Draw()override;
};