#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include "WindowsApp.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <wrl.h>


class Input final
{
private:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	HRESULT result;

	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;

private:
	Input();

	~Input();

public:
	//コピーコンストラクタを無効
	Input(const Input& obj) = delete;
	//代入演算子も
	Input& operator=(const Input& obj) = delete;


	static Input& GetInstance();

	void Update();


	IDirectInput8* GetDirectInput() { return directInput.Get(); }
};

