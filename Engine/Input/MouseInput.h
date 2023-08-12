#pragma once
#include"Input.h"
#include"Util.h"


#define CLICK_LEFT               0
#define CLICK_RIGHT              1
#define CLICK_WHEEL              2


class MouseInput final
{
private:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	HRESULT result_;
	DIMOUSESTATE mouseData_;
	DIMOUSESTATE oldMouseData_;

	//キーボードデバイスの生成
	ComPtr<IDirectInputDevice8> mouse_ = nullptr;

	const int32_t SEARCH_COUNT_MAX_ = 120;
	int32_t searchCount_ = 0;

	bool isActive_ = false;

	//カーソル位置
	Vec2 cursorPos_ = {};

private:
	//接続されているか調べる(デバイス生成したりする)
	void MouseConnectSearch();

	MouseInput();

	~MouseInput() { ; }

public:
	//コピーコンストラクタを無効
	MouseInput(const MouseInput& obj) = delete;
	//代入演算子も
	MouseInput& operator=(const MouseInput& obj) = delete;


	static MouseInput& GetInstance();

	void CreateDevice(bool isExclusive = false);

	void Update();

	void Finalize();

public://ゲッター

	//ジョイスティックが傾けられているか
	bool GetMouseActive();


	//クリックされているか
	bool GetClick(byte click);
	//クリックされた瞬間
	bool GetTriggerClick(byte click);
	//クリック離された瞬間
	bool GetTriggerReleaseClick(byte click);

	//カーソルの位置を取得
	const Vec2& GetCurcorPos();

	//カーソルの移動量取得
	Vec2 GetCursorVelocity();

	//ホイールの回転量
	long GetWheelAmountOfRot();
};

