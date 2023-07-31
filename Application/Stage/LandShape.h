#pragma once
#include"TouchableObject.h"


class LandShape : public TouchableObject
{
private:

public:



private:


public:
	static std::unique_ptr<LandShape> Create(std::unique_ptr<WorldMat> worldMat, Model* model);

	bool Initialize(std::unique_ptr<WorldMat> worldMat, Model* model);

	void Update()override;

	void Draw()override;

};