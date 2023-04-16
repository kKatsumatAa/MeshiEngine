#include "Input.h"

Input::Input()
{
	//directInputê∂ê¨
	result = DirectInput8Create(
		WindowsApp::GetInstance().Getw().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
}

Input::~Input()
{
}

Input& Input::GetInstance()
{
	static Input instance;
	return instance;
}

void Input::Update()
{
}
