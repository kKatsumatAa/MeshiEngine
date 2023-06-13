#include "SceneFactory.h"


std::unique_ptr<SceneState> SceneFactory::CreateScene(const std::string& sceneName)
{
    //Ÿ‚ÌƒV[ƒ“‚Ì“ü‚ê•¨
    std::unique_ptr<SceneState> newScene;

    if (sceneName == "TITLE")
    {
        newScene = std::make_unique<SceneTitle>();
    }
    else if (sceneName == "GAME")
    {
        newScene = std::make_unique<SceneGame>();
    }
    else if (sceneName == "LOAD")
    {
        newScene = std::make_unique<SceneLoad>();
    }

    return std::move(newScene);
}
