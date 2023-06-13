#pragma once

#include "SceneState.h"
#include <string>
#include <memory>


/// <summary>
/// シーン工場（概念・抽象クラス）
/// </summary>
class AbstractSceneFactory
{
public:
	//仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;

	//シーン生成
	virtual std::unique_ptr<SceneState> CreateScene(const std::string& sceneName) = 0;
};

