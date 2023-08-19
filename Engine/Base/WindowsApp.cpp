#include "WindowsApp.h"
#include<imgui_impl_win32.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


const float WindowsApp::WINDOW_WIDTH_ = 1280.0f;
const float WindowsApp::WINDOW_HEIGHT_ = 720.0f;


// imgui��p�̃E�B���h�E�v���V�[�W��
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam,
	LPARAM lParam);


LRESULT WindowsApp::WindowProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam) {
	
	//imgui�p�̃E�B���h�E�v���V�[�W���Ăяo��
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}
	
	//���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
	switch (msg) {
		//�E�B���h�E���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��āA�A�v���̏C����`����
		PostQuitMessage(0);
		return 0;
	}

	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WindowsApp::WindowsApp()
{
	//�V�X�e���^�C�}�[�̕���\���グ��
	timeBeginPeriod(1);

	//�E�C���h�E�N���X�̐ݒ�
	wndclassEX_.cbSize = sizeof(WNDCLASSEX);
	wndclassEX_.lpfnWndProc = (WNDPROC)WindowProc;		//�E�C���h�E�v���V�[�W���̐ݒ�
	wndclassEX_.lpszClassName = L"DirectX";				//�E�C���h�E�N���X��
	wndclassEX_.hInstance = GetModuleHandle(nullptr);		//�E�C���h�E�n���h��
	wndclassEX_.hCursor = LoadCursor(NULL, IDC_ARROW);	//�J�[�\���w��

	//�E�C���h�E�N���X��OS�ɓo�^����
	RegisterClassEx(&wndclassEX_);
	//�E�C���h�E�T�C�Y{X,Y,����,�c��}
	RECT wrc = { 0,0,(long)WINDOW_WIDTH_,(long)WINDOW_HEIGHT_ };
	//�����ŃT�C�Y�␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	hwnd_ = CreateWindow(
		wndclassEX_.lpszClassName,		//�N���X��
		L"LE3A_03_�C�C�__���E�L",			//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	//�W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,			//�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,			//�\��Y���W(OS�ɔC����)
		wrc.right - wrc.left,	//�E�B���h�E����
		wrc.bottom - wrc.top,	//�E�B���h�E�c��
		nullptr,				//�e�E�B���h�E�n���h��
		nullptr,				//���j���[�n���h��
		wndclassEX_.hInstance,			//�Ăяo���A�v���P�[�V�����n���h��
		nullptr					//�I�v�V����
	);

	//�E�C���h�E��\����Ԃɂ���
	ShowWindow(hwnd_, SW_SHOW);

	viewport_ = { 0, 0, WINDOW_WIDTH_, WINDOW_HEIGHT_, 0.0f, 1.0f };
}

WindowsApp& WindowsApp::GetInstance()
{
	static WindowsApp sInst; // private �ȃR���X�g���N�^���Ăяo���B
	return sInst;
}

bool WindowsApp::MessegeRoop(MSG msg)
{
	//���b�Z�[�W�����邩?
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);	//�L�[���̓��b�Z�[�W�̏���
		DispatchMessage(&msg);	//�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
	}

	//�~�{�^���ŏI��
	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}
