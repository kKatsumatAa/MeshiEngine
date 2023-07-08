#pragma once
#include "Object.h"


class TouchableObject :
	public Object
{
public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<TouchableObject> Create(std::unique_ptr<WorldMat>worldMat,
		Model* model);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize(std::unique_ptr<WorldMat>worldMat, Model* model);
};

