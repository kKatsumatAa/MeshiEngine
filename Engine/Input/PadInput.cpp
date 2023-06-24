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
	isActive_ = true;

	//すでに中身があれば新たに生成しない
	if (gamePad_)
	{
		return;
	}

	//パッドデバイスの生成
	result_ = Input::GetInstance().GetDirectInput()->CreateDevice(GUID_Joystick, &gamePad_, NULL);
	assert(SUCCEEDED(result_));

	//入力データ形式のセット
	result_ = gamePad_->SetDataFormat(&c_dfDIJoystick);
	assert(SUCCEEDED(result_));

	//排他制御レベルのリセット
	result_ = gamePad_->SetCooperativeLevel(
		WindowsApp::GetInstance().Gethwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//画面が手前にあるとき入力受付｜デバイスをこのアプリだけで専有しない｜Windowsキーを無効
	assert(SUCCEEDED(result_));

	// 軸モードを絶対値モードとして設定
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;	// 絶対値モードの指定(DIPROPAXISMODE_RELにしたら相対値)

	// 軸モードを変更
	result_ = gamePad_->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	assert(SUCCEEDED(result_));

	// X軸の値の範囲設定
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

	// Y軸の値の範囲設定
	diprg.diph.dwObj = DIJOFS_Y;
	result_ = gamePad_->SetProperty(DIPROP_RANGE, &diprg.diph);
	assert(SUCCEEDED(result_));

}

void PadInput::PadConnectSearch()
{
	//毎フレーム接続確認すると重いので
	if (searchCount_ % SEARCH_COUNT_MAX_ == 0)
	{
		searchCount_ = 0;
		//いったんここでfalseにすることで接続されてないときはそのまま、されていたらtrue
		isActive_ = false;

		// デバイスの列挙（接続されてるかを確認）
		result_ = Input::GetInstance().GetDirectInput()->EnumDevices(
			DI8DEVTYPE_GAMEPAD,
			DeviceFindCallBack,//接続されていればこの関数が呼ばれる
			NULL,
			DIEDFL_ATTACHEDONLY//
		);
		assert(SUCCEEDED(result_));

		//パッドが接続されてないかつ、前までされていたら削除
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
	//接続を確認
	PadConnectSearch();

	if (gamePad_ != NULL)
	{
		//前回のキー情報
		oldPadData_ = padData_;

		//キーボード情報の取得開始
		gamePad_->Acquire();
		//ポーリング(一定間隔でデバイスの情報を同期)
		gamePad_->Poll();
		//全キーの入力情報を取得
		result_ = gamePad_->GetDeviceState(sizeof(padData_), &padData_);
		//assert(SUCCEEDED(result));
	}
}

bool PadInput::GetJoyStickActive()
{
	if (gamePad_ != NULL)
	{

		//ジョイスティック
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
		// スティックの方向判定
	// 無反応範囲
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
		// 無反応範囲
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
