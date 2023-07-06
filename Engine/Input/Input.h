#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include "WindowsApp.h"

#include <wrl.h>


class Input final
{
private:
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	HRESULT result_;

	//DirectInput�̏�����
	ComPtr<IDirectInput8> directInput_ = nullptr;

private:
	Input();

public:
	//�R�s�[�R���X�g���N�^�𖳌�
	Input(const Input& obj) = delete;
	//������Z�q��
	Input& operator=(const Input& obj) = delete;

	void Finalize();

	static Input& GetInstance();

	void Update();


	IDirectInput8* GetDirectInput() { return directInput_.Get(); }
};

