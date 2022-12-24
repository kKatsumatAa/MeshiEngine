#pragma once
#include"Util.h"


enum ROOM_TYPE
{
	RIGHT_ROOM = 1,
	LEFT_ROOM
};

//�ړ���̕����ł͂Ȃ��X�e�[�W��̍��E�ɂ�����̕���
class Room
{
private:
	bool isUsed = false;
	Vec3 pos;
	int roomType;
	float radius;

public:
	void Initialize(Vec3 pos, int roomType, float radius);

	//�����Ɠ������Ă����ꍇ�����̃^�C�v��Ԃ�
	int Collision(Vec3 pos, float radius);

	void SetIsUsed(bool is) { isUsed = is; }
	bool GetIsUsed() { return isUsed; }

	void SetRoomType(int roomType) { this->roomType = roomType; }
	int GetRoomType() { return roomType; }
};

