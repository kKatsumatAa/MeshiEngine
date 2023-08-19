#pragma once
#include "AbstractSceneFactory.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneStageSelect.h"


//ゲーム別シーン工場
class SceneFactory :
    public AbstractSceneFactory
{
public:
    /// <summary>
    /// シーン生成
    /// </summary>
    /// <param name="sceneName">シーン名</param>
    /// <returns>生成したシーン</returns>
    std::unique_ptr<SceneState> CreateScene(const std::string& sceneName) override;
};

