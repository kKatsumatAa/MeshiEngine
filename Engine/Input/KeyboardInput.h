#pragma once
#include"Input.h"


class KeyboardInput final
{
private:
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//�S�L�[�̓��͏�Ԃ��擾����
	BYTE key[256] = {};
	BYTE oldkey[256] = {};
	HRESULT result;

	//�L�[�{�[�h�f�o�C�X�̐���
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

private:
	KeyboardInput();

	~KeyboardInput();

public:
	//�R�s�[�R���X�g���N�^�𖳌�
	KeyboardInput(const KeyboardInput& obj) = delete;
	//������Z�q��
	KeyboardInput& operator=(const KeyboardInput& obj) = delete;


	static KeyboardInput& GetInstance();

	void Update();


	//�g���K�[�p
	bool KeyPush(BYTE keys)
	{
		return key[keys];
	}
	bool KeyRelease(BYTE keys)
	{
		return !key[keys];
	}
	bool KeyTrigger(BYTE keys)
	{
		return (key[keys] && !oldkey[keys]);
	}
	bool KeyReleaseTrigger(BYTE keys)
	{
		return (!key[keys] && oldkey[keys]);
	}
};