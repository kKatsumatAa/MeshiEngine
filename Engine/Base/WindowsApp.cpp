#include "WindowsApp.h"
#include<imgui_impl_win32.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


const float WindowsApp::WINDOW_WIDTH_ = 1280.0f;
const float WindowsApp::WINDOW_HEIGHT_ = 720.0f;


// imgui専用のウィンドウプロシージャ
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam,
	LPARAM lParam);


LRESULT WindowsApp::WindowProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam) {
	
	//imgui用のウィンドウプロシージャ呼び出し
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}
	
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

WindowsApp::WindowsApp()
{
	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	//ウインドウクラスの設定
	wndclassEX_.cbSize = sizeof(WNDCLASSEX);
	wndclassEX_.lpfnWndProc = (WNDPROC)WindowProc;		//ウインドウプロシージャの設定
	wndclassEX_.lpszClassName = L"DirectX";				//ウインドウクラス名
	wndclassEX_.hInstance = GetModuleHandle(nullptr);		//ウインドウハンドル
	wndclassEX_.hCursor = LoadCursor(NULL, IDC_ARROW);	//カーソル指定

	//ウインドウクラスをOSに登録する
	RegisterClassEx(&wndclassEX_);
	//ウインドウサイズ{X,Y,横幅,縦幅}
	RECT wrc = { 0,0,(long)WINDOW_WIDTH_,(long)WINDOW_HEIGHT_ };
	//自動でサイズ補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd_ = CreateWindow(
		wndclassEX_.lpszClassName,		//クラス名
		L"LE3A_03_イイダ_ユウキ",			//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//標準的なウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標(OSに任せる)
		CW_USEDEFAULT,			//表示Y座標(OSに任せる)
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		wndclassEX_.hInstance,			//呼び出しアプリケーションハンドル
		nullptr					//オプション
	);

	//ウインドウを表示状態にする
	ShowWindow(hwnd_, SW_SHOW);

	viewport_ = { 0, 0, WINDOW_WIDTH_, WINDOW_HEIGHT_, 0.0f, 1.0f };
}

WindowsApp& WindowsApp::GetInstance()
{
	static WindowsApp sInst; // private なコンストラクタを呼び出す。
	return sInst;
}

bool WindowsApp::MessegeRoop(MSG msg)
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
