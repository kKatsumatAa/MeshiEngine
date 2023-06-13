#pragma once

#include "SceneState.h"
#include <string>
#include <memory>


/// <summary>
/// �V�[���H��i�T�O�E���ۃN���X�j
/// </summary>
class AbstractSceneFactory
{
public:
	//���z�f�X�g���N�^
	virtual ~AbstractSceneFactory() = default;

	//�V�[������
	virtual std::unique_ptr<SceneState> CreateScene(const std::string& sceneName) = 0;
};

