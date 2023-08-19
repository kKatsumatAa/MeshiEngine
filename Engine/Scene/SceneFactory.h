#pragma once
#include "AbstractSceneFactory.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneStageSelect.h"


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
    std::unique_ptr<SceneState> CreateScene(const std::string& sceneName) override;
};

