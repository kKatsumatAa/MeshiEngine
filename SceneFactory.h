#pragma once
#include "AbstractSceneFactory.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneLoad.h"


//�Q�[���ʃV�[���H��
class SceneFactory :
    public AbstractSceneFactory
{
public:
    /// <summary>
    /// �V�[������
    /// </summary>
    /// <param name="sceneName">�V�[����</param>
    /// <returns>���������V�[��</returns>
    SceneState* CreateScene(const std::string& sceneName) override;
};

