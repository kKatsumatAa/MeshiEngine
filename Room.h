#pragma once
#include"Util.h"


enum ROOM_TYPE
{
	RIGHT_ROOM = 1,
	LEFT_ROOM
};

//移動先の部屋ではなくステージ上の左右にある方の部屋
class Room
{
private:
	bool isUsed = false;
	Vec3 pos;
	int roomType;
	float radius;

public:
	void Initialize(Vec3 pos, int roomType, float radius);

	//部屋と当たっていた場合部屋のタイプを返す
	int Collision(Vec3 pos, float radius);

	void SetIsUsed(bool is) { isUsed = is; }
	bool GetIsUsed() { return isUsed; }

	void SetRoomType(int roomType) { this->roomType = roomType; }
	int GetRoomType() { return roomType; }
};

