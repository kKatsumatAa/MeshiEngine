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

	HRESULT result;
	DIJOYSTATE padData;
	DIJOYSTATE oldPadData;

	//キーボードデバイスの生成
	ComPtr<IDirectInputDevice8> gamePad = nullptr;

	const float stickRange = 1000;
	const float stickRRange = 65535 / 2.0f;

	const int searchCountMax = 120;
	int searchCount = 0;

	bool isActive = false;

private:
	//接続されているか調べる(デバイス生成したりする)
	void PadConnectSearch();

	PadInput();

	~PadInput();

public:
	//コピーコンストラクタを無効
	PadInput(const PadInput& obj) = delete;
	//代入演算子も
	PadInput& operator=(const PadInput& obj) = delete;


	static PadInput& GetInstance();

	void CreateDevice();

	void Update();

public://ゲッター

	//ジョイスティックが傾けられているか
	bool GetJoyStickActive();
	//ジョイスティックがどのくらい倒されているか(0.0~1.0f)
	Vec2 GetLeftStickTilt();
	Vec2 GetRightStickTilt();

	//ボタンが押されているか
	bool GetPushButton(BYTE button);
	bool GetTriggerButton(BYTE button);
	bool GetReleaseTrigger(BYTE button);
};

