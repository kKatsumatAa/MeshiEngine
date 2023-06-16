#include "Input.h"

Input::Input()
{
	//directInputê∂ê¨
	result_ = DirectInput8Create(
		WindowsApp::GetInstance().GetWndclassEX().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result_));
}

Input::~Input()
{
}

Input& Input::GetInstance()
{
	static Input sInstance;
	return sInstance;
}

void Input::Update()
{
}
