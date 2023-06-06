#include "SceneFactory.h"


SceneState* SceneFactory::CreateScene(const std::string& sceneName)
{
    //次のシーンの入れ物
    SceneState* newScene = nullptr;

    if (sceneName == "TITLE")
    {
        newScene = new SceneTitle();
    }
    else if (sceneName == "GAME")
    {
        newScene = new SceneGame();
    }
    else if (sceneName == "LOAD")
    {
        newScene = new SceneLoad();
    }

    return newScene;
}
