#pragma once
#include"Framework.h"


//�Q�[���S��
class MyGame : public Framework
{
private:


public:
	//������
	void Initialize() override;

	//�I��
	void Finalize() override;

	//���t���[���X�V
	bool Update() override;

	//�`��
	void Draw() override;
};

