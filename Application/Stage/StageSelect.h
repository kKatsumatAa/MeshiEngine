#pragma once
#include"Util.h"
#include"DebugText.h"


class StageSelect final
{
private:
	const std::string BASE_DIRECTORY_ = "Resources/Level/levelData/";

private:
	const float nameFrameWidthExtend_ = 20.0f;
	//�I���ς݂�
	bool isSelected_ = false;
	//�I�������X�e�[�W��
	std::string selectStageName_;
	int32_t selectStageIndex_ = 0;
	//�Z���N�g��̕�
	Vec2 nameWidthHeight_;
	//�Z���N�g�܂Ƃ܂�̍���ʒu
	 Vec2 namesLeftUpPos_;
	//�X�e�[�W���ǂꂾ�����邩�ǂݍ��݁A���߂Ă���
	std::vector<std::string> stageNames_;
	//�ǂꂾ���X�N���[�����ꂽ��
	float scrollValue_ = 0;
	//
	Object selectBox_;

	//�f�o�b�O�e�L�X�g
	DebugText debugText_;
	uint64_t debugTextHandle_;

private:
	StageSelect() { ; }
	~StageSelect() { ; }

public:
	StageSelect& operator=(const StageSelect& inst) = delete;
	StageSelect(const StageSelect& inst) = delete;

	static StageSelect& GetInstance();

private:
	//�X�e�[�W�̖��O�ǂݍ���
	void LoadStageNames();

public:
	void Initialize(
	);
	void Update();
	void Draw();
	void DrawSprite();

public:
	bool GetIsSelected() { return isSelected_; }
	const std::string& GetSelectStageName() { return selectStageName_; }
	int32_t GetSelectStageIndex() { return selectStageIndex_; }

};