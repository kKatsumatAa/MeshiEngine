#include "PadInput.h"

// �f�o�C�X�������Ɏ��s�����
BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	PadInput::GetInstance().CreateDevice();

	return DIENUM_STOP;
}

void PadInput::CreateDevice()
{
	//�Ȃ����Ă���Ă΂��֐��Ȃ̂Ńt���O�̓I����
	isActive_ = true;

	//���łɒ��g������ΐV���ɐ������Ȃ�
	if (gamePad_)
	{
		return;
	}

	//�p�b�h�f�o�C�X�̐���
	result_ = Input::GetInstance().GetDirectInput()->CreateDevice(GUID_Joystick, &gamePad_, NULL);
	assert(SUCCEEDED(result_));

	//���̓f�[�^�`���̃Z�b�g
	result_ = gamePad_->SetDataFormat(&c_dfDIJoystick);
	assert(SUCCEEDED(result_));

	//�r�����䃌�x���̃��Z�b�g
	result_ = gamePad_->SetCooperativeLevel(
		WindowsApp::GetInstance().Gethwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//��ʂ���O�ɂ���Ƃ����͎�t�b�f�o�C�X�����̃A�v�������Ő�L���Ȃ��bWindows�L�[�𖳌�
	assert(SUCCEEDED(result_));

	// �����[�h���Βl���[�h�Ƃ��Đݒ�
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;	// ��Βl���[�h�̎w��(DIPROPAXISMODE_REL�ɂ����瑊�Βl)

	// �����[�h��ύX
	result_ = gamePad_->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	assert(SUCCEEDED(result_));

	// X���̒l�͈̔͐ݒ�
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = (long)-STICK_RANGE_;
	diprg.lMax = (long)STICK_RANGE_;
	result_ = gamePad_->SetProperty(DIPROP_RANGE, &diprg.diph);
	assert(SUCCEEDED(result_));

	// Y���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_Y;
	result_ = gamePad_->SetProperty(DIPROP_RANGE, &diprg.diph);
	assert(SUCCEEDED(result_));

}

void PadInput::PadConnectSearch()
{
	//���t���[���ڑ��m�F����Əd���̂�
	if (searchCount_ % SEARCH_COUNT_MAX_ == 0)
	{
		searchCount_ = 0;
		//�������񂱂���false�ɂ��邱�ƂŐڑ�����ĂȂ��Ƃ��͂��̂܂܁A����Ă�����true
		isActive_ = false;

		// �f�o�C�X�̗񋓁i�ڑ�����Ă邩���m�F�j
		result_ = Input::GetInstance().GetDirectInput()->EnumDevices(
			DI8DEVTYPE_GAMEPAD,
			DeviceFindCallBack,//�ڑ�����Ă���΂��̊֐����Ă΂��
			NULL,
			DIEDFL_ATTACHEDONLY//
		);
		assert(SUCCEEDED(result_));

		//�p�b�h���ڑ�����ĂȂ����A�O�܂ł���Ă�����폜
		if (gamePad_ && !isActive_)
		{
			gamePad_->Unacquire();
			gamePad_->Release();
			gamePad_.Reset();
		}
	}

	searchCount_++;
}

PadInput::PadInput()
{
	result_ = {};
	//Initialize();

}

void PadInput::Finalize()
{
	if (!gamePad_) { return; }

	gamePad_->Unacquire();
	gamePad_->Release();
}



PadInput& PadInput::GetInstance()
{
	static PadInput sInstance;
	return sInstance;
}

void PadInput::Update()
{
	//�ڑ����m�F
	PadConnectSearch();

	if (gamePad_ != NULL)
	{
		//�O��̃L�[���
		oldPadData_ = padData_;

		//�L�[�{�[�h���̎擾�J�n
		gamePad_->Acquire();
		//�|�[�����O(���Ԋu�Ńf�o�C�X�̏��𓯊�)
		gamePad_->Poll();
		//�S�L�[�̓��͏����擾
		result_ = gamePad_->GetDeviceState(sizeof(padData_), &padData_);
		//assert(SUCCEEDED(result));
	}
}

bool PadInput::GetJoyStickActive()
{
	if (gamePad_ != NULL)
	{

		//�W���C�X�e�B�b�N
		if (padData_.lX < 0 || padData_.lX > 0 || padData_.lY < 0 || padData_.lY > 0)
		{
			return true;
		}
	}
	return false;
}

Vec2 PadInput::GetLeftStickTilt()
{

	if (gamePad_ != NULL)
	{
		// �X�e�B�b�N�̕�������
	// �������͈�
		long unresponsive_range = (long)(STICK_RANGE_ / 5.0f);

		if (padData_.lX< -unresponsive_range || padData_.lX > unresponsive_range
			|| padData_.lY < -unresponsive_range || padData_.lY > unresponsive_range)
		{

			float x;
			float y;

			x = (padData_.lX) / (STICK_RANGE_);
			y = (padData_.lY) / (STICK_RANGE_);

			return Vec2(x, y);
		}
	}

	return Vec2(0, 0);
}

Vec2 PadInput::GetRightStickTilt()
{
	if (gamePad_ != NULL)
	{
		// �������͈�
		long unresponsive_range = (long)(STICK_R_RANGE_ / 5.0f);

		if (padData_.lRx - STICK_R_RANGE_<-unresponsive_range || padData_.lRx - STICK_R_RANGE_ > unresponsive_range
			|| padData_.lRy - STICK_R_RANGE_<-unresponsive_range || padData_.lRy - STICK_R_RANGE_ > unresponsive_range)
		{

			float x;
			float y;

			x = (padData_.lRx - STICK_R_RANGE_) / (STICK_R_RANGE_);
			y = (padData_.lRy - STICK_R_RANGE_) / (STICK_R_RANGE_);

			return Vec2(x, y);
		}
	}

	return Vec2(0, 0);
}

bool PadInput::GetPushButton(byte button)
{
	if (gamePad_ != NULL)
	{
		if (padData_.rgbButtons[button])
		{
			return true;
		}
	}

	return false;
}

bool PadInput::GetTriggerButton(byte button)
{
	if (gamePad_ != NULL)
	{
		if (padData_.rgbButtons[button] && !oldPadData_.rgbButtons[button])
		{
			return true;
		}
	}

	return false;
}

bool PadInput::GetReleaseTrigger(byte button)
{
	if (gamePad_ != NULL)
	{
		if (!padData_.rgbButtons[button] && oldPadData_.rgbButtons[button])
		{
			return true;
		}
	}

	return false;
}
