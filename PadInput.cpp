#include "PadInput.h"

PadInput::PadInput()
{
	//// �f�o�C�X�̗�
	//result= Input::GetInstance().GetDirectInput()->EnumDevices(
	//	DI8DEVTYPE_JOYSTICK,
	//	DeviceFindCallBack,
	//	&parameter,
	//	DIEDFL_ATTACHEDONLY
	//);
	//assert(SUCCEEDED(result));

	//�p�b�h�f�o�C�X�̐���
	result = Input::GetInstance().GetDirectInput()->CreateDevice(GUID_Joystick, &gamePad, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = gamePad->SetDataFormat(&c_dfDIJoystick);
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃��Z�b�g
	result = gamePad->SetCooperativeLevel(
		WindowsApp::GetInstance().Gethwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//��ʂ���O�ɂ���Ƃ����͎�t�b�f�o�C�X�����̃A�v�������Ő�L���Ȃ��bWindows�L�[�𖳌�
	assert(SUCCEEDED(result));

	// �����[�h���Βl���[�h�Ƃ��Đݒ�
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;	// ��Βl���[�h�̎w��(DIPROPAXISMODE_REL�ɂ����瑊�Βl)

	// �����[�h��ύX
	result = gamePad->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	assert(SUCCEEDED(result));

	// X���̒l�͈̔͐ݒ�
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = -stickRange;
	diprg.lMax = stickRange;
	result = gamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
	assert(SUCCEEDED(result));

	// Y���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_Y;
	result = gamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
	assert(SUCCEEDED(result));
}

PadInput::~PadInput()
{
}

// �f�o�C�X�������Ɏ��s�����
BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	return DIENUM_CONTINUE;
}

PadInput& PadInput::GetInstance()
{
	static PadInput instance;
	return instance;
}

void PadInput::Update()
{
	//�O��̃L�[���
	oldPadData = padData;

	//�L�[�{�[�h���̎擾�J�n
	gamePad->Acquire();
	//�S�L�[�̓��͏����擾
	result = gamePad->GetDeviceState(sizeof(padData), &padData);

	//assert(SUCCEEDED(result));
}

bool PadInput::GetJoyStickActive()
{
	//�W���C�X�e�B�b�N
	if (padData.lX < 0 || padData.lX > 0 || padData.lY < 0 || padData.lY > 0)
	{
		return true;
	}

	return false;
}

Vec2 PadInput::GetLeftStickTilt()
{
	// �X�e�B�b�N�̕�������
// �������͈�
	long unresponsive_range = this->stickRange / 5.0f;

	if (padData.lX< -unresponsive_range || padData.lX > unresponsive_range
		|| padData.lY < -unresponsive_range || padData.lY > unresponsive_range)
	{

		float x;
		float y;

		x = (padData.lX) / (this->stickRange);
		y = (padData.lY) / (this->stickRange);

		return Vec2(x, y);
	}

	return Vec2(0, 0);
}

Vec2 PadInput::GetRightStickTilt()
{
	// �������͈�
	long unresponsive_range = this->stickRRange / 5.0f;

	if (padData.lRx - stickRRange<-unresponsive_range || padData.lRx - stickRRange > unresponsive_range
		|| padData.lRy - stickRRange<-unresponsive_range || padData.lRy - stickRRange > unresponsive_range)
	{

		float x;
		float y;

		x = (padData.lRx - stickRRange) / (this->stickRRange);
		y = (padData.lRy - stickRRange) / (this->stickRRange);

		return Vec2(x, y);
	}

	return Vec2(0, 0);
}

bool PadInput::GetPushButton(BYTE button)
{
	if (padData.rgbButtons[button])
	{
		return true;
	}

	return false;
}

bool PadInput::GetTriggerButton(BYTE button)
{
	if (padData.rgbButtons[button] && !oldPadData.rgbButtons[button])
	{
		return true;
	}

	return false;
}

bool PadInput::GetReleaseTrigger(BYTE button)
{
	if (!padData.rgbButtons[button] && oldPadData.rgbButtons[button])
	{
		return true;
	}

	return false;
}
