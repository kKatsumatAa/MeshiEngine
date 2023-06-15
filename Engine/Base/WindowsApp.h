#pragma once
#include <Windows.h>
#include <d3d12.h>
#pragma comment(lib,"winmm.lib")
#include<stdint.h>


class WindowsApp final
{
private:
	WNDCLASSEX wndclassEX_;
	HWND hwnd_;


	//ウインドウプロシージャ(const &にするとエラー出る)
	static LRESULT WindowProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam);

	WindowsApp();

	~WindowsApp() { ; }


public:
	//ウィンドウサイズ
	const float WINDOW_WIDTH_ = 1280.0f;
	const float WINDOW_HEIGHT_ = 720.0f;

	// ビューポート設定コマンド
	D3D12_VIEWPORT viewport_;


	//コピーコンストラクタを無効
	WindowsApp(const WindowsApp& obj) = delete;
	//代入演算子も
	WindowsApp& operator=(const WindowsApp& obj) = delete;

	static WindowsApp& GetInstance();

	//(const &にするとエラー出る)
	bool MessegeRoop(MSG msg);

	void UnregisterClassA()
	{
		UnregisterClass(wndclassEX_.lpszClassName, wndclassEX_.hInstance);
	}

	//getter
	const WNDCLASSEX& GetWndclassEX() { return wndclassEX_; }
	const HWND& Gethwnd() { return hwnd_; }
};

