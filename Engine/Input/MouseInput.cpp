#include "MouseInput.h"
#include "ImGuiManager.h"

// デバイス発見時に実行される
BOOL CALLBACK DeviceFindCallBackM(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	//引数は必要だが使用しないので
	pvRef;
	MouseInput::GetInstance().CreateDevice(ipddi);

	return DIENUM_STOP;
}

void MouseInput::CreateDevice(LPCDIDEVICEINSTANCE ipddi)
{
	//つながってたら呼ばれる関数なのでフラグはオンに
	isActive_ = true;

	//すでに中身があれば新たに生成しない
	if (mouse_)
	{
		return;
	}

	//デバイス作成
	HRESULT result = S_OK;
	result = Input::GetInstance().GetDirectInput()->CreateDevice(
		ipddi->guidInstance,
		&mouse_,
		NULL
	);
	assert(SUCCEEDED(result));

	//デバイスフォーマットの設定
	result = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));

	//ほかのアプリでもマウス取得させるか
	int32_t exclusive = DISCL_NONEXCLUSIVE;
	if (isExclusive_) {
		exclusive = DISCL_EXCLUSIVE;
	}

	//協調モードの設定(ウィンドウがアクティブ中のみ取得、別アプリケーションでも取得できる)
	if (FAILED(mouse_->SetCooperativeLevel(WindowsApp::GetInstance().Gethwnd(),
		DISCL_FOREGROUND | exclusive | DISCL_NOWINKEY)
	))
	{
		assert(false);
	}
}

void MouseInput::MouseConnectSearch()
{
	//毎フレーム接続確認すると重いので
	if (searchCount_ % SEARCH_COUNT_MAX_ == 0)
	{
		searchCount_ = 0;
		//いったんここでfalseにすることで接続されてないときはそのまま、されていたらtrue
		isActive_ = false;

		// デバイスの列挙（接続されてるかを確認）
		DeviceEnumerationAndCreate();

		//パッドが接続されてないかつ、前までされていたら削除
		if (mouse_ && !isActive_)
		{
			DeleteDevice();
		}
	}

	searchCount_++;
}

void MouseInput::DeviceEnumerationAndCreate()
{
	// デバイスの列挙（接続されてるかを確認）
	result_ = Input::GetInstance().GetDirectInput()->EnumDevices(
		DI8DEVTYPE_MOUSE,
		DeviceFindCallBackM,//接続されていればこの関数が呼ばれる
		NULL,
		DIEDFL_ATTACHEDONLY//
	);
	assert(SUCCEEDED(result_));
}

void MouseInput::DeleteDevice()
{
	mouse_->Unacquire();
	mouse_->Release();
	mouse_.Reset();
}

MouseInput::MouseInput()
{
	result_ = {};
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
	//接続を確認
	MouseConnectSearch();

	if (mouse_ != NULL)
	{
		//前回の情報
		oldMouseData_ = mouseData_;

		//情報の取得開始
		mouse_->Acquire();
		//ポーリング(一定間隔でデバイスの情報を同期)
		mouse_->Poll();
		//入力情報を取得
		mouse_->GetDeviceState(sizeof(mouseData_), &mouseData_);

		//カーソルの位置取得
		POINT p;
		GetCursorPos(&p);
		//スクリーン座標をクライアント座標に変える
		ScreenToClient(WindowsApp::GetInstance().Gethwnd(), &p);

		cursorPos_.x = (float)p.x;
		cursorPos_.y = (float)p.y;
	}
}

bool MouseInput::GetMouseActive()
{
	if (mouse_ != NULL)
	{

		//ジョイスティック
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

void MouseInput::SetIsExclusive(bool is)
{
	//設定を変えて新たにデバイス作り直さなきゃいけないので
	if (is != isExclusive_)
	{
		isExclusive_ = is;

		//元のデバイス削除
		if (mouse_)
		{
			DeleteDevice();
		}

		// デバイスの列挙（接続されてるかを確認）
		DeviceEnumerationAndCreate();
	}
}
