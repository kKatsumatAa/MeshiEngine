/*
* @file KeyboardInput.h
* @brief キーボードの入力
*/

#pragma once
#include"Input.h"


class KeyboardInput final
{
private:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//全キーの入力状態を取得する
	byte key_[256] = {};
	byte oldkey_[256] = {};
	HRESULT result_;

	//キーボードデバイスの生成
	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;

private:
	//コンストラクタ
	KeyboardInput();
	//デストラクタ
	~KeyboardInput();

public:
	//コピーコンストラクタを無効
	KeyboardInput(const KeyboardInput& obj) = delete;
	//代入演算子も
	KeyboardInput& operator=(const KeyboardInput& obj) = delete;

	//インスタンス取得
	static KeyboardInput& GetInstance();

public:
	//更新
	void Update();

public:
	//キーが押されているか
	bool KeyPush(byte keys)
	{
		return key_[keys];
	}
	//キーが離されているか
	bool KeyRelease(byte keys)
	{
		return !key_[keys];
	}
	//キーが押された瞬間か
	bool KeyTrigger(byte keys)
	{
		return (key_[keys] && !oldkey_[keys]);
	}
	//キーが話された瞬間か
	bool KeyReleaseTrigger(byte keys)
	{
		return (!key_[keys] && oldkey_[keys]);
	}
};