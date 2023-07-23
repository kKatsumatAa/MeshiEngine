#include "SceneState.h"


void SceneState::SetSceneManager(SceneManager* sceneM)
{
	//stateではなくSceneクラスのインスタンス//
	sceneM_ = sceneM;
}