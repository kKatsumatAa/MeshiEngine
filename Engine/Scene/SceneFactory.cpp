#include "SceneFactory.h"


std::unique_ptr<SceneState> SceneFactory::CreateScene(const std::string& sceneName)
{
    //Ÿ‚ÌƒV[ƒ“‚Ì“ü‚ê•¨
    std::unique_ptr<SceneState> newScene;

    if (sceneName == "TITLE")
    {
        newScene = std::make_unique<SceneTitle>();
    }
    else if (sceneName == "STAGESELECT")
    {
        newScene = std::make_unique<SceneStageSelect>();
    }
    else if (sceneName == "GAME")
    {
        newScene = std::make_unique<SceneGame>();
    }
    else if (sceneName == "CLEAR")
    {
        newScene = std::make_unique<SceneGame>();
    }
    

    return std::move(newScene);
}
