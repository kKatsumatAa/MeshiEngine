#include "PadInput.h"

// デバイス発見時に実行される
BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	PadInput::GetInstance().CreateDevice();

	return DIENUM_STOP;
}

void PadInput::CreateDevice()
{
	//つながってたら呼ばれる関数なのでフラグはオンに
	isActive = true;

	//すでに中身があれば新たに生成しない
	if (gamePad)
	{
		return;
	}

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
	diprg.lMin = (long)-stickRange;
	diprg.lMax = (long)stickRange;
	result = gamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
	assert(SUCCEEDED(result));

	// Y軸の値の範囲設定
	diprg.diph.dwObj = DIJOFS_Y;
	result = gamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
	assert(SUCCEEDED(result));

}

void PadInput::PadConnectSearch()
{
	searchCount++;

	//舞フレーム接続確認すると重いので
	if (searchCount % searchCountMax == 0)
	{
		searchCount = 0;
		//いったんここでfalseにすることで接続されてないときはそのまま、されていたらtrue
		isActive = false;

		// デバイスの列挙（接続されてるかを確認）
		result = Input::GetInstance().GetDirectInput()->EnumDevices(
			DI8DEVTYPE_GAMEPAD,
			DeviceFindCallBack,//接続されていればこの関数が呼ばれる
			NULL,
			DIEDFL_ATTACHEDONLY//
		);
		assert(SUCCEEDED(result));

		//パッドが接続されてないかつ、前までされていたら削除
		if (gamePad && !isActive)
		{
			gamePad.Reset();
		}
	}
}

PadInput::PadInput()
{

	//Initialize();

}

PadInput::~PadInput()
{
}



PadInput& PadInput::GetInstance()
{
	static PadInput instance;
	return instance;
}

void PadInput::Update()
{
	//接続を確認
	PadConnectSearch();

	if (gamePad != NULL)
	{
		//前回のキー情報
		oldPadData = padData;

		//キーボード情報の取得開始
		gamePad->Acquire();
		//全キーの入力情報を取得
		result = gamePad->GetDeviceState(sizeof(padData), &padData);
		//assert(SUCCEEDED(result));
	}
}

bool PadInput::GetJoyStickActive()
{
	if (gamePad != NULL)
	{

		//ジョイスティック
		if (padData.lX < 0 || padData.lX > 0 || padData.lY < 0 || padData.lY > 0)
		{
			return true;
		}
	}
	return false;
}

Vec2 PadInput::GetLeftStickTilt()
{

	if (gamePad != NULL)
	{
		// スティックの方向判定
	// 無反応範囲
		long unresponsive_range = (long)(this->stickRange / 5.0f);

		if (padData.lX< -unresponsive_range || padData.lX > unresponsive_range
			|| padData.lY < -unresponsive_range || padData.lY > unresponsive_range)
		{

			float x;
			float y;

			x = (padData.lX) / (this->stickRange);
			y = (padData.lY) / (this->stickRange);

			return Vec2(x, y);
		}
	}

	return Vec2(0, 0);
}

Vec2 PadInput::GetRightStickTilt()
{
	if (gamePad != NULL)
	{
		// 無反応範囲
		long unresponsive_range = (long)(this->stickRRange / 5.0f);

		if (padData.lRx - stickRRange<-unresponsive_range || padData.lRx - stickRRange > unresponsive_range
			|| padData.lRy - stickRRange<-unresponsive_range || padData.lRy - stickRRange > unresponsive_range)
		{

			float x;
			float y;

			x = (padData.lRx - stickRRange) / (this->stickRRange);
			y = (padData.lRy - stickRRange) / (this->stickRRange);

			return Vec2(x, y);
		}
	}

	return Vec2(0, 0);
}

bool PadInput::GetPushButton(byte button)
{
	if (gamePad != NULL)
	{
		if (padData.rgbButtons[button])
		{
			return true;
		}
	}

	return false;
}

bool PadInput::GetTriggerButton(byte button)
{
	if (gamePad != NULL)
	{
		if (padData.rgbButtons[button] && !oldPadData.rgbButtons[button])
		{
			return true;
		}
	}

	return false;
}

bool PadInput::GetReleaseTrigger(byte button)
{
	if (gamePad != NULL)
	{
		if (!padData.rgbButtons[button] && oldPadData.rgbButtons[button])
		{
			return true;
		}
	}

	return false;
}
