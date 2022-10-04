#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include "WindowsApp.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <wrl.h>


class KeyboardInput
{
private:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//全キーの入力状態を取得する
	BYTE key[256] = {};
	BYTE oldkey[256] = {};
	HRESULT result;

	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	//キーボードデバイスの生成
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

private:
	KeyboardInput()
	{
		result = DirectInput8Create(
			 WindowsApp::GetInstance().w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
			(void**)&directInput, nullptr);
		assert(SUCCEEDED(result));

		//キーボードデバイスの生成
		result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
		assert(SUCCEEDED(result));

		//入力データ形式のセット
		result = keyboard->SetDataFormat(&c_dfDIKeyboard);
		assert(SUCCEEDED(result));

		//排他制御レベルのリセット
		result = keyboard->SetCooperativeLevel(
			 WindowsApp::GetInstance().hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		//画面が手前にあるとき入力受付｜デバイスをこのアプリだけで専有しない｜Windowsキーを無効
		assert(SUCCEEDED(result));
	}

public:

	static KeyboardInput& GetInstance()
	{
		static KeyboardInput inst; // private なコンストラクタを呼び出す。
		return inst;
	}

	void Update()
	{
		//前回のキー情報
		for (int i = 0; i < 256; i++)
		{
			oldkey[i] = key[i];
		}

		//キーボード情報の取得開始
		keyboard->Acquire();
		//全キーの入力情報を取得
		keyboard->GetDeviceState(sizeof(key), key);
	}

	//トリガー用
	bool keyPush(BYTE keys)
	{
		return key[keys];
	}
	bool keyRelease(BYTE keys)
	{
		return !key[keys];
	}
	bool keyTrigger(BYTE keys)
	{
		return (key[keys] && !oldkey[keys]);
	}
	bool keyReleaseTrigger(BYTE keys)
	{
		return (!key[keys] && oldkey[keys]);
	}
};