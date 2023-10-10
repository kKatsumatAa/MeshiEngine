#pragma once
#include <Windows.h>
#include <d3d12.h>
#include<stdint.h>

#pragma warning(push)
//c5267隴ｦ蜻翫ｒ辟｡隕・
#pragma warning(disable:5267)
#include <DirectXMath.h>
#include <DirectXTex.h>
#pragma warning(pop)

class WindowsApp final
{
private:
	WNDCLASSEX wndclassEX_;
	HWND hwnd_;


	//繧ｦ繧､繝ｳ繝峨え繝励Ο繧ｷ繝ｼ繧ｸ繝｣(const &縺ｫ縺吶ｋ縺ｨ繧ｨ繝ｩ繝ｼ蜃ｺ繧・
	static LRESULT WindowProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam);

	WindowsApp();

	~WindowsApp() { ; }


public:
	//繧ｦ繧｣繝ｳ繝峨え繧ｵ繧､繧ｺ
	static const float WINDOW_WIDTH_;
	static const float WINDOW_HEIGHT_;

	// 繝薙Η繝ｼ繝昴・繝郁ｨｭ螳壹さ繝槭Φ繝・
	D3D12_VIEWPORT viewport_;


	//繧ｳ繝斐・繧ｳ繝ｳ繧ｹ繝医Λ繧ｯ繧ｿ繧堤┌蜉ｹ
	WindowsApp(const WindowsApp& obj) = delete;
	//莉｣蜈･貍皮ｮ怜ｭ舌ｂ
	WindowsApp& operator=(const WindowsApp& obj) = delete;

	static WindowsApp& GetInstance();

	//(const &縺ｫ縺吶ｋ縺ｨ繧ｨ繝ｩ繝ｼ蜃ｺ繧・
	bool MessegeRoop(MSG msg);

	void UnregisterClassA()
	{
		UnregisterClass(wndclassEX_.lpszClassName, wndclassEX_.hInstance);
	}

	//getter
	const WNDCLASSEX& GetWndclassEX() { return wndclassEX_; }
	const HWND& Gethwnd() { return hwnd_; }
};

