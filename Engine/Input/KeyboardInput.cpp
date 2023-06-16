#include "KeyboardInput.h"

KeyboardInput::KeyboardInput()
{
	//キーボードデバイスの生成
	result_ = Input::GetInstance().GetDirectInput()->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result_));

	//入力データ形式のセット
	result_ = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result_));

	//排他制御レベルのリセット
	result_ = keyboard_->SetCooperativeLevel(
		WindowsApp::GetInstance().Gethwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//画面が手前にあるとき入力受付｜デバイスをこのアプリだけで専有しない｜Windowsキーを無効
	assert(SUCCEEDED(result_));
}

KeyboardInput::~KeyboardInput()
{
}

KeyboardInput& KeyboardInput::GetInstance()
{
	static KeyboardInput sInst; // private なコンストラクタを呼び出す。
	return sInst;
}

void KeyboardInput::Update()
{
	//前回のキー情報
	memcpy(oldkey_, key_, sizeof(key_));

	//キーボード情報の取得開始
	keyboard_->Acquire();
	//全キーの入力情報を取得
	keyboard_->GetDeviceState(sizeof(key_), key_);
}
