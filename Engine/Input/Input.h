#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include "WindowsApp.h"

#include <wrl.h>


class Input final
{
private:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	HRESULT result_;

	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput_ = nullptr;

private:
	Input();

public:
	//コピーコンストラクタを無効
	Input(const Input& obj) = delete;
	//代入演算子も
	Input& operator=(const Input& obj) = delete;

	void Finalize();

	static Input& GetInstance();

	void Update();


	IDirectInput8* GetDirectInput() { return directInput_.Get(); }
};

