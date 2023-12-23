/*
* @file Input.h
* @brief 入力の取得に必要な機能
*/

#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
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
	//コンストラクタ
	Input();

public:
	//コピーコンストラクタを無効
	Input(const Input& obj) = delete;
	//代入演算子も無効
	Input& operator=(const Input& obj) = delete;

	//インスタンス取得
	static Input& GetInstance();

public:
	//終了処理
	void Finalize();
	//更新
	void Update();

	//DirectInputを取得
	IDirectInput8* GetDirectInput() { return directInput_.Get(); }
};

