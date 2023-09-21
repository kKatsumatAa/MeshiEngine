#include "MouseInput.h"
#include "ImGuiManager.h"

// �f�o�C�X�������Ɏ��s�����
BOOL CALLBACK DeviceFindCallBackM(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	MouseInput::GetInstance().CreateDevice();

	return DIENUM_STOP;
}

void MouseInput::CreateDevice(bool isExclusive)
{
	//�Ȃ����Ă���Ă΂��֐��Ȃ̂Ńt���O�̓I����
	isActive_ = true;

	//���łɒ��g������ΐV���ɐ������Ȃ�
	if (mouse_)
	{
		return;
	}

	//�f�o�C�X�쐬
	HRESULT result = Input::GetInstance().GetDirectInput()->CreateDevice(
		GUID_SysMouse,
		&mouse_,
		NULL
	);
	assert(SUCCEEDED(result));

	//�f�o�C�X�t�H�[�}�b�g�̐ݒ�
	mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));

	//�ق��̃A�v���ł��}�E�X�擾�����邩
	int32_t exclusive = DISCL_NONEXCLUSIVE;
	if (isExclusive) {
		exclusive = DISCL_EXCLUSIVE;
	}

	//�������[�h�̐ݒ�(�E�B���h�E���A�N�e�B�u���̂ݎ擾�A�ʃA�v���P�[�V�����ł��擾�ł���)
	if (FAILED(mouse_->SetCooperativeLevel(WindowsApp::GetInstance().Gethwnd(),
		DISCL_FOREGROUND | exclusive | DISCL_NOWINKEY)
	))
	{
		assert(false);
	}

	//

}

void MouseInput::MouseConnectSearch()
{
	//���t���[���ڑ��m�F����Əd���̂�
	if (searchCount_ % SEARCH_COUNT_MAX_ == 0)
	{
		searchCount_ = 0;
		//�������񂱂���false�ɂ��邱�ƂŐڑ�����ĂȂ��Ƃ��͂��̂܂܁A����Ă�����true
		isActive_ = false;

		// �f�o�C�X�̗񋓁i�ڑ�����Ă邩���m�F�j
		result_ = Input::GetInstance().GetDirectInput()->EnumDevices(
			DI8DEVTYPE_MOUSE,
			DeviceFindCallBackM,//�ڑ�����Ă���΂��̊֐����Ă΂��
			NULL,
			DIEDFL_ATTACHEDONLY//
		);
		assert(SUCCEEDED(result_));

		//�p�b�h���ڑ�����ĂȂ����A�O�܂ł���Ă�����폜
		if (mouse_ && !isActive_)
		{
			mouse_->Unacquire();
			mouse_->Release();
			mouse_.Reset();
		}
	}

	searchCount_++;
}

MouseInput::MouseInput()
{
	result_ = {};
	//Initialize();

}

void MouseInput::Finalize()
{
	if (!mouse_) { return; }

	mouse_->Unacquire();
	mouse_->Release();
}



MouseInput& MouseInput::GetInstance()
{
	static MouseInput sInstance;
	return sInstance;
}

void MouseInput::Update()
{
	//�ڑ����m�F
	MouseConnectSearch();

	if (mouse_ != NULL)
	{
		//�O��̏��
		oldMouseData_ = mouseData_;

		//���̎擾�J�n
		mouse_->Acquire();
		//�|�[�����O(���Ԋu�Ńf�o�C�X�̏��𓯊�)
		mouse_->Poll();
		//���͏����擾
		mouse_->GetDeviceState(sizeof(mouseData_), &mouseData_);

		//�J�[�\���̈ʒu�擾
		POINT p;
		GetCursorPos(&p);
		//�X�N���[�����W���N���C�A���g���W�ɕς���
		ScreenToClient(WindowsApp::GetInstance().Gethwnd(), &p);

		cursorPos_.x = (float)p.x;
		cursorPos_.y = (float)p.y;
	}
}

bool MouseInput::GetMouseActive()
{
	if (mouse_ != NULL)
	{

		//�W���C�X�e�B�b�N
		if (mouseData_.lX < 0 || mouseData_.lX > 0 || mouseData_.lY < 0 || mouseData_.lY > 0)
		{
			return true;
		}
	}
	return false;
}

bool MouseInput::GetClick(byte click)
{
	if (mouse_ == NULL) { return false; }

	if (ImGui::GetIO().WantCaptureMouse == false && mouseData_.rgbButtons[click])
	{
		return true;
	}

	return false;
}

bool MouseInput::GetTriggerClick(byte click)
{
	if (mouse_ == NULL) { return false; }

	if (ImGui::GetIO().WantCaptureMouse == false && 
		mouseData_.rgbButtons[click] && !oldMouseData_.rgbButtons[click])
	{
		return true;
	}

	return false;
}

bool MouseInput::GetTriggerReleaseClick(byte click)
{
	if (mouse_ == NULL) { return false; }

	if (ImGui::GetIO().WantCaptureMouse == false && 
		!mouseData_.rgbButtons[click] && oldMouseData_.rgbButtons[click])
	{
		return true;
	}

	return false;
}

const Vec2& MouseInput::GetCurcorPos()
{
	return cursorPos_;
}

Vec2 MouseInput::GetCursorVelocity()
{
	return Vec2((float)mouseData_.lX, (float)mouseData_.lY);
}

long MouseInput::GetWheelAmountOfRot()
{
	return mouseData_.lZ;
}
