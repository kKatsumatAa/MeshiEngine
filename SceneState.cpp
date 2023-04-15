#include "SceneState.h"


void SceneState::SetScene(SceneManager* sceneM)
{
	//stateではなくSceneクラスのインスタンス//
	this->sceneM = sceneM;
}