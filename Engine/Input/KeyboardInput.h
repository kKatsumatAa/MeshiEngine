#pragma once
#include"Input.h"


class KeyboardInput final
{
private:
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//�S�L�[�̓��͏�Ԃ��擾����
	byte key_[256] = {};
	byte oldkey_[256] = {};
	HRESULT result_;

	//�L�[�{�[�h�f�o�C�X�̐���
	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;

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
	bool KeyPush(byte keys)
	{
		return key_[keys];
	}
	bool KeyRelease(byte keys)
	{
		return !key_[keys];
	}
	bool KeyTrigger(byte keys)
	{
		return (key_[keys] && !oldkey_[keys]);
	}
	bool KeyReleaseTrigger(byte keys)
	{
		return (!key_[keys] && oldkey_[keys]);
	}
};