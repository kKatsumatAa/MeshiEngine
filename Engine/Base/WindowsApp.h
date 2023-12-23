/*
* @file WindowsApp.h
* @brief Windows系の処理
*/

#pragma once
#include <Windows.h>
#include <d3d12.h>
#include<stdint.h>

#pragma warning(push)
//c5267警告を無視
#pragma warning(disable:5267)
#include <DirectXMath.h>
#include <DirectXTex.h>
#pragma warning(pop)

class WindowsApp final
{
private:
	WNDCLASSEX wndclassEX_;
	HWND hwnd_;

public:
	//ウィンドウサイズ
	static const float WINDOW_WIDTH_;
	static const float WINDOW_HEIGHT_;

	// ビューポート設定コマンド
	D3D12_VIEWPORT viewport_;


public:
	//コピーコンストラクタを無効
	WindowsApp(const WindowsApp& obj) = delete;
	//代入演算子も
	WindowsApp& operator=(const WindowsApp& obj) = delete;

	//インスタンス取得
	static WindowsApp& GetInstance();

public:
	//ウインドウプロシージャ(const &にするとエラー出る)
	static LRESULT WindowProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam);

	//コンストラクタ
	WindowsApp();
	//デストラクタ
	~WindowsApp() { ; }

public:
	//(const &にするとエラー出る)
	bool MessegeRoop(MSG msg);

	//UnregisterClassのラッパー関数
	void UnregisterClassA()
	{
		UnregisterClass(wndclassEX_.lpszClassName, wndclassEX_.hInstance);
	}

	//WNDCLASSEXを取得
	const WNDCLASSEX& GetWndclassEX() { return wndclassEX_; }
	//hwnd取得
	const HWND& Gethwnd() { return hwnd_; }
};

