#include "PadInput.h"

PadInput::PadInput()
{
	//// デバイスの列挙
	//result= Input::GetInstance().GetDirectInput()->EnumDevices(
	//	DI8DEVTYPE_JOYSTICK,
	//	DeviceFindCallBack,
	//	&parameter,
	//	DIEDFL_ATTACHEDONLY
	//);
	//assert(SUCCEEDED(result));

	//パッドデバイスの生成
	result = Input::GetInstance().GetDirectInput()->CreateDevice(GUID_Joystick, &gamePad, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = gamePad->SetDataFormat(&c_dfDIJoystick);
	assert(SUCCEEDED(result));

	//排他制御レベルのリセット
	result = gamePad->SetCooperativeLevel(
		WindowsApp::GetInstance().Gethwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//画面が手前にあるとき入力受付｜デバイスをこのアプリだけで専有しない｜Windowsキーを無効
	assert(SUCCEEDED(result));

	// 軸モードを絶対値モードとして設定
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;	// 絶対値モードの指定(DIPROPAXISMODE_RELにしたら相対値)

	// 軸モードを変更
	result = gamePad->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	assert(SUCCEEDED(result));

	// X軸の値の範囲設定
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

	// Y軸の値の範囲設定
	diprg.diph.dwObj = DIJOFS_Y;
	result = gamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
	assert(SUCCEEDED(result));
}

PadInput::~PadInput()
{
}

// デバイス発見時に実行される
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
	//前回のキー情報
	oldPadData = padData;

	//キーボード情報の取得開始
	gamePad->Acquire();
	//全キーの入力情報を取得
	result = gamePad->GetDeviceState(sizeof(padData), &padData);

	//assert(SUCCEEDED(result));
}

bool PadInput::GetJoyStickActive()
{
	//ジョイスティック
	if (padData.lX < 0 || padData.lX > 0 || padData.lY < 0 || padData.lY > 0)
	{
		return true;
	}

	return false;
}

Vec2 PadInput::GetLeftStickTilt()
{
	// スティックの方向判定
// 無反応範囲
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
	// 無反応範囲
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
