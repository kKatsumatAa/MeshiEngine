#include "Room.h"

void Room::Initialize(Vec3 pos, int roomType, float radius)
{
	this->pos = pos;
	this->roomType = roomType;
	this->radius = radius;
	isUsed = false;
}

int Room::Collision(Vec3 pos, float radius)
{
	if (CollisionBox(pos.x, pos.y, this->pos.x, this->pos.y, radius, this->radius))
	{
		return roomType;
	}

	return 0;
}
