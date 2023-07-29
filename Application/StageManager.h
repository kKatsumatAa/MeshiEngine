#pragma once
#include"LevelManager.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"
#include "ClearEffect.h"
#include "ClearEffectState.h"
#include "TutorialState.h"
#include "Tutorial.h"
#include "ParticleManager.h"


class StageManager final
{
private:
	int32_t stageNum_ = 1;

public:


private:
	StageManager() { ; }
	~StageManager() { ; }

public:
	StageManager& operator=(const StageManager& inst) = delete;
	StageManager(const StageManager& inst) = delete;

	static StageManager& GetInstance();

private:
	void Initialize();

public:
	//�X�e�[�W�ǂݍ���
	void LoadStage(int32_t stageIndex);

	//�X�e�[�W�ԍ�
	void SetStageNum(int32_t stageNum) { stageNum_ = stageNum; }
	int32_t GetStageNum() { return stageNum_; }
	
public:
	void Update();
	void Draw();
	void DrawImGui();
	void DrawSprite();
};