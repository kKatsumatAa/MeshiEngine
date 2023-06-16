#include "KeyboardInput.h"

KeyboardInput::KeyboardInput()
{
	//�L�[�{�[�h�f�o�C�X�̐���
	result_ = Input::GetInstance().GetDirectInput()->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result_));

	//���̓f�[�^�`���̃Z�b�g
	result_ = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result_));

	//�r�����䃌�x���̃��Z�b�g
	result_ = keyboard_->SetCooperativeLevel(
		WindowsApp::GetInstance().Gethwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//��ʂ���O�ɂ���Ƃ����͎�t�b�f�o�C�X�����̃A�v�������Ő�L���Ȃ��bWindows�L�[�𖳌�
	assert(SUCCEEDED(result_));
}

KeyboardInput::~KeyboardInput()
{
}

KeyboardInput& KeyboardInput::GetInstance()
{
	static KeyboardInput sInst; // private �ȃR���X�g���N�^���Ăяo���B
	return sInst;
}

void KeyboardInput::Update()
{
	//�O��̃L�[���
	memcpy(oldkey_, key_, sizeof(key_));

	//�L�[�{�[�h���̎擾�J�n
	keyboard_->Acquire();
	//�S�L�[�̓��͏����擾
	keyboard_->GetDeviceState(sizeof(key_), key_);
}
