#pragma once
#include"Object.h"

class Tutorial;


class TutorialState
{
protected:
	Tutorial* tutorial_;

	//�����B��������
	bool isAchievement_ = false;

	//���o�̃^�C�}�[
	int32_t endingTimer_ = 0;
	int32_t beginingTimer_ = 0;

	float t_ = 0;

	float scale_ = 0;
	const float MAX_SCALE_ = 1.5f;

	float alpha_ = 0.0f;

	//�摜
	uint64_t texHandle_ = NULL;
	Object texObj_;

	//���o�̎��Ԑ���
	const int32_t BEGINING_TIMER_MAX_ = 10;
	const int32_t ENDING_TIMER_MAX_ = 10;

	//�����B����
	float rate_ = 0;
	int32_t rateMax_ = 150;


public:
	virtual ~TutorialState() { ; }

public:
	void SetTutorial(Tutorial* tutorial) { tutorial_ = tutorial; }

	static std::unique_ptr<TutorialState>GetState(const std::string& name);

	virtual void CommonUpdate();
	virtual void CommonSpriteDraw();

	//�����B����̉��o
	virtual bool EndingUpdate(const int32_t TIMER_MAX);
	//�ŏ��̉��o����
	virtual bool BeginingUpdate(const int32_t TIMER_MAX);

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};


//�����Ȃ�
class TutorialStateNone : public TutorialState
{
private:

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//��{�������
class TutorialStateOperation : public TutorialState
{
private:


public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//�Q�[���X�s�[�h�̐���
class TutorialStateGameSpeed : public TutorialState
{
private:
	

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//�}�E�X�������
class TutorialStateMouse : public TutorialState
{
private:
	bool isPushedLeftClick = false;
	bool isPushedRightClick = false;

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};