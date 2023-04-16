#pragma once

#include "SceneState.h"
#include <string>


/// <summary>
/// �V�[���H��i�T�O�E���ۃN���X�j
/// </summary>
class AbstractSceneFactory
{
public:
	//���z�f�X�g���N�^
	virtual ~AbstractSceneFactory() = default;

	//�V�[������
	virtual SceneState* CreateScene(const std::string& sceneName) = 0;
};

