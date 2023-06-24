#pragma once
#include"Input.h"
#include"Util.h"


#define GAMEPAD_A                0
#define GAMEPAD_B                1
#define GAMEPAD_X                2
#define GAMEPAD_Y                3
#define GAMEPAD_LEFT_SHOULDER    4
#define GAMEPAD_RIGHT_SHOULDER   5
#define GAMEPAD_SELECT           6
#define GAMEPAD_START            7
#define GAMEPAD_LEFT_STICK       8
#define GAMEPAD_RIGHT_STICK      9

#define GAMEPAD_ARROW_UP          33
#define GAMEPAD_ARROW_DOWN        34
#define GAMEPAD_ARROW_LEFT        35
#define GAMEPAD_ARROW_RIGHT       36


class PadInput final
{
private:
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	HRESULT result_;
	DIJOYSTATE padData_;
	DIJOYSTATE oldPadData_;

	//�L�[�{�[�h�f�o�C�X�̐���
	ComPtr<IDirectInputDevice8> gamePad_ = nullptr;

	const float STICK_RANGE_ = 1000;
	const float STICK_R_RANGE_ = 65535 / 2.0f;

	const int32_t SEARCH_COUNT_MAX_ = 120;
	int32_t searchCount_ = 0;

	bool isActive_ = false;

private:
	//�ڑ�����Ă��邩���ׂ�(�f�o�C�X���������肷��)
	void PadConnectSearch();

	PadInput();

	~PadInput() { ; }

public:
	//�R�s�[�R���X�g���N�^�𖳌�
	PadInput(const PadInput& obj) = delete;
	//������Z�q��
	PadInput& operator=(const PadInput& obj) = delete;


	static PadInput& GetInstance();

	void CreateDevice();

	void Update();

	void Finalize();

public://�Q�b�^�[

	//�W���C�X�e�B�b�N���X�����Ă��邩
	bool GetJoyStickActive();
	//�W���C�X�e�B�b�N���ǂ̂��炢�|����Ă��邩(0.0~1.0f)
	Vec2 GetLeftStickTilt();
	Vec2 GetRightStickTilt();

	//�{�^����������Ă��邩
	bool GetPushButton(byte button);
	bool GetTriggerButton(byte button);
	bool GetReleaseTrigger(byte button);
};

