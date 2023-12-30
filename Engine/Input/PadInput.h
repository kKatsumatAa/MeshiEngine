/*
* @file PadInput.h
* @brief ゲームパッドの入力
*/

#pragma once
#include"Input.h"
#include"Util.h"


#define GAMEPAD_A                0
#define GAMEPAD_B                1
#define GAMEPAD_X                2
#define GAMEPAD_Y                3
#define GAMEPAD_LEFT_SHOULDER    4
#define GAMEPAD_RIGHT_SHOULDER   5
#define GAMEPAD_SELECT           6
#define GAMEPAD_START            7
#define GAMEPAD_LEFT_STICK       8
#define GAMEPAD_RIGHT_STICK      9

#define GAMEPAD_ARROW_UP          33
#define GAMEPAD_ARROW_DOWN        34
#define GAMEPAD_ARROW_LEFT        35
#define GAMEPAD_ARROW_RIGHT       36


class PadInput final
{
private:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	HRESULT result_;
	DIJOYSTATE padData_;
	DIJOYSTATE oldPadData_;
	LPVOID pvRef_;

	//キーボードデバイスの生成
	ComPtr<IDirectInputDevice8> gamePad_ = nullptr;

	const float STICK_RANGE_ = 1000;
	const float STICK_R_RANGE_ = 65535 / 2.0f;
	const float STICK_RANGE_RATE_ = 1.0f / 5.0f;

	const int32_t SEARCH_COUNT_MAX_ = 120;
	int32_t searchCount_ = 0;

	bool isActive_ = false;


public:
	//コピーコンストラクタを無効
	PadInput(const PadInput& obj) = delete;
	//代入演算子も
	PadInput& operator=(const PadInput& obj) = delete;

	//インスタンス取得
	static PadInput& GetInstance();

private:
	//コンストラクタ
	PadInput();
	//デストラクタ
	~PadInput() { ; }

private:
	//接続されているか調べる(デバイス生成したりする)
	void PadConnectSearch();

public:
	//デバイス作成
	void CreateDevice(LPCDIDEVICEINSTANCE ipddi);
	//更新
	void Update();
	//終了処理
	void Finalize();

public://ゲッター
	//ジョイスティックが傾けられているか
	bool GetJoyStickActive();
	//左ジョイスティックがどのくらい倒されているか(0.0~1.0f)
	Vec2 GetLeftStickTilt();
	//右ジョイスティックがどのくらい倒されているか(0.0~1.0f)
	Vec2 GetRightStickTilt();

	//ボタンが押されているか
	bool GetPushButton(BYTE button);
	//ボタンが押された瞬間か
	bool GetTriggerButton(BYTE button);
	//ボタンが離された瞬間か
	bool GetReleaseTrigger(BYTE button);
};

