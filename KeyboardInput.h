#pragma once
#include"Input.h"


class KeyboardInput final
{
private:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//全キーの入力状態を取得する
	BYTE key[256] = {};
	BYTE oldkey[256] = {};
	HRESULT result;

	//キーボードデバイスの生成
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

private:
	KeyboardInput();

	~KeyboardInput();

public:
	//コピーコンストラクタを無効
	KeyboardInput(const KeyboardInput& obj) = delete;
	//代入演算子も
	KeyboardInput& operator=(const KeyboardInput& obj) = delete;


	static KeyboardInput& GetInstance();

	void Update();


	//トリガー用
	bool KeyPush(BYTE keys)
	{
		return key[keys];
	}
	bool KeyRelease(BYTE keys)
	{
		return !key[keys];
	}
	bool KeyTrigger(BYTE keys)
	{
		return (key[keys] && !oldkey[keys]);
	}
	bool KeyReleaseTrigger(BYTE keys)
	{
		return (!key[keys] && oldkey[keys]);
	}
};