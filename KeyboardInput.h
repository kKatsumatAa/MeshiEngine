#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include "WindowsApp.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <wrl.h>


class KeyboardInput
{
private:
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//�S�L�[�̓��͏�Ԃ��擾����
	BYTE key[256] = {};
	BYTE oldkey[256] = {};
	HRESULT result;

	//DirectInput�̏�����
	ComPtr<IDirectInput8> directInput = nullptr;
	//�L�[�{�[�h�f�o�C�X�̐���
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

private:
	KeyboardInput()
	{
		result = DirectInput8Create(
			 WindowsApp::GetInstance().w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
			(void**)&directInput, nullptr);
		assert(SUCCEEDED(result));

		//�L�[�{�[�h�f�o�C�X�̐���
		result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
		assert(SUCCEEDED(result));

		//���̓f�[�^�`���̃Z�b�g
		result = keyboard->SetDataFormat(&c_dfDIKeyboard);
		assert(SUCCEEDED(result));

		//�r�����䃌�x���̃��Z�b�g
		result = keyboard->SetCooperativeLevel(
			 WindowsApp::GetInstance().hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		//��ʂ���O�ɂ���Ƃ����͎�t�b�f�o�C�X�����̃A�v�������Ő�L���Ȃ��bWindows�L�[�𖳌�
		assert(SUCCEEDED(result));
	}

public:

	static KeyboardInput& GetInstance()
	{
		static KeyboardInput inst; // private �ȃR���X�g���N�^���Ăяo���B
		return inst;
	}

	void Update()
	{
		//�O��̃L�[���
		for (int i = 0; i < 256; i++)
		{
			oldkey[i] = key[i];
		}

		//�L�[�{�[�h���̎擾�J�n
		keyboard->Acquire();
		//�S�L�[�̓��͏����擾
		keyboard->GetDeviceState(sizeof(key), key);
	}

	//�g���K�[�p
	bool keyPush(BYTE keys)
	{
		return key[keys];
	}
	bool keyRelease(BYTE keys)
	{
		return !key[keys];
	}
	bool keyTrigger(BYTE keys)
	{
		return (key[keys] && !oldkey[keys]);
	}
	bool keyReleaseTrigger(BYTE keys)
	{
		return (!key[keys] && oldkey[keys]);
	}
};