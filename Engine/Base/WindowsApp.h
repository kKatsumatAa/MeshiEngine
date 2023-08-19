#pragma once
#include <Windows.h>
#include <d3d12.h>
#include<stdint.h>


class WindowsApp final
{
private:
	WNDCLASSEX wndclassEX_;
	HWND hwnd_;


	//�E�C���h�E�v���V�[�W��(const &�ɂ���ƃG���[�o��)
	static LRESULT WindowProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam);

	WindowsApp();

	~WindowsApp() { ; }


public:
	//�E�B���h�E�T�C�Y
	static const float WINDOW_WIDTH_;
	static const float WINDOW_HEIGHT_;

	// �r���[�|�[�g�ݒ�R�}���h
	D3D12_VIEWPORT viewport_;


	//�R�s�[�R���X�g���N�^�𖳌�
	WindowsApp(const WindowsApp& obj) = delete;
	//������Z�q��
	WindowsApp& operator=(const WindowsApp& obj) = delete;

	static WindowsApp& GetInstance();

	//(const &�ɂ���ƃG���[�o��)
	bool MessegeRoop(MSG msg);

	void UnregisterClassA()
	{
		UnregisterClass(wndclassEX_.lpszClassName, wndclassEX_.hInstance);
	}

	//getter
	const WNDCLASSEX& GetWndclassEX() { return wndclassEX_; }
	const HWND& Gethwnd() { return hwnd_; }
};

