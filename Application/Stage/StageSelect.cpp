#include "StageSelect.h"
#include "MouseInput.h"
#include "CursorUI.h"


StageSelect& StageSelect::GetInstance()
{
	static StageSelect sInst;
	return sInst;
}

void StageSelect::LoadStageNames()
{
	GetFileNames(BASE_DIRECTORY_, stageNames_);
}

//----------------------------------------------------------
void StageSelect::Initialize()
{
	//�摜
	debugTextHandle_ = TextureManager::LoadGraph("ascii.png");

	//���Z�b�g
	stageNames_.clear();
	isSelected_ = false;

	//�X�e�[�W���̘g�̑傫��
	nameWidthHeight_ = { CursorUI::cursorSize_.x * nameFrameWidthExtend_ ,CursorUI::cursorSize_.y };
	namesLeftUpPos_ = { CursorUI::cursorSize_.x * nameFrameWidthExtend_ ,CursorUI::cursorSize_.y * 4.0f };

	//�F
	selectBox_.SetColor({ 8.0f,0,0,1.0f });

	LoadStageNames();
}

void StageSelect::Update()
{
	//�X�N���[������


	//�N���b�N���ꂽ��
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
	{
		Vec2 cursorPos = CursorUI::GetInstance().GetCursorPos();

		//�����蔻��𒲂ׂ�
		int32_t count = 0;
		for (auto stageName : stageNames_)
		{
			//�������Ă���ΑI�񂾖��O,�ԍ���ۑ�
			if (CollisionBox(namesLeftUpPos_ + Vec2{0, nameWidthHeight_.y* (float)count},//����
				namesLeftUpPos_ + Vec2{nameWidthHeight_.x, nameWidthHeight_.y* (float)(count + 1)},//�E��
				cursorPos + Vec2{0, scrollValue_},
				cursorPos + Vec2{0, scrollValue_}))
			{
				selectStageName_ = stageName;
				selectStageIndex_ = count + 1;
				isSelected_ = true;

				break;
			}

			count++;
		}
	}
}

void StageSelect::Draw()
{
}

void StageSelect::DrawSprite()
{
	int32_t count = 0;

	for (auto stageName : stageNames_)
	{
		//�X�e�[�W���\��
		debugText_.Print(stageName.c_str(), namesLeftUpPos_.x, namesLeftUpPos_.y + nameWidthHeight_.y * count, 114514,
			TEXT_EXTEND_);

		Vec2 cursorPos = MouseInput::GetInstance().GetCurcorPos();
		//�J�[�\���������Ă���Ԃ��\��
		if (CollisionBox(namesLeftUpPos_ + Vec2{0, nameWidthHeight_.y* (float)count},//����
			namesLeftUpPos_ + Vec2{nameWidthHeight_.x, nameWidthHeight_.y* (float)(count + 1)},//�E��
			cursorPos + Vec2{0, scrollValue_},
			cursorPos + Vec2{0, scrollValue_}))
		{
			selectBox_.SetTrans({ namesLeftUpPos_ + Vec2{0, nameWidthHeight_.y} *(float)count,0 });
			selectBox_.SetScale({ nameWidthHeight_,1.0f });

			selectBox_.DrawBoxSprite(nullptr);
		}

		count++;
	}

	debugText_.DrawAll(debugTextHandle_, nullptr);
}
