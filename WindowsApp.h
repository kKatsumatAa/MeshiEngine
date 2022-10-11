#pragma once
#include <Windows.h>
#include <d3d12.h>




class WindowsApp
{
private:
	WNDCLASSEX w;
	HWND hwnd;



	//ウインドウプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		//メッセージに応じてゲーム固有の処理を行う
		switch (msg) {
			//ウィンドウが破棄された
		case WM_DESTROY:
			//OSに対して、アプリの修了を伝える
			PostQuitMessage(0);
			return 0;
		}

		//標準のメッセージ処理を行う
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	WindowsApp()
	{
		//ウインドウクラスの設定
		w.cbSize = sizeof(WNDCLASSEX);
		w.lpfnWndProc = (WNDPROC)WindowProc;		//ウインドウプロシージャの設定
		w.lpszClassName = L"DirectX";				//ウインドウクラス名
		w.hInstance = GetModuleHandle(nullptr);		//ウインドウハンドル
		w.hCursor = LoadCursor(NULL, IDC_ARROW);	//カーソル指定

		//ウインドウクラスをOSに登録する
		RegisterClassEx(&w);
		//ウインドウサイズ{X,Y,横幅,縦幅}
		RECT wrc = { 0,0,(LONG)window_width,(LONG)window_height };
		//自動でサイズ補正
		AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

		//ウィンドウオブジェクトの生成
		hwnd = CreateWindow(
			w.lpszClassName,		//クラス名
			L"フライレーズン",			//タイトルバーの文字
			WS_OVERLAPPEDWINDOW,	//標準的なウィンドウスタイル
			CW_USEDEFAULT,			//表示X座標(OSに任せる)
			CW_USEDEFAULT,			//表示Y座標(OSに任せる)
			wrc.right - wrc.left,	//ウィンドウ横幅
			wrc.bottom - wrc.top,	//ウィンドウ縦幅
			nullptr,				//親ウィンドウハンドル
			nullptr,				//メニューハンドル
			w.hInstance,			//呼び出しアプリケーションハンドル
			nullptr					//オプション
		);

		//ウインドウを表示状態にする
		ShowWindow(hwnd, SW_SHOW);

		viewport = { 0, 0, window_width, window_height, 0.0f, 1.0f };
	}


public:
	//ウィンドウサイズ
	const float window_width = 1280.0f;
	const float window_height = 720.0f;

	// ビューポート設定コマンド
	D3D12_VIEWPORT viewport;


	static WindowsApp& GetInstance()
	{
		static WindowsApp inst; // private なコンストラクタを呼び出す。
		return inst;
	}

	bool MessegeRoop(MSG msg)
	{
		//メッセージがあるか?
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);	//キー入力メッセージの処理
			DispatchMessage(&msg);	//プロシージャにメッセージを送る
		}

		//×ボタンで終了
		if (msg.message == WM_QUIT) {
			return true;
		}

		return false;
	}

	void UnregisterClassA()
	{
		UnregisterClass(w.lpszClassName, w.hInstance);
	}

	void operator=(const WindowsApp& win)
	{
		*this = win;
	}

	//getter
	WNDCLASSEX Getw() { return w; }
	HWND Gethwnd() { return hwnd; }
};
