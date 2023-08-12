#pragma once
#include"Input.h"
#include"Util.h"


#define CLICK_LEFT               0
#define CLICK_RIGHT              1
#define CLICK_WHEEL              2


class MouseInput final
{
private:
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	HRESULT result_;
	DIMOUSESTATE mouseData_;
	DIMOUSESTATE oldMouseData_;

	//�L�[�{�[�h�f�o�C�X�̐���
	ComPtr<IDirectInputDevice8> mouse_ = nullptr;

	const int32_t SEARCH_COUNT_MAX_ = 120;
	int32_t searchCount_ = 0;

	bool isActive_ = false;

	//�J�[�\���ʒu
	Vec2 cursorPos_ = {};

private:
	//�ڑ�����Ă��邩���ׂ�(�f�o�C�X���������肷��)
	void MouseConnectSearch();

	MouseInput();

	~MouseInput() { ; }

public:
	//�R�s�[�R���X�g���N�^�𖳌�
	MouseInput(const MouseInput& obj) = delete;
	//������Z�q��
	MouseInput& operator=(const MouseInput& obj) = delete;


	static MouseInput& GetInstance();

	void CreateDevice(bool isExclusive = false);

	void Update();

	void Finalize();

public://�Q�b�^�[

	//�W���C�X�e�B�b�N���X�����Ă��邩
	bool GetMouseActive();


	//�N���b�N����Ă��邩
	bool GetClick(byte click);
	//�N���b�N���ꂽ�u��
	bool GetTriggerClick(byte click);
	//�N���b�N�����ꂽ�u��
	bool GetTriggerReleaseClick(byte click);

	//�J�[�\���̈ʒu���擾
	const Vec2& GetCurcorPos();

	//�J�[�\���̈ړ��ʎ擾
	Vec2 GetCursorVelocity();

	//�z�C�[���̉�]��
	long GetWheelAmountOfRot();
};

