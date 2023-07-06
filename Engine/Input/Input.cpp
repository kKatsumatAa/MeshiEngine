#include "Input.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

Input::Input()
{
	//directInput¶¬
	result_ = DirectInput8Create(
		WindowsApp::GetInstance().GetWndclassEX().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result_));
}

void Input::Finalize()
{
	directInput_->Release();
}

Input& Input::GetInstance()
{
	static Input sInstance;
	return sInstance;
}

void Input::Update()
{
}
