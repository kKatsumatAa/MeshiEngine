#include "SceneState.h"


void SceneState::SetScene(SceneManager* sceneM)
{
	//stateではなくSceneクラスのインスタンス//
	sceneM_ = sceneM;
}